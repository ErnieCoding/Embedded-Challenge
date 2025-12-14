#include "FFTBuffer.h"
#include <cmath>


FFTBuffer::FFTBuffer() {
    arm_rfft_fast_init_f32(&rfft, FFT_SIZE);
    reset();
}

bool FFTBuffer::addSample(float value) {
    if (index >= RAW_SAMPLES) return false;
    fftInput[index++] = value;
    return true;
}

bool FFTBuffer::isFull() const {
    return index >= RAW_SAMPLES;
}

void FFTBuffer::reset() {
    index = 0;
    memset(fftInput, 0, sizeof(fftInput));
    memset(fftOutput, 0, sizeof(fftOutput));
    memset(mag, 0, sizeof(mag));

    dominantHz = 0.0f;
    dominantHz = 0.0f;
}

void FFTBuffer::process() {
    for (int i = RAW_SAMPLES; i < FFT_SIZE; i++) fftInput[i] = 0.0f; // zero-pad the rest of the samples [156:]

    float mean = 0.0f;
    for (int i = 0; i < RAW_SAMPLES; i++) mean += fftInput[i];
    mean /= (float)RAW_SAMPLES;

    // Hann windowing for the first 156 samples
    constexpr float TWO_PI = 6.28318530718f;
    for (int n = 0; n < RAW_SAMPLES; n++) {
        float w = 0.5f * (1.0f - cosf(TWO_PI * n / (RAW_SAMPLES - 1)));
        fftInput[n] = (fftInput[n] - mean) * w;
    }

    arm_rfft_fast_f32(&rfft, fftInput, fftOutput, 0);

    mag[0] = fabsf(fftOutput[0]);
    arm_cmplx_mag_f32(&fftOutput[2], &mag[1], (FFT_SIZE/2 - 1));

    computeDominantInRange(0.5f, 12.0f);
}

void FFTBuffer::computeDominantInRange(float fLow, float fHigh) {
    int b0 = hzToBin(fLow);
    int b1 = hzToBin(fHigh);
    if (b0 < 1) b0 = 1;
    if (b1 > (FFT_SIZE/2 - 1)) b1 = (FFT_SIZE/2 - 1);

    float best = 0.0f;
    int bestBin = 0;

    for (int b = b0; b <= b1; b++) {
        float v = mag[b];
        if (v > best) { best = v; bestBin = b; }
    }

    dominantMag = best;
    dominantHz  = bestBin * BIN_HZ;
}