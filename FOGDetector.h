#pragma once
#include <stdint.h>

class FOGDetector {
public:
    FOGDetector();

    // 输入：3 秒 FFT 得到的 dominant frequency & magnitude
    // 输出：0 = no fog, 1 = fog
    uint8_t detect(float dominantHz, float dominantMag);

private:
    bool prevWalking;

    // 内部判定函数
    bool isWalking(float hz, float mag) const;

    // 阈值参数（可调）
    static constexpr float WALK_F_LO = 1.0f;        // 步频下限
    static constexpr float WALK_F_HI = 2.2f;        // 步频上限
    static constexpr float WALK_MAG_THRESHOLD = 0.8f;
    static constexpr float FREEZE_MAG_THRESHOLD = 0.3f;
};
