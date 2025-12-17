#ifndef FOG_DETECTOR_H
#define FOG_DETECTOR_H
#include <cstdint>

class FOGDetector {
public:
    FOGDetector();
    uint8_t detect(float accHz, float accMag, float gyroHz, float gyroMag);

private:
    enum class State { IDLE, WALKING, FREEZE };
    State state;
    int freezeCount;

    static constexpr float WALK_HZ_MAX = 3.5f;     
    static constexpr float NON_WALK_HZ_MIN = 5.0f; 
    static constexpr float ACC_MAG_MAX_FOR_FOG = 1.0f;
    static constexpr float GYRO_MAG_MIN_FOR_FOG = 0.6f;
    static constexpr float ACC_MAG_MIN_FOR_WALK = 5.0f;
    static constexpr int FREEZE_CONSECUTIVE_NEEDED = 3;
};
#endif