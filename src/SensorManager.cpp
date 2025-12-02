#include "SensorManager.h"

SensorManager::SensorManager(I2C &i2c)
    : accelgyro(i2c, LSM6DSL_I2C_ADD_L) {}

void SensorManager::init() {
    accelgyro.begin();

    // Set accelerometer ±2g, 52 Hz ODR (matches project requirements)
    accelgyro.Set_X_FS(LSM6DSL_FS_XL_2G);
    accelgyro.Set_X_ODR(LSM6DSL_ODR_XL_52Hz);

    // Set gyroscope ±250 dps, 52 Hz ODR
    accelgyro.Set_G_FS(LSM6DSL_FS_G_250dps);
    accelgyro.Set_G_ODR(LSM6DSL_ODR_G_52Hz);
}

void SensorManager::readAccel(float &ax, float &ay, float &az) {
    int32_t data[3];
    accelgyro.Get_X_Axes(data);
    ax = data[0] / 1000.0f;
    ay = data[1] / 1000.0f;
    az = data[2] / 1000.0f;
}

void SensorManager::readGyro(float &gx, float &gy, float &gz) {
    int32_t data[3];
    accelgyro.Get_G_Axes(data);
    gx = data[0] / 1000.0f;
    gy = data[1] / 1000.0f;
    gz = data[2] / 1000.0f;
}

// Simple magnitude metric for LED blinking logic
float SensorManager::computeMotionIntensity(float ax, float ay, float az,
                                            float gx, float gy, float gz) {
    float am = sqrtf(ax*ax + ay*ay + az*az);
    float gm = sqrtf(gx*gx + gy*gy + gz*gz);
    return am + (gm * 0.1f);    // tuned weight, adjustable later
}
