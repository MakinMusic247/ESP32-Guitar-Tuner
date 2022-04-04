#define _USE_MATH_DEFINES  // Makes `M_PI' available as a constant.

#include <stdlib.h>
#include <math.h>
#include "fft.h"


// takes a complex array x (row or column)
// We have a row (or column x) --> want to split it into odd and even sub arrays --> recursively fft those
/*
    Recursive FFT function. Splits incoming complex array x into even and odd sub arrays
    Parameters:
        x --> input complex array
        N --> size of input array
        lev --> fft level (how many have taken place)
*/

void my_fft::perform_transform(Complex* x, int r, int r_lev, int stride)
{
    //if(r_lev == 1) printf("N=%d, lev=%d\n", r, r_lev);

    if (r <= 1) return; // ensure not last recursion
    int M = r / 2.0F; // half size

    // First copy the input values to the complex temporary buffers.
    int n, k;
    Complex* cp;
    // First copy the input values to the real and imaginary temporary buffers.
    for (cp = x, n = 0; n < r; n++, cp += stride)
    {
        complex_buf[n] = *cp; //real_buf[n] = *rp;  imag_buf[n] = *ip;
    }

    // divide input array into odd and even sub arrays
    // this will hold the values from the input array
    Complex* even = new Complex[M];
    Complex* odd = new Complex[M];
    for (int i = 0; i < M; i++) {
        even[i] = complex_buf[2 * i];
        odd[i] = complex_buf[2 * i + 1];
    }

    // perform fft on sub arrays
    this->perform_transform(even, M, r_lev, stride/2.0);
    this->perform_transform(odd, M, r_lev, stride/2.0);

    // combine back into array x
    for (cp = x, k = 0; k < M; k++, cp += stride) {
        // calculate odd constant 
        Complex t = std::polar(1.0, -2 * PI * k / r);
        t *= odd[k];

        // fill buffer 
        *(x + k * stride) = even[k] + t; // fills first half of fft buffer   //x[k] = even[k] + t; // *cp =  *(x + n * stride) = 
        *(x + k * stride + M) = even[k] - t; // fills 2nd half of fft buffer (offset by M)    //x[k + N / 2] = even[k] - t; // *(cp + M) =   
    }

    r_lev += 1;
}