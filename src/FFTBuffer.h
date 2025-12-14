#pragma once
#include <stdint.h>
#include <cstring>
#include <arm_math.h>

class FFTBuffer {
public:
    static constexpr int SAMPLE_RATE = 52; // Sensor ODR
    static constexpr int DURATION_SEC = 3;
    static constexpr int RAW_SAMPLES = SAMPLE_RATE * DURATION_SEC;  // 156 samples in total in a 3 second interval
    static constexpr int FFT_SIZE = 256;
    static const float FREQ_BIN_SZ = (float)SAMPLE_RATE / (float)FFT_SIZE;
    
    volatile float current_dominant_freq = 0.0f;
    
    // FFT Buffers
    float fft_input[FFTBuffer::FFT_SIZE];
    float fft_output[FFTBuffer::FFT_SIZE];
    float fft_mag[FFTBuffer::FFT_SIZE / 2];

    arm_rfft_fast_instance_f32 S;

    FFTBuffer();
    
    bool addSample(float value);
    bool isFull() const;
    void reset();
    int getCount() const;

    void computeFFT(arm_rfft_fast_instance_f32 S, float fft_input[], float fft_output[], int flag);
    void computeMagtd(float fft_output[], float fft_mag[], int numSamples);
    void findDominantFreq(float fft_mag[]);

private:
    int index;
};
