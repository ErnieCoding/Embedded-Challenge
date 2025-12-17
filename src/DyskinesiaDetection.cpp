#include "DyskinesiaDetection.h"
#include <cmath>

DyskinesiaDetector::DyskinesiaDetector()
    : streak(0)
{
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


float DyskinesiaDetector::spectralEntropy(const FFTBuffer &fft, float fLow, float fHigh) const {
    int b0 = FFTBuffer::hzToBin(fLow);
    int b1 = FFTBuffer::hzToBin(fHigh);

    if (b0 < 1) b0 = 1;
    int maxBin = (FFTBuffer::FFT_SIZE / 2 - 1);
    if (b1 > maxBin) b1 = maxBin;
    if (b1 < b0) return 0.0f;

    // Build PSD over the band
    int N = (b1 - b0 + 1);
    float S = 0.0f;
    // temporary buffer small enough for stack
    float psd[128];
    for (int b = b0, i = 0; b <= b1; b++, i++) {
        float v = fft.mag[b];
        float p = v * v;
        psd[i] = p;
        S += p;
    }

    if (S <= 1e-12f) return 0.0f;

    // compute entropy (base-2) and normalize by log2(N)
    float entropy = 0.0f;
    for (int i = 0; i < N; i++) {
        float p = psd[i] / S;
        if (p > 0.0f) {
            entropy -= p * (logf(p) / logf(2.0f));
        }
    }

    float norm = logf((float)N) / logf(2.0f);
    if (norm <= 0.0f) return 0.0f;
    return entropy / norm; // 0..1
}


uint8_t DyskinesiaDetector::detectRaw(const FFTBuffer &fft) const {
    // Ignore cases where dominant motion is below REF_F_LO (likely gravity / very slow drift)
    if (fft.dominantHz < REF_F_LO) return 0;

    // Compute motion energy excluding very-low-frequency components (gravity)
    float motion = bandPower(fft, REF_F_LO, REF_F_HI);
    if (motion < TOTAL_POWER_MIN) return 0;

    float mid = bandPower(fft, MID_F_LO, MID_F_HI);
    float ratio = mid / (motion + 1e-9f);

    // Compute entropy over motion band (ignoring gravity)
    float ent = spectralEntropy(fft, REF_F_LO, REF_F_HI);

    // dyskinesia tends to be irregular/broadband -> higher entropy
    bool entropyOk = (ent >= ENTROPY_THRESHOLD);
    bool ratioOk = (ratio >= RATIO_THRESHOLD);

    return (entropyOk && ratioOk) ? 1 : 0;
}


uint8_t DyskinesiaDetector::detect(const FFTBuffer &fft) {
    uint8_t raw = detectRaw(fft);

    if (raw) streak++;
    else streak = 0;

    return (streak >= CONSECUTIVE_NEEDED) ? 1 : 0;
}
