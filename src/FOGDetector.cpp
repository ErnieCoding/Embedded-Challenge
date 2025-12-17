#include "FOGDetector.h"
#include <cstdio>

FOGDetector::FOGDetector()
    : state(State::IDLE), freezeCount(0) {}

uint8_t FOGDetector::detect(float accHz,
                            float accMag,
                            float gyroHz,
                            float gyroMag)
{
    // 1. Definition: Walking 
    // (Significant Accel Magnitude + Low Frequency)
    bool walkingNow =
        (accHz > 0.6f && accHz < WALK_HZ_MAX) &&
        (accMag > ACC_MAG_MIN_FOR_WALK);

    // 2. Definition: Freeze Candidate
    // (Low Accel Mag + High Gyro Mag)
    bool freezeLike =
        (accMag < ACC_MAG_MAX_FOR_FOG) &&
        (gyroMag > GYRO_MAG_MIN_FOR_FOG);

    // State Machine
    switch (state) {
    case State::IDLE:
        // Must detect walking first
        if (walkingNow) {
            state = State::WALKING;
            freezeCount = 0;
            printf("[FOG] Walking Started\n");
        }
        break;

    case State::WALKING:
        if (freezeLike) {
            freezeCount++;
            if (freezeCount >= FREEZE_CONSECUTIVE_NEEDED) {
                state = State::FREEZE;
                freezeCount = 0;
                printf("[FOG] !!! FREEZE DETECTED !!!\n");
            }
        } else {
            // Reset counter if not looking like a freeze
            if (walkingNow) {
                freezeCount = 0;
            }
            // If stopped moving but not freeze-like (e.g. sitting down), go Idle
            else if (accMag < 0.5f && gyroMag < 0.5f) {
                state = State::IDLE;
                freezeCount = 0;
            }
        }
        break;

    case State::FREEZE:
        // Stay frozen until walking is detected again
        if (walkingNow) {
            state = State::WALKING;
            freezeCount = 0;
            printf("[FOG] Unfrozen -> Walking\n");
        }
        break;
    }

    // CONTINUOUS BROADCAST: Return 1 as long as we are in FREEZE state
    return (state == State::FREEZE) ? 1 : 0;
}