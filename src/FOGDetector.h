#ifndef FOG_DETECTOR_H
#define FOG_DETECTOR_H

#include <cstdint>

class FOGDetector {
public:
    FOGDetector();

    // 输入：当前窗口的 4 个 FFT 特征
    // 输出：1 = FOG（边沿触发，触发一次），0 = 其它
    uint8_t detect(float accHz,
                   float accMag,
                   float gyroHz,
                   float gyroMag);

private:
    enum class State { IDLE, WALKING, FREEZE };
    State state;

    int freezeCount;

    // ===== 这些阈值你说“对了”，我不动它们，只修逻辑 =====
    static constexpr float WALK_HZ_MAX = 3.5f;     // 走路主频上限
    static constexpr float NON_WALK_HZ_MIN = 5.0f; // 非步态主频下限

    static constexpr float ACC_MAG_MAX_FOR_FOG = 1.0f;
    static constexpr float GYRO_MAG_MIN_FOR_FOG = 0.6f;

    // 走路时 accMag 必须“明显大于你静止的 0.02~0.05”
    // 这个阈值用来保证“必须先走过”，否则不会进入 WALKING
    static constexpr float ACC_MAG_MIN_FOR_WALK = 5.0f;

    // 防抖：连续多少个窗口像 freeze 才确认
    static constexpr int FREEZE_CONSECUTIVE_NEEDED = 3;
};

#endif





