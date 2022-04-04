#ifndef DFT_H_
#define DFT_H_
 
#include <cstddef>
 
class my_dft {
public: // Publically available member functions of the class.
    my_dft()
    {
        N = 0; real_buf = imag_buf = NULL; real_trig = imag_trig = NULL; 
        dft_real = dft_imag = NULL; // Added
    }

    ~my_dft() { cleanup(); }

    void init(int N, bool is_forward);
    
    void perform_transform(float* real, float* imag, int stride);
    
private: // Internal helper functions
    void cleanup()
    {
        if (real_buf != NULL) delete[] real_buf;
        if (imag_buf != NULL) delete[] imag_buf;
        if (real_trig != NULL) delete[] real_trig;
        if (imag_trig != NULL) delete[] imag_trig;
        real_buf = imag_buf = NULL;  real_trig = imag_trig = NULL;
        N = 0;
    }
public: //private: // Data members
    int N;
    float* real_buf; // Working buffer with `N' entries
    float* imag_buf; // Working buffer with `N' entries
    double* real_trig; // Holds cos(2*pi*n/N) for n=0,...,N-1
    double* imag_trig; // For inverse transforms, this array holds sin(2*pi*n/N)
                       // For forward transforms, it holds sin(-2*pi*n/N).

    // Added: store DFT real and complex pixel values (of size height * width)
    float* dft_real;
    float* dft_imag;
};
 
#endif