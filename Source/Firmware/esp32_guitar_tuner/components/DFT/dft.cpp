#define _USE_MATH_DEFINES  // Makes `M_PI' available as a constant.

#include <stdlib.h>
#include <math.h>
#include "dft.h"

#include <cstdio>
#include <math.h>
#include "esp_log.h"
#include <limits>



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



void my_dft::perform_transform(float* real, float* imag)
{

    // First copy the input values to the real and imaginary temporary buffers.
    int n, k;
    float* _real, * _imag;

    for (_real = real, _imag = imag, n = 0; n < N; n++, _real++, _imag++){
        real_buf[n] = *_real;  imag_buf[n] = *_imag;
    }

    // Now compute each output coefficient in turn
    for (_real = real, _imag = imag, k = 0; k < N; k++, _real++, _imag++){
        int index = 0; // This holds n*k mod N; it indexes the trig tables
        double real_sum = 0.0, imag_sum = 0.0;

        

        // TODO: error somewhere around here
        // for (int n = 0; n < N; n++, index += k)
        // {
            // if (index >= N)
            //     index -= N;
            // real_sum += real_buf[n] * real_trig[index] - imag_buf[n] * imag_trig[index];
            // imag_sum += real_buf[n] * imag_trig[index] + imag_buf[n] * real_trig[index];
        // }
        // *_real = (float)real_sum;
        // *_imag = (float)imag_sum;
    }


}




void my_dft::display(const float *data, int32_t len, int width, int height, float min, float max, char view_char)
{
    uint8_t *view_data = new uint8_t[width * height];
    float *view_data_min = new float[width];
    float *view_data_max = new float[width];
    //

    for (int y = 0; y < height ; y++) {
        for (int x = 0 ; x < width ; x++) {
            view_data[y * width + x] = ' ';
        }
    }
    for (int i = 0 ; i < width ; i++) {
        view_data_min[i] = max;
        view_data_max[i] = min;
    }
    float x_step = (float)(width) / (float)len;
    float y_step = (float)(height - 1) / (max - min);
    float data_min = std::numeric_limits<float>::max();
    float data_max = std::numeric_limits<float>::min();
    int min_pos = 0;
    int max_pos = 0;

    for (int i = 0 ; i < len ; i++) {
        int x_pos = i * x_step;
        if (data[i] < view_data_min[x_pos]) {
            view_data_min[x_pos] = data[i];
        }
        if (data[i] > view_data_max[x_pos]) {
            view_data_max[x_pos] = data[i];
        }

        if (view_data_min[x_pos] < min) {
            view_data_min[x_pos] = min;
        }
        if (view_data_max[x_pos] > max) {
            view_data_max[x_pos] = max;
        }
        ESP_LOGD("view", "for i=%i, x_pos=%i,  max=%f, min=%f, data=%f", i, x_pos, view_data_min[x_pos], view_data_max[x_pos], data[i]);
        if (data[i] > data_max) {
            data_max = data[i];
            max_pos = i;
        }
        if (data[i] < data_min) {
            data_min = data[i];
            min_pos = i;
        }
    }
    ESP_LOGI("view", "Data min[%i] = %f, Data max[%i] = %f", min_pos, data_min, max_pos, data_max);
    ESP_LOGD("view", "y_step = %f", y_step);
    for (int x = 0 ; x < width ; x++) {
        int y_count = (view_data_max[x] - view_data_min[x]) * y_step + 1;
        ESP_LOGD("view", "For x= %i y_count=%i  ,min =%f, max=%f, ... ", x, y_count, view_data_min[x], view_data_max[x]);
        for (int y = 0 ; y < y_count ; y++) {
            int y_pos = (max - view_data_max[x]) * y_step + y;
            ESP_LOGD("view", " %i, ", y_pos);
            view_data[y_pos * width + x] = view_char;
        }
        ESP_LOGD("view", " ");
    }

    printf(" ");
    for (int x = 0 ; x < width ; x++) {
        printf("_");
    }
    printf("\n");
    for (int y = 0; y < height ; y++) {
        printf("%i", y % 10);
        for (int x = 0 ; x < width ; x++) {
            printf("%c", view_data[y * width + x]);
        }
        printf("|\n");
    }
    printf(" ");
    for (int x = 0 ; x < width ; x++) {
        printf("%i", x % 10);
    }
    printf("\n");
    ESP_LOGI("view", "Plot: Length=%i, min=%f, max=%f", len, min, max);
    delete view_data;
    delete view_data_min;
    delete view_data_max;
}

