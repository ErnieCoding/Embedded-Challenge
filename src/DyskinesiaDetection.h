#pragma once
#include <stdint.h>
#include "FFTBuffer.h"

class DyskinesiaDetector {
public:
    int detectionCount; 
    
    DyskinesiaDetector();

    // Returns 1 if 80% (16/20) of the last minute contained Dyskinesia
    uint8_t detect(const FFTBuffer &fft);
    
    // Returns 1 if the CURRENT 3-second window looks like Dyskinesia
    uint8_t detectRaw(const FFTBuffer &fft) const;

private:
    static constexpr float DYSK_F_LO = 5.0f; 
    static constexpr float DYSK_F_HI = 7.0f; 
    
    static constexpr float REF_F_LO = 0.5f;
    static constexpr float REF_F_HI = 15.0f;

    // Thresholds
    static constexpr float TOTAL_POWER_MIN = 100.0f; 
    static constexpr float RATIO_THRESHOLD = 0.25f; 
    static constexpr float MIN_DOM_MAG = 10.0f;   

    // Rolling Window Settings
    static constexpr int HISTORY_SIZE = 20;   
    static constexpr int REQUIRED_COUNT = 16;

    float bandPower(const FFTBuffer &fft, float fLow, float fHigh) const;
    
    // Circular Buffer
    uint8_t history[HISTORY_SIZE];
    int historyIndex;
};