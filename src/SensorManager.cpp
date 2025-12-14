#include "SensorManager.h"

SensorManager::SensorManager(I2C &i2c) : _i2c(i2c) {}

bool SensorManager::writeReg(uint8_t reg, uint8_t val) {
    char buf[2] = {(char)reg, (char)val};
    return (_i2c.write(LSM6DSL_ADDR, buf, 2) == 0);
}

bool SensorManager::readReg(uint8_t reg, uint8_t &val) {
    char r = (char)reg;
    if (_i2c.write(LSM6DSL_ADDR, &r, 1, true) != 0) return false;
    if (_i2c.read(LSM6DSL_ADDR, &r, 1) != 0) return false;
    val = (uint8_t)r;
    return true;
}

bool SensorManager::readInt16(uint8_t reg_low, int16_t &val) {
    uint8_t lo, hi;
    if (!readReg(reg_low, lo)) return false;
    if (!readReg(reg_low + 1, hi)) return false;
    val = (int16_t)((hi << 8) | lo);
    return true;
}

bool SensorManager::init() {
    uint8_t who;
    
    // Verify sensor connection
    if (!readReg(WHO_AM_I, who) || who != 0x6A) {
        printf("ERROR: LSM6DSL sensor not found (WHO_AM_I = 0x%02X)\n", who);
        return false;
    }
    printf("LSM6DSL sensor found (WHO_AM_I = 0x6A)\n");
    
    // Configure CTRL3_C: Block data update + auto-increment
    writeReg(CTRL3_C, 0x44);
    
    // Configure accelerometer: 52 Hz, ±2g range
    // CTRL1_XL: ODR=52Hz (0011), FS=±2g (00), BW=400Hz
    writeReg(CTRL1_XL, 0x30);  // 0011 0000
    
    // Configure gyroscope: 52 Hz, ±250 dps range
    // CTRL2_G: ODR=52Hz (0011), FS=±250dps (00)
    writeReg(CTRL2_G, 0x30);   // 0011 0000
    
    printf("Sensor configured: Accel ±2g @ 52Hz, Gyro ±250dps @ 52Hz\n");
    
    // Wait for sensor to stabilize
    ThisThread::sleep_for(100ms);
    
    return true;
}

void SensorManager::readAccel(float &ax, float &ay, float &az) {
    int16_t acc[3];
    
    // Read all 3 axes
    for (int i = 0; i < 3; i++) {
        readInt16(OUTX_L_XL + i*2, acc[i]);
    }
    
    // Convert to g (raw * sensitivity / 1000)
    ax = acc[0] * ACCEL_SENSITIVITY / 1000.0f;
    ay = acc[1] * ACCEL_SENSITIVITY / 1000.0f;
    az = acc[2] * ACCEL_SENSITIVITY / 1000.0f;
}

void SensorManager::readGyro(float &gx, float &gy, float &gz) {
    int16_t gyro[3];
    
    // Read all 3 axes
    for (int i = 0; i < 3; i++) {
        readInt16(OUTX_L_G + i*2, gyro[i]);
    }
    
    // Convert to dps (raw * sensitivity / 1000)
    gx = gyro[0] * GYRO_SENSITIVITY / 1000.0f;
    gy = gyro[1] * GYRO_SENSITIVITY / 1000.0f;
    gz = gyro[2] * GYRO_SENSITIVITY / 1000.0f;
}

void SensorManager::readGyroRaw(int16_t &gx, int16_t &gy, int16_t &gz) {
    readInt16(OUTX_L_G, gx);
    readInt16(OUTX_L_G + 2, gy);
    readInt16(OUTX_L_G + 4, gz);
}

float SensorManager::computeMotionIntensity(float ax, float ay, float az,
                                            float gx, float gy, float gz) {
    float am = sqrtf(ax*ax + ay*ay + az*az);
    float am_no_gravity = fabsf(am - 1.0f);
    float gm = sqrtf(gx*gx + gy*gy + gz*gz);
    return am_no_gravity + (gm * 0.1f);
}
