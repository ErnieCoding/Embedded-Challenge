#include "mbed.h"
#include "SensorManager.h"
#include "FFTBuffer.h"

I2C i2c(PB_11, PB_10);
DigitalOut led(LED2);

// Serial console setup
BufferedSerial serial_port(USBTX, USBRX, 115200);
FileHandle *mbed::mbed_override_console(int fd) {
    return &serial_port;
}

SensorManager sensors(i2c);
FFTBuffer acc_buffer;
FFTBuffer gyro_buffer;

int main() {
    ThisThread::sleep_for(5000ms); // 5 sec delay to open serial monitor
    
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

        // printf("\n\n\n===============================\n\n");
        // printf(">Raw_Acc: [%.2f]  [%.2f]  [%.2f]\n", ax, ay, az);
        // printf(">Raw_Gyro: [%.2f]  [%.2f]  [%.2f]\n\n", gx, gy, gz);
        // printf("---");

        float acc_mag  = sqrtf(ax*ax + ay*ay + az*az);
        float gyro_mag = sqrtf(gx*gx + gy*gy + gz*gz);

        acc_buffer.addSample(acc_mag);
        gyro_buffer.addSample(gyro_mag);

        if (acc_buffer.isFull() && gyro_buffer.isFull()){
            // Processing accelerometer data
            acc_buffer.process();
            // Processing gyroscope data
            gyro_buffer.process();

            // printf(">ACC_DOM_Freq_MAG: [%.2f]\n", acc_buffer.dominantMag);
            // printf(">ACC_DOM_Freq_Hz: [%.2f]\n\n", acc_buffer.dominantHz);

            // printf(">GYRO_DOM_Freq_MAG: [%.2f]\n", gyro_buffer.dominantMag);
            // printf(">GYRO_DOM_Freq_Hz: [%.2f]\n\n", gyro_buffer.dominantHz);

            acc_buffer.reset();
            gyro_buffer.reset();
        }

        // printf("===============================\n\n\n");



        
        sampleCount++;

        // 52 Hz = approx 19ms period
        ThisThread::sleep_for(19ms);
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
