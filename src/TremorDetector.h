#include <stdint.h>
#include "FFTBuffer.h"

class TremorDetector {
    public:
        TremorDetector();
    
        // Return 0 or 1 (stable decision with debounce)
        uint8_t detect(const FFTBuffer &fft);
        uint8_t detectRaw(const FFTBuffer &fft) const;
    private:
        int tremorStreak;
        static constexpr float TREMOR_F_LO = 3.0f;
        static constexpr float TREMOR_F_HI = 5.0f;
        static constexpr float REF_F_LO = 0.5f;
        static constexpr float REF_F_HI = 10.0f;
        static constexpr float SCORE_THRESHOLD = 0.20f;
        static constexpr int CONSECUTIVE_NEEDED = 2;
        static constexpr float REF_POWER_MIN = 1.0f;
        static constexpr float DOM_MAG_MIN   = 0.05f;
        float bandPower(const FFTBuffer &fft, float fLow, float fHigh) const;
    };
    