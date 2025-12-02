#pragma once
#include "mbed.h"
#include "LSM6DSL/lsm6dsl_class.h"

class SensorManager {
public:
    SensorManager(I2C &i2c);

    void init();
    void readAccel(float &ax, float &ay, float &az);
    void readGyro(float &gx, float &gy, float &gz);

    float computeMotionIntensity(float ax, float ay, float az,
                                 float gx, float gy, float gz);

private:
    LSM6DSL accelgyro;
};
