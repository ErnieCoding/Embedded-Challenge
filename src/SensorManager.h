#pragma once
#include "mbed.h"

// LSM6DSL Registers
#define LSM6DSL_ADDR        (0x6A << 1)
#define WHO_AM_I            0x0F
#define CTRL1_XL            0x10
#define CTRL2_G             0x11
#define CTRL3_C             0x12
#define STATUS_REG          0x1E
#define OUTX_L_G            0x22
#define OUTX_L_XL           0x28

class SensorManager {
public:
    SensorManager(I2C &i2c);
    
    bool init();
    void readAccel(float &ax, float &ay, float &az);
    void readGyro(float &gx, float &gy, float &gz);
    void readGyroRaw(int16_t &gx, int16_t &gy, int16_t &gz);
    
    float computeMotionIntensity(float ax, float ay, float az,
                                 float gx, float gy, float gz);

private:
    I2C &_i2c;
    
    // Helper functions for register access
    bool writeReg(uint8_t reg, uint8_t val);
    bool readReg(uint8_t reg, uint8_t &val);
    bool readInt16(uint8_t reg_low, int16_t &val);
    
    // Sensitivity factors for ±2g and ±250dps
    static constexpr float ACCEL_SENSITIVITY = 0.061f;  // mg/LSB
    static constexpr float GYRO_SENSITIVITY = 8.75f;    // mdps/LSB
};
