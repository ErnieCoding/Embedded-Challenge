#pragma once
#include <stdint.h>
#include "FFTBuffer.h"

class DyskinesiaDetector {
public:
    DyskinesiaDetector();

    // stable decision (debounced)
    uint8_t detect(const FFTBuffer &fft);
    uint8_t detectRaw(const FFTBuffer &fft) const;

private:
    int streak;

    // Frequency bands (Hz)
    static constexpr float MID_F_LO = 4.0f;  // dyskinesia often shows energy above tremor band
    static constexpr float MID_F_HI = 12.0f;
    // Increase reference low cutoff to ignore gravity and very-low-frequency drift
    static constexpr float REF_F_LO = 1.0f;
    static constexpr float REF_F_HI = 20.0f;

    // Heuristic thresholds (tunable)
    // minimum motion energy to consider (ignore tiny noise/g sensor offset)
    static constexpr float TOTAL_POWER_MIN = 1e-2f;
    static constexpr float RATIO_THRESHOLD = 0.12f; // mid-band power ratio
    static constexpr float ENTROPY_THRESHOLD = 0.60f; // spectral entropy (0..1)
    static constexpr int CONSECUTIVE_NEEDED = 3; // debounce

    float bandPower(const FFTBuffer &fft, float fLow, float fHigh) const;
    float spectralEntropy(const FFTBuffer &fft, float fLow, float fHigh) const;
};
