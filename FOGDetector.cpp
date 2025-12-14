#include "FOGDetector.h"

FOGDetector::FOGDetector()
    : prevWalking(false)
{}

// 判断是否为 walking 状态
bool FOGDetector::isWalking(float hz, float mag) const {
    return (hz >= WALK_F_LO &&
            hz <= WALK_F_HI &&
            mag >= WALK_MAG_THRESHOLD);
}

// 返回 0 或 1
uint8_t FOGDetector::detect(float hz, float mag)
{
    bool walkingNow = isWalking(hz, mag);
    bool freezeNow  = (mag < FREEZE_MAG_THRESHOLD);

    uint8_t fog = 0;

    // 条件：之前在走，现在突然 freeze
    if (prevWalking && freezeNow && !walkingNow) {
        fog = 1;
    }

    prevWalking = walkingNow;
    return fog;
}
