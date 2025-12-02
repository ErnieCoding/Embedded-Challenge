#pragma once
#include <stdint.h>
#include <cstring>

class TimeBuffer {
public:
    static constexpr int SAMPLE_RATE = 52;
    static constexpr int DURATION_SEC = 3;
    static constexpr int RAW_SAMPLES = SAMPLE_RATE * DURATION_SEC;  // 156
    static constexpr int FFT_SIZE = 256;

    TimeBuffer();
    
    bool addSample(float value);
    bool isFull() const;
    void reset();
    int getCount() const;
    
    void getFFTInput(float *out) const;
    float getFrequencyResolution() const;

private:
    float data[RAW_SAMPLES];
    int index;
};
