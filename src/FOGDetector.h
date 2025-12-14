#pragma once
#include <stdint.h>

class FOGDetector {
public:
    FOGDetector();


    uint8_t detect(float dominantHz, float dominantMag);

private:
    bool prevWalking;


    bool isWalking(float hz, float mag) const;

    static constexpr float WALK_F_LO = 1.0f;
    static constexpr float WALK_F_HI = 2.2f;  
    static constexpr float WALK_MAG_THRESHOLD = 0.8f;
    static constexpr float FREEZE_MAG_THRESHOLD = 0.3f;
};


