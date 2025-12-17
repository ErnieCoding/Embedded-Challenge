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

bool FFTBuffer::addSample(float x, float y, float z) {
    if (index >= RAW_SAMPLES) return false;
    fftInputX[index] = x;
    fftInputY[index] = y;
    fftInputZ[index] = z;
    index++;
    return true;
}

bool FFTBuffer::isFull() const {
    return index >= RAW_SAMPLES;
}

void FFTBuffer::reset() {
    // Reset literally everything
    index = 0;
    memset(fftInputX, 0, sizeof(fftInputX));
    memset(fftInputY, 0, sizeof(fftInputX));
    memset(fftInputZ, 0, sizeof(fftInputX));
    memset(fftOutput, 0, sizeof(fftOutput));
    memset(mag, 0, sizeof(mag));

    dominantHz = 0.0f;
    dominantMag = 0.0f;
}

void FFTBuffer::process() {
    memset(mag, 0, sizeof(mag));

    float* axisBuffers[3] = {fftInputX, fftInputY, fftInputZ};

    for (int a = 0; a < 3; a++) {
        float* currentInput = axisBuffers[a];

        float mean = 0.0f;
        for (int i = 0; i < RAW_SAMPLES; i++) mean += currentInput[i];
        mean /= (float)RAW_SAMPLES;
        for (int i = 0; i < RAW_SAMPLES; i++) currentInput[i] -= mean;

        for (int i = 0; i < RAW_SAMPLES; i++) {
            float hanning = 0.5f * (1.0f - cosf(2.0f * 3.14159f * i / (RAW_SAMPLES - 1)));
            currentInput[i] *= hanning;
        }

        // Zero Padding the rest of the FFTBuffer
        static float tempFFTInput[FFT_SIZE];
        memcpy(tempFFTInput, currentInput, sizeof(float) * RAW_SAMPLES);
        for(int i = RAW_SAMPLES; i < FFT_SIZE; i++) tempFFTInput[i] = 0.0f;

        arm_rfft_fast_f32(&rfft, tempFFTInput, fftOutput, 0);

        // Accumulate Magnitude
        static float tempMag[FFT_SIZE / 2];
        arm_cmplx_mag_f32(fftOutput, tempMag, FFT_SIZE / 2);
        
        // Sum this axis's magnitude into the global mag buffer
        for (int i = 0; i < FFT_SIZE / 2; i++) {
            mag[i] += tempMag[i];
        }
    }

    mag[0] = 0.0f;
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

