#include <cstddef>
class my_dft_config {
public: // Publically available member functions of the class.
    my_dft_config()
    {
        dft_real = dft_imag = NULL; // Added
    }
    
    ~my_dft_config() { cleanup(); }
    void init(int N) {
        this->dft_real = new float[N];
        this->dft_imag = new float[N];
    }
    
private: // Internal helper functions
    void cleanup()
    {
        if (dft_real != NULL) delete[] dft_real;
        if (dft_imag != NULL) delete[] dft_imag;
    }
public: //private: // Data members
    // Added: store DFT real and complex pixel values (of size height * width)
    float* dft_real;
    float* dft_imag;

};