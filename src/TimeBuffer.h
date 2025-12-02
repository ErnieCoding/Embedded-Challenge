#pragma once
#include <cstdint>
#include <cstring>

class TimeBuffer {
public:
    static constexpr int RAW_SAMPLES = 156;      // 52 Hz × 3 sec
    static constexpr int FFT_SIZE    = 256;      // next power of 2

    TimeBuffer();

    bool addSample(float value);
    bool isFull() const;
    void reset();

    // Copies raw samples (zero-padded) into an FFT input buffer
    void getFFTInput(float *out);

private:
    float data[RAW_SAMPLES];
    int index;
};
