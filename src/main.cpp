#include "mbed.h"
#include "SensorManager.h"
#include "FFTBuffer.h"

I2C i2c(PB_11, PB_10);
DigitalOut led(LED2);

// Serial console setup
static BufferedSerial serial_port(USBTX, USBRX, 115200);
FileHandle *mbed::mbed_override_console(int fd) {
    return &serial_port;
}

SensorManager sensors(i2c);
FFTBuffer fft_buffer;

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
        
        // LED INTENSITY BLINKING - CAN USE FOR TEST
        // if (intensity < 0.5f) { // threshold for no motion
        //     led = 0;
        //     blinkTimer.reset();
        // } else {
        //     float clamped = intensity;
        //     if (clamped > 5.0f) clamped = 5.0f; // max blink rate
        //     blinkInterval = 0.5f / clamped;

        //     if (blinkTimer.elapsed_time().count() / 1e6f > blinkInterval) {
        //         led = !led;
        //         blinkTimer.reset();
        //     }
        // }
        
        float acc_z_centered = az - 1.0f;

        for (int i = 0; i < FFTBuffer::RAW_SAMPLES; i++){
            fft_buffer.addSample(ax);
            fft_buffer.addSample(ay);
            fft_buffer.addSample(az);
        }

        if (fft_buffer.isFull()){
            // Compute frequencies & Magnitude
            fft_buffer.computeFFT(fft_buffer.S, fft_buffer.fft_input, fft_buffer.fft_output, 0);
            fft_buffer.computeMagtd(fft_buffer.fft_output, fft_buffer.fft_mag, fft_buffer.FFT_SIZE/2);

            // Find dominant frequency
            fft_buffer.findDominantFreq(fft_buffer.fft_mag);

            fft_buffer.reset();
        }

        printf("\n\n\n===============================\n");
        printf(">Raw_Acc: %.2f  %.2f  %.2f\n", ax, ay, az);
        printf(">Raw_Gyro: %.2f  %.2f  %.2f\n\n", gx, gy, gz);

        printf(">Freq_Hz: %.2f\n", fft_buffer.current_dominant_freq);

        
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
