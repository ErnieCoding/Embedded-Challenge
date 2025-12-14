#include "FFTBuffer.h"
#include <cmath>


FFTBuffer::FFTBuffer() : index(0) {
    arm_rfft_fast_init_f32(&S, FFT_SIZE);
}

bool FFTBuffer::addSample(float value) {
    if (index >= FFT_SIZE) return false;
    fft_input[index++] = value;
    return true;
}

bool FFTBuffer::isFull() const {
    return index >= FFT_SIZE;
}

void FFTBuffer::reset() {
    index = 0;
}

int FFTBuffer::getCount() const {
    return index;
}

void FFTBuffer::computeFFT(arm_rfft_fast_instance_f32 S, float fft_input[], float fft_output[], int flag = 0){
    arm_rfft_fast_f32(&S, fft_input, fft_output, flag);
}

void FFTBuffer::computeMagtd(float fft_output[], float fft_mag[], int numSamples = FFT_SIZE / 2){
    arm_cmplx_mag_f32(fft_output, fft_mag, numSamples);
}

void FFTBuffer::findDominantFreq(float fft_mag[]){
    float max_val = 0.0f;
    int max_bin = 0;

    for (int i = 1; i < FFTBuffer::FFT_SIZE / 2; i++){
        if (fft_mag[i] > max_val){
            max_val = fft_mag[i];
            max_bin = i;
        }
    }

    if (max_val > 1.0f){
        current_dominant_freq = max_bin * FFTBuffer::FREQ_BIN_SZ;
    } else {
        current_dominant_freq = 0.0f;
    }

}
