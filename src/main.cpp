#include "mbed.h"
#include "SensorManager.h"

I2C i2c(PB_11, PB_10);
DigitalOut led(LED2);

SensorManager sensors(i2c);

int main() {
    printf("\n=== Motion Sensor Demo ===\n");
    printf("Project: Tremor Detection System\n");
    printf("Sampling Rate: 52Hz\n\n");
    
    // Set I2C frequency to 400kHz
    i2c.frequency(400000);
    
    // Initialize sensor with error checking
    if (!sensors.init()) {
        printf("FATAL: Sensor initialization failed!\n");
        while(1) {
            led = !led;
            thread_sleep_for(100);
        }
    }
    
    Timer blinkTimer;
    blinkTimer.start();
    
    float blinkInterval = 0.5f;
    uint32_t sampleCount = 0;
    
    printf("Starting data acquisition...\n\n");
    
    while (true) {
        float ax, ay, az, gx, gy, gz;
        sensors.readAccel(ax, ay, az);
        sensors.readGyro(gx, gy, gz);
        
        float intensity = sensors.computeMotionIntensity(ax, ay, az, gx, gy, gz);
        
        float clamped = intensity;
        if (clamped < 0.2f) clamped = 0.2f;
        if (clamped > 5.0f) clamped = 5.0f;
        
        blinkInterval = 0.5f / clamped;
        
        if (blinkTimer.elapsed_time().count() / 1e6f > blinkInterval) {
            led = !led;
            blinkTimer.reset();
        }
        
        if (sampleCount % 10 == 0) {
            printf("ACC[g]: %.3f %.3f %.3f | GYRO[dps]: %.2f %.2f %.2f | I: %.3f\n",
                   ax, ay, az, gx, gy, gz, intensity);
        }
        
        sampleCount++;
        thread_sleep_for(19);
    }
}
