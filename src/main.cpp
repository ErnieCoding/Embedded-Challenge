#include "mbed.h"
#include "SensorManager.h"

I2C i2c(PB_11, PB_10);

DigitalOut led(LED2);

SensorManager sensors(i2c);

int main() {
    printf("\n--- Motion Sensor Demo ---\n");

    sensors.init();

    Timer blinkTimer;
    blinkTimer.start();

    float blinkInterval = 0.5f;

    while (true) {
        float ax, ay, az, gx, gy, gz;
        sensors.readAccel(ax, ay, az);
        sensors.readGyro(gx, gy, gz);

        float intensity = sensors.computeMotionIntensity(ax, ay, az,
                                                         gx, gy, gz);

        // Speed up blinking based on motion intensity
        float clamped = intensity;
        if (clamped < 0.2f) clamped = 0.2f;
        if (clamped > 5.0f) clamped = 5.0f;

        blinkInterval = 0.5f / clamped;

        // LED blink
        if (blinkTimer.elapsed_time().count() / 1e6f > blinkInterval) {
            led = !led;
            blinkTimer.reset();
        }

        // Debug output
        printf("ACC: %.3f %.3f %.3f   GYRO: %.3f %.3f %.3f   Intensity: %.3f\n",
               ax, ay, az, gx, gy, gz, intensity);

        thread_sleep_for(20);   // ~50Hz loop
    }
}
