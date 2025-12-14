#pragma once
#include <stdint.h>
#include <cstring>
#include <arm_math.h>
#include <cmath>

class FFTBuffer {
public:
    static constexpr int SAMPLE_RATE = 52; // Sensor ODR
    static constexpr int DURATION_SEC = 3;
    static constexpr int RAW_SAMPLES = SAMPLE_RATE * DURATION_SEC;  // 156 samples in total in a 3 second interval
    static constexpr int FFT_SIZE = 256;
    static constexpr float BIN_HZ = (float)SAMPLE_RATE / (float)FFT_SIZE;

    FFTBuffer();
    
    bool addSample(float value);
    bool isFull() const;
    void reset();
    
    void process();

    float mag[FFT_SIZE/2]; // Nyquist sampling
    float dominantHz = 0.0f;
    float dominantMag = 0.0f;

    static int hzToBin(float hz) {return (int)lroundf(hz / BIN_HZ);}

private:
    int index = 0;
    float fftInput[FFT_SIZE];
    float fftOutput[FFT_SIZE];
    arm_rfft_fast_instance_f32 rfft;

    void computeDominantInRange(float fLow, float fHigh);
};
