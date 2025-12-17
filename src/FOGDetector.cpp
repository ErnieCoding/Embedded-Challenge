#include "FOGDetector.h"

FOGDetector::FOGDetector()
    : state(State::IDLE), freezeCount(0) {}

uint8_t FOGDetector::detect(float accHz,
                            float accMag,
                            float gyroHz,
                            float gyroMag)
{
    // 1) 定义 WALKING：低频(走路频段) + accMag 足够大
    bool walkingNow =
        (accHz > 0.6f && accHz < WALK_HZ_MAX) &&
        (accMag > ACC_MAG_MIN_FOR_WALK);

    // 2) 定义 FREEZE：accMag 很小 + gyroMag 很大 + accHz 非步态(偏高)
    //    注意这里用 AND，不再是之前那个几乎恒真的 OR
    bool freezeLike =
        (accMag < ACC_MAG_MAX_FOR_FOG) &&
        (gyroMag > GYRO_MAG_MIN_FOR_FOG);

    uint8_t fog = 0;

    switch (state) {
    case State::IDLE:
        // 必须先进入 WALKING，后面才可能触发 FOG
        if (walkingNow) {
            state = State::WALKING;
            freezeCount = 0;
        }
        break;

    case State::WALKING:
        if (freezeLike) {
            freezeCount++;
            if (freezeCount >= FREEZE_CONSECUTIVE_NEEDED) {
                fog = 1;              // ★ 只在 WALKING->FREEZE 时触发一次
                state = State::FREEZE;
                freezeCount = 0;
            }
        } else {
            freezeCount = 0;
            // 仍在走就保持 WALKING；走没了回 IDLE
            if (!walkingNow) state = State::IDLE;
        }
        break;

    case State::FREEZE:
        // freeze 结束，重新走起来才回 WALKING
        if (walkingNow) {
            state = State::WALKING;
            freezeCount = 0;
        }
        break;
    }

    return fog;
}

