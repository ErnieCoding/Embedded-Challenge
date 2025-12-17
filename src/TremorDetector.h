#include <stdint.h>
#include "FFTBuffer.h"

class TremorDetector {
    public:
        int tremorStreak;
        TremorDetector();
    
        // Return 0 or 1 (stable decision with debounce)
        uint8_t detect(const FFTBuffer &fft);
        uint8_t detectRaw(const FFTBuffer &fft) const;
    private:
        static constexpr float TREMOR_F_LO = 3.0f;
        static constexpr float TREMOR_F_HI = 5.0f;

        static constexpr float REF_F_LO = 0.5f;
        static constexpr float REF_F_HI = 10.0f;

        static constexpr float SCORE_THRESHOLD = 0.4f;
        static constexpr int CONSECUTIVE_NEEDED = 20;
        
        static constexpr float REF_POWER_MIN = 20.0f;
        static constexpr float DOM_MAG_MIN   = 2.0f;
        static constexpr float DOMINANCE_THRESHOLD = 0.6f;
        static constexpr float TREMOR_POWER_MIN = 30.0f; 
        float bandPower(const FFTBuffer &fft, float fLow, float fHigh) const;
    };
    