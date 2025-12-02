#include "TimeBuffer.h"
#include <cstring>
#include <cmath>

TimeBuffer::TimeBuffer() : index(0) {
    memset(data, 0, sizeof(data));
}

bool TimeBuffer::addSample(float value) {
    if (index >= RAW_SAMPLES) return false;
    data[index++] = value;
    return true;
}

bool TimeBuffer::isFull() const {
    return index >= RAW_SAMPLES;
}

void TimeBuffer::reset() {
    index = 0;
}

int TimeBuffer::getCount() const {
    return index;
}

void TimeBuffer::getFFTInput(float *out) const {
    for (int i = 0; i < RAW_SAMPLES; i++)
        out[i] = data[i];
    
    for (int i = RAW_SAMPLES; i < FFT_SIZE; i++)
        out[i] = 0.0f;
}

float TimeBuffer::getFrequencyResolution() const {
    return (float)SAMPLE_RATE / (float)FFT_SIZE;
}
