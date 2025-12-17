#include "TremorDetector.h"
#include <cmath>

TremorDetector::TremorDetector()
    : tremorStreak(0)
{}


float TremorDetector::bandPower(const FFTBuffer &fft, float fLow, float fHigh) const {
    int b0 = FFTBuffer::hzToBin(fLow);
    int b1 = FFTBuffer::hzToBin(fHigh);

    // valid bins: 1 .. (FFT_SIZE/2 - 1)
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

uint8_t TremorDetector::detectRaw(const FFTBuffer &fft) const {
    //if overall spectrum is too small, treat as "no motion" => no tremor
    float Pr = bandPower(fft, REF_F_LO, REF_F_HI);
    if (Pr < REF_POWER_MIN) return 0;
    // Gate 1: dominant peak magnitude must be above a floor
    if (fft.dominantMag < DOM_MAG_MIN) return 0;
    // 1) strongest peak must be in tremor band
    bool peakInBand = (fft.dominantHz >= TREMOR_F_LO && fft.dominantHz <= TREMOR_F_HI);
    if (!peakInBand) return 0;

    // 2) tremor ratio score
    float Pt = bandPower(fft, TREMOR_F_LO, TREMOR_F_HI);

    float score = Pt / (Pr + 1e-9f);

    return (score >= SCORE_THRESHOLD) ? 1 : 0;
}

uint8_t TremorDetector::detect(const FFTBuffer &fft) {
    uint8_t tremorRaw = detectRaw(fft);

    if (tremorRaw) tremorStreak++;
    else tremorStreak = 0;

    return (tremorStreak >= CONSECUTIVE_NEEDED) ? 1 : 0;
}