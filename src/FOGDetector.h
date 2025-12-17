#pragma once
#include <stdint.h>
#include <cstring>
#include <arm_math.h>
#include <cmath>

class FFTBuffer {
public:
    static constexpr float SAMPLE_RATE = 52.0f; // Sensor ODR
    static constexpr int RAW_SAMPLES = 156;  // 156 samples in total in a 3 second interval
    static constexpr int FFT_SIZE = 256;
    static constexpr float BIN_HZ = SAMPLE_RATE / (float)FFT_SIZE;

    FFTBuffer();

    void init();
    
    bool addSample(float x, float y, float z);
    bool isFull() const;
    void reset();
    void process();

    float mag[FFT_SIZE/2]; // Nyquist sampling
    float dominantHz;
    float dominantMag;

    static int hzToBin(float hz) { return (int)(hz / BIN_HZ + 0.5f); }

private:
    int index;
    bool initalized = false;

    // Input buffers for each of the 3 axis (x, y, z)
    float fftInputX[RAW_SAMPLES];
    float fftInputY[RAW_SAMPLES];
    float fftInputZ[RAW_SAMPLES];

    // Output buffer for the FFT readings
    float fftOutput[FFT_SIZE];

    arm_rfft_fast_instance_f32 rfft;


    void computeDominantInRange(float fLow, float fHigh);
};


