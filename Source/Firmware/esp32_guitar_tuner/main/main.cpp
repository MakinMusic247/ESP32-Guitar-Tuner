#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

// added libraries
#include "driver/timer.h"
#include "freertos/queue.h"
#include <driver/adc.h>
#include <cstring>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "esp_log.h"
#include <limits>

// custom components
#include "DFT/dft.h"

/*
    GPIO channels
    GPOI36 --> ADC1_CHANNEL_0
    GPIO37 --> ADC1_CHANNEL_1
    GPIO32 --> ADC1_CHANNEL_4
    GPIO33 --> ADC1_CHANNEL_5
    GPIO34 --> ADC1_CHANNEL_6
*/
#define ADC_IN          ADC1_CHANNEL_0           // define our analog input
#define sampling_rate   5000                     // sampling rate of ADC (Hz). Note: must be > the highest frequency of the signal we wish to observe
#define N               512                     // buffer size should be a power of 2

float in_buffer[N] = {0.0F};                       // buffer to store incoming analog signal
float out_buffer[N] = {0.0F};                      // buffer to copy in_buffer into for processing
float* dft_buffer = new float[N];               // buffer to hold DFT samples
float window[N] = {0.0F};

int sampleCount = 0;
bool buffFull = false; // If buffer to store data is full
int timer_period = 1000000 / sampling_rate;     // Timer interrupt period
static intr_handle_t s_timer_handle;

bool DEBUG_MODE = true;


extern "C" { 
    void app_main(); 
}


/* Read analog input signal and store in buffer */
void analog_read(){
    int input = adc1_get_raw(ADC_IN);
    float adc_volt = (3.3 * input) / 4096;

    if(DEBUG_MODE){
        ets_printf("%d\n", input);

    } else{
        // if(sampleCount >= N) return;

        in_buffer[sampleCount] = adc_volt;
        sampleCount++;
    }
    

    
}


/* Interrupt Service Routine. Called every x milliseconds to sample data */
void IRAM_ATTR timer_isr(void* arg)
{
    TIMERG0.int_clr_timers.t0 = 1;
    TIMERG0.hw_timer[0].config.alarm_en = 1;
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;

    // Main task starts here, get sensor data
	analog_read();

    // if buffered samples are collected
    if(sampleCount == N) {
        buffFull = true;
    }
}


/* Initialize timer interrupt for given sampling rate */
void initialize_timer(int timer_period_us)
{
    timer_config_t config = {
            .alarm_en = TIMER_ALARM_EN,
            .counter_en = TIMER_PAUSE,
            .intr_type = TIMER_INTR_LEVEL,
            .counter_dir = TIMER_COUNT_UP,
            .auto_reload = TIMER_AUTORELOAD_EN,
            .divider = 80   /* 1 us per tick */
    };
    
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, timer_period_us);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_isr, NULL, 0, &s_timer_handle);

    timer_start(TIMER_GROUP_0, TIMER_0);
}


void generate_sinusoid(bool add_noise){

    double m_phase=0;
    int f=40;
    int fs=2500;
    double phaseInc = 2 * M_PI * f / fs;

    for(int i=0; i<N; i++){
        in_buffer[i] = cos(m_phase);
        m_phase = m_phase + phaseInc;
    }
}


esp_err_t generate_tone(int len, float Ampl, float freq, float phase)
{
    // freq must be between -1 and 1
    float ph = phase / 180 * M_PI;
    float fr  = 2 * M_PI * freq;
    for (int i = 0 ; i < len ; i++) {
        in_buffer[i] = Ampl * sin(ph);
        ph += fr;
        if (ph > 2 * M_PI) {
            ph -= 2 * M_PI;
        }
        if (ph < -2 * M_PI) {
            ph += 2 * M_PI;
        }
    }
    return ESP_OK;
}


/* 
    Apply a Hanning window to the signal to remove sharp edges
    Hanning window is of the form: 0.5 * (1 - cos(i * 2 * M_PI * 1/(N-1)))
 */
void hanning_window()
{
    float len_mult = 1/(float)(N-1);
    for (int i = 0; i < N; i++) {
        window[i] = 0.5 * (1 - cosf(i * 2 * M_PI * len_mult));
    }
}


void apply_window(){
    for (int i = 0; i < N; i++) {
        // double multiplier = 0.5 * (1 - cos(2 * M_PI * i/N));
        out_buffer[i] = in_buffer[i] * window[i];
    }
}



