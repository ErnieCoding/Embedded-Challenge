#include "FFTBuffer.h"
#include <cmath>
#include "mbed.h"


FFTBuffer::FFTBuffer() {
    memset(&rfft, 0, sizeof(rfft));
    reset();
}

void FFTBuffer::init(){
    if (initalized) return;

    arm_status st = arm_rfft_fast_init_f32(&rfft, FFT_SIZE);
    MBED_ASSERT(st == ARM_MATH_SUCCESS);
    initalized = true;
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
    dominantMag = 0.0f;
}

void FFTBuffer::process() {
    float mean = 0.0f;
    for (int i = 0; i < RAW_SAMPLES; i++) mean += fftInput[i];
    mean /= (float)RAW_SAMPLES;

    for (int i = 0; i < RAW_SAMPLES; i++) {
        fftInput[i] -= mean;
    }

    for (int i = RAW_SAMPLES; i < FFT_SIZE; i++) fftInput[i] = 0.0f; // zero-pad the rest of the samples [156:]

    arm_rfft_fast_f32(&rfft, fftInput, fftOutput, 0);

    mag[0] = 0.0f;
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