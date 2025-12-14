#include "FOGDetector.h"

FOGDetector::FOGDetector()
    : prevWalking(false)
{}

// Detect if is walking
bool FOGDetector::isWalking(float hz, float mag) const {
    return (hz >= WALK_F_LO &&
            hz <= WALK_F_HI &&
            mag >= WALK_MAG_THRESHOLD);
}

// Return 0 or 1
uint8_t FOGDetector::detect(float hz, float mag)
{
    bool walkingNow = isWalking(hz, mag);
    bool freezeNow  = (mag < FREEZE_MAG_THRESHOLD);

    uint8_t fog = 0;

    // Condition: walking and sudden freeze
    if (prevWalking && freezeNow && !walkingNow) {
        fog = 1;
    }

    prevWalking = walkingNow;
    return fog;
}

