#define _USE_MATH_DEFINES  // Makes `M_PI' available as a constant.

#include <stdlib.h>
#include <math.h>
#include "dft.h"

#include <cstdio>
#include <math.h>
#include "esp_log.h"
#include <limits>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"



void my_dft::init(int N, bool is_forward)
{
    cleanup(); // Delete any pre-existing buffers.
    this->N = N;
    real_buf = new float[N];
    imag_buf = new float[N];
    real_trig = new double[N];
    imag_trig = new double[N];
    for (int n = 0; n < N; n++)
    {
        real_trig[n] = cos(n * 2.0 * M_PI / N);
        imag_trig[n] = sin(n * 2.0 * M_PI / N);
        if (is_forward)
            imag_trig[n] = -imag_trig[n];
    }
}


/* perform a 1D DFT taking in the real and imaginary arrays and writing the output back to them */
void my_dft::perform_transform(float* real, float* imag, int stride)
{

    // 1. copy the input values to the real and imaginary temporary buffers.
    int n, k;

    for(int j = 0; j<N ; j++){
        real_buf[j] = real[j];
        imag_buf[j] = imag[j];
    }

    // 2. Compute the coefficients X[k] for all k
    for(k = 0; k < N; k++){
        float real_sum = 0.0F, imag_sum = 0.0F;

        // 3. perform sum X[k] = sum()
        for(int n = 0; n < N; n++){
            real_sum += real_buf[n] * real_trig[n];
            imag_sum += imag_buf[n] * imag_trig[n];
        }
        
        // 4. set input/output pointer to output value
        real[k] = (float)real_sum;
        imag[k] = (float)imag_sum;
        // vTaskDelay(1 / portTICK_PERIOD_MS);
    }

}