void discrete_fourier_transform(){
    printf("Performing DFT\n");
    // 1. Allocate buffers to store the real and imaginary components
    float* dft_real = new float[N];
    float* dft_imag = new float[N];

    // 2. copy samples into the dft 'real' buffer
    for(int i = 0; i<N; i++){
        dft_real[i] = out_buffer[i];
        dft_imag[i] = 0.0F;
    }

    // 3. Initialize the DFT object
    my_dft _dft;
    _dft.init(N, true);
    _dft.perform_transform(dft_real, dft_imag, 1);

    // 4. Display result
    for(int k = 0; k < N; k++){
        printf("X[%d] = %.2f - %.2fi\n", k, (float)dft_real[k], (float)dft_imag[k]);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

}



void app_main(void)
{
    printf("Starting\n");

    

    if(DEBUG_MODE){ // test out DFT function with code generated tone
        // 1. generate a test sinusoidal signal
        generate_tone(N, 1.0, 0.16, 0); 

        // 2. apply Hanning window to the input signal
        hanning_window();
        apply_window();
        for(int i=0; i<200; i++){ printf("x[%d] = %.2f\n", i, out_buffer[i]); vTaskDelay(1 / portTICK_PERIOD_MS); }

        // 3. perform DFT
        vTaskDelay(500 / portTICK_PERIOD_MS);
        discrete_fourier_transform();
        vTaskDelay(500 / portTICK_PERIOD_MS);

        printf("Finished\n");
    }

    else{
        // configure ADC
        adc1_config_width(ADC_WIDTH_BIT_10); // ADC_WIDTH_BIT_12 = 0~4095 ; ADC_WIDTH_BIT_9 = 0~512
        adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // set adc scale (ADC_ATTEN_DB_0 = 0~1.5V, ADC_ATTEN_DB_11 = 0~3.9V)


        for (int i = 5; i >= 0; i--) {
            printf("Starting in %d seconds...\n", i);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        // initialize timer interrupt to sample analog signal
        initialize_timer(timer_period);
    }
    

        
    // Main loop here
    while(1){
        if(DEBUG_MODE){
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        else{

            if(buffFull){ // If the input buffer is full perform DFT
                printf("Performing DFT\n");
                timer_disable_intr(TIMER_GROUP_0, TIMER_0);
                // memcpy(out_buffer, in_buffer, sizeof (out_buffer)); // copy input buffer into temporary buffer
                sampleCount = 0;
                buffFull = false; // reset 

                // TODO: perform DFT
                discrete_fourier_transform();
            }

        }
    }
    
}






// void discrete_fourier_transform(){
//     int M = N;
//     int k = N;

//     double scale_factor = 1.0 / N;

//     double real_sum = 0.0;
//     double imag_sum = 0.0;
//     double dft_real[N] = {0.0};
//     double dft_imag[N] = {0.0};
//     float dft_pwr[N] = {0.0F};

//     double cosine[N] = {0};
//     double sine[N] = {0};
//     for (int n = 0; n < N; n++) { // TODO: move to init DFT function
//         cosine[n] = cos((2 * n * k * M_PI) / N);
//         sine[n] = sin((2 * n * k * M_PI) / N);
//     }

//     // get coefficients
//     // printf("The coeffs are: ");
//     // Get each coefficient, Xk
//     for(int k=0; k<N; k++){

//         real_sum = 0.0;
//         imag_sum = 0.0;

//         // get sum from 0-N of input * complex exponential
//         for (int n = 0; n < N; n++) {
//                 real_sum += in_buffer[n] * cosine[n];
//                 imag_sum += in_buffer[n] * sine[n];
//             }

//         dft_real[k] = real_sum;
//         dft_imag[k] = imag_sum;
//         float sq_mag = real_sum*real_sum + imag_sum*imag_sum;
//         float log_mag = (float)log(sq_mag * scale_factor);
//         if (log_mag < 0.0F) log_mag = 0.0F;
//         dft_pwr[k] = log_mag;

//         // printf("X[%d] = %f - %fi\n", k, dft_real, dft_imaginary);
//     }

//     printf("DFT display:\n");
//     // dft_display(dft_buffer, N/2, 64, 10,  -60, 40, '|');

//     // TODO: check which one has greatest magnitude

//     // TODO: simple print causes stack overflow (something to do with floats?)
//     printf("X[%d] = %.2f - %.2fi\n", k, (float)dft_real[k], (float)dft_imag[k]);


// }




// void discrete_fourier_transform()
// {
//     // Create an object to store the real (cosine) and imaginary (sine) coefficients
//     // my_dft_config* dft_config = new my_dft_config;
//     // dft_config->init(N);
//     // float* dft_real = dft_config->dft_real;
//     // float* dft_imag = dft_config->dft_imag;
//     float* dft_real = new float[N];
//     float* dft_imag = new float[N];

//     // copy the samples into the real buffer
//     for(int n = 0; n < N; n++){
//         dft_real[n] = in_buffer[n];
//         dft_imag[n] = 0.0F;
//     }

    
//     my_dft dft1;
//     // clear existing buffers and set up cos and sin functions
//     dft1.init(N, true); 
//     // Calculate the DFT coefficients dft_coeff[i] = sum of real and imaginary
//     for (int i = 0; i < N; i++)
//         dft1.perform_transform(dft_real + i, dft_imag + i);

//     // // Get the DFT magnitudes in log scale
//     // double scale_factor = 1.0 / N;
//     // for(int n = 0; n < N; n++){
//     //     float real_val = dft_real[N];
//     //     float imag_val = dft_imag[N];

//     //     // calculate the square magnitude
//     //     float sq_mag = real_val * real_val + imag_val * imag_val; 
//     //     float log_mag = (float)log(sq_mag * scale_factor); 
//     //     if (log_mag < 0.0F) //(log_mag < 0.0F)
//     //         log_mag = 0.0F; //log_mag = 0.0F;

//     //     // write to output buffer
//     //     dft_buffer[n] = log_mag;
//     // }

//     // ets_printf("DFT");
//     // dft1.display(dft_buffer, N, 64, 10,  -60, 40, '|');

// }