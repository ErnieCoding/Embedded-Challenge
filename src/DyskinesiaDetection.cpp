#include "DyskinesiaDetection.h"
#include <cmath>
#include <cstring>
#include <cstdio> // For printf

DyskinesiaDetector::DyskinesiaDetector(){
    detectionCount = 0;
    historyIndex = 0;
    memset(history, 0, sizeof(history));
}

float DyskinesiaDetector::bandPower(const FFTBuffer &fft, float fLow, float fHigh) const {
    int b0 = FFTBuffer::hzToBin(fLow);
    int b1 = FFTBuffer::hzToBin(fHigh);

    if (b0 < 1) b0 = 1;
    int maxBin = (FFTBuffer::FFT_SIZE / 2 - 1);
    if (b1 > maxBin) b1 = maxBin;
    if (b1 < b0) return 0.0f;

    float sum = 0.0f;
    for (int b = b0; b <= b1; b++) {
        float v = fft.mag[b];
        sum += v * v;
    }
    return sum;
}

uint8_t DyskinesiaDetector::detectRaw(const FFTBuffer &fft) const {
    float total = bandPower(fft, REF_F_LO, REF_F_HI);
    float powerInBand = bandPower(fft, DYSK_F_LO, DYSK_F_HI);
    float ratio = powerInBand / (total + 1e-9f);

    // --- DEBUGGING OUTPUT ---
    if (fft.dominantMag > 10.0f) {
        printf("[DYSK CHECK] Hz: %.2f | Mag: %.2f | Ratio: %.2f ", fft.dominantHz, fft.dominantMag, ratio);
        
        if (fft.dominantHz < DYSK_F_LO) printf("-> FAIL: Too Slow (<%.1f)\n", DYSK_F_LO);
        else if (fft.dominantHz > DYSK_F_HI) printf("-> FAIL: Too Fast (>%.1f)\n", DYSK_F_HI);
        else if (total < TOTAL_POWER_MIN) printf("-> FAIL: Too Weak (Power)\n");
        else if (ratio < RATIO_THRESHOLD) printf("-> FAIL: Ratio Low (<%.2f)\n", RATIO_THRESHOLD);
        else printf("-> PASS!\n");
    }

    if (total < TOTAL_POWER_MIN) return 0;
    if (fft.dominantMag < MIN_DOM_MAG) return 0;

    bool freqMatch = (fft.dominantHz >= DYSK_F_LO && fft.dominantHz <= DYSK_F_HI);
    if (!freqMatch) return 0;

    if (ratio < RATIO_THRESHOLD) return 0;

    return 1;
}

uint8_t DyskinesiaDetector::detect(const FFTBuffer &fft) {
    // Get result for this 3-second window
    uint8_t raw = detectRaw(fft);
    
    // Add to Circular Buffer
    history[historyIndex] = raw;
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;

    // Count total positives in the buffer
    detectionCount = 0;
    for (int i = 0; i < HISTORY_SIZE; i++) {
        detectionCount += history[i];
    }

    // Return 1 if >= 80% (16/20) are positive
    return (detectionCount >= REQUIRED_COUNT) ? 1 : 0;
}