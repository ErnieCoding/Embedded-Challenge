#include "mbed.h"
#include "SensorManager.h"

I2C i2c(PB_11, PB_10);
DigitalOut led(LED2);

// Serial console setup
static BufferedSerial serial_port(USBTX, USBRX, 115200);
FileHandle *mbed::mbed_override_console(int fd) {
    return &serial_port;
}

SensorManager sensors(i2c);

int main() {
    ThisThread::sleep_for(5000ms); // 5 sec delay to open serial monitor

    printf("\n\n\n");
    printf("=== Motion Sensor Demo ===\n");
    printf("Project: Tremor Detection System\n");
    printf("Sampling Rate: 52Hz\n\n");
    fflush(stdout);
    
    // Set I2C to 400kHz
    i2c.frequency(400000);
    
    if (!sensors.init()) {
        printf("FATAL: Sensor initialization failed!\n");
        fflush(stdout);
        while(1) {
            led = !led;
            thread_sleep_for(100);
        }
    }
    
    // Confirm sensor initialized
    printf("Sensor initialized successfully!\n");
    printf("Starting data acquisition...\n\n");
    fflush(stdout);
    
    Timer blinkTimer;
    blinkTimer.start();
    
    float blinkInterval = 0.5f;
    uint32_t sampleCount = 0;
    
    while (true) {
        float ax, ay, az, gx, gy, gz;
        sensors.readAccel(ax, ay, az);
        sensors.readGyro(gx, gy, gz);
        
        float intensity = sensors.computeMotionIntensity(ax, ay, az, gx, gy, gz);
        
        if (intensity < 0.5f) { // threshold for no motion
            led = 0;
            blinkTimer.reset();
        } else {
            float clamped = intensity;
            if (clamped > 5.0f) clamped = 5.0f; // max blink rate
            blinkInterval = 0.5f / clamped;

            if (blinkTimer.elapsed_time().count() / 1e6f > blinkInterval) {
                led = !led;
                blinkTimer.reset();
            }
        }
        
        if (sampleCount % 10 == 0) {
            printf("ACC[g]: %.3f %.3f %.3f | GYRO[dps]: %.3f %.3f %.3f | I: %.3f\n", ax, ay, az, gx, gy, gz, intensity);

            if (intensity < 0.5f) {
                printf("\n[STILL]\n");
            } else {
                printf("\n[MOTION!]\n");
            }

            fflush(stdout);
        }
        
        sampleCount++;
        thread_sleep_for(19);
    }
}



/* LED TEST */
// DigitalOut led(LED2);

// int main() {
//     // Blink pattern: 3 fast, pause, repeat
//     while(1) {
//         for(int i = 0; i < 5; i++) {
//             led = 1;
//             ThisThread::sleep_for(100ms);
//             led = 0;
//             ThisThread::sleep_for(100ms);
//         }
//         ThisThread::sleep_for(1000ms);
        
//         printf("Hello World!\n");
//     }
// }


/*LED TEST 2*/
// DigitalOut led(LED2);
// BufferedSerial pc(USBTX, USBRX, 115200);

// int main() {
//     // Wait for serial to be ready
//     ThisThread::sleep_for(2000ms);
    
//     char msg[] = "\n\nHello from STM32!\n";
//     pc.write(msg, sizeof(msg));
    
//     while(1) {
//         led = !led;
//         ThisThread::sleep_for(500ms);
        
//         char ping[] = "Ping!\n";
//         pc.write(ping, sizeof(ping));
//     }
// }
