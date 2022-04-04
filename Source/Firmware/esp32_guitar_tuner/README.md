# Esp32 Guitar Tuner




## Background
We use the Discrete Fourier Transform (DFT) to identify the fundamental frequency of a plucked note on the guitar and display how far the user is from the correct note. 

An electric guitar typically covers the range of 80Hz to 1200Hz. Assuming the highest frequency we will observe is 1200Hz, we select a sampling rate of 2500Hz in order to satisfy the Nyquist Criteria. 


### Discrete Fourier Transform
Steps:
1. Collect a signal of size N and apply window function (Hann window)
2. Define a DFT buffer of size N
3. Create an object to store the real (cosine) and imaginary (sine) coefficients
4. Define cosine and sine functions for size N (more efficient than calling sin() and cos() functions constantly)
5. Calculate the DFT coefficients dft_coeff[i] = sum of real and imaginary
    for k = 0:N
        for n = 0:N
            sum_real += signal[n] * cosine[n];
            sum_imag += signal[n] * sine[n];
        X[k] = <sum_real> + i<sum_imag>
    
6. Calculate the power of each coefficient
    pwr = sqroot(a^2 + b^2)

7. Find the index of the maximum power and convert to frequency
    freq = i_max * Fs / N   (to convert from DFT sample to frequency)



