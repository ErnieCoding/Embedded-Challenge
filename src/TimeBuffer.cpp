#include "TimeBuffer.h"

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

void TimeBuffer::getFFTInput(float *out) {
    // copy collected samples
    for (int i = 0; i < RAW_SAMPLES; i++)
        out[i] = data[i];

    // zero pad remaining FFT slots
    for (int i = RAW_SAMPLES; i < FFT_SIZE; i++)
        out[i] = 0.0f;
}
