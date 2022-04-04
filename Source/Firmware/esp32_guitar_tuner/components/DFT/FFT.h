#ifndef DFT_H_
#define DFT_H_

#include <cstddef>
#include <complex>
#include <cmath>

typedef std::complex<float> Complex; // define complex class
#define  PI       3.14159265358979323846

class my_fft {
public: // Publically available member functions of the class.
    my_fft()
    {
        N = 0; complex_buf = NULL;  complex_trig = NULL;
    }
    /* After construction, be sure to call `init', before using the
       `forward_transform' or `inverse_transform' functions. */
    ~my_fft() { cleanup(); }
    void init(int N, bool is_forward);
    
    //void perform_transform(float* real, float* imag, int stride);
    void perform_transform(Complex* x, int r, int r_lev, int stride);
    /*  */

private: // Internal helper functions
    void cleanup()
    {
        if (complex_buf != NULL) delete[] complex_buf;
        if (complex_trig != NULL) delete[] complex_trig;
        complex_buf = NULL;  complex_trig = NULL;
        N = 0;
    }
public: //private: // Data members
    int N;
    Complex* complex_buf;
    Complex* complex_trig;

};

#endif
