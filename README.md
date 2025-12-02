# Embedded Challenge

## Project File Structure

```bash
RTES_Embedded_Challenge/
├── src/
│ ├── main.cpp # Main application entry point
│ ├── SensorManager.h # Sensor interface header
│ ├── SensorManager.cpp # Sensor implementation
│ ├── TimeBuffer.h # Data buffering header
│ └── TimeBuffer.cpp # Data buffering implementation
├── platformio.ini # PlatformIO project configuration
```

---

## File Descriptions

### **main.cpp**
**Purpose**: Main application logic and program entry point

**Key Functions**:
- Initializes I2C communication (400kHz) for sensor access
- Configures serial console output (115200 baud) via BufferedSerial
- Reads accelerometer and gyroscope data at ~52Hz
- Computes real-time motion intensity metric
- Controls LED2 based on motion (off when still, blinks faster with increased motion)
- Outputs sensor readings to serial monitor every 10 samples (~0.19 seconds)
- Provides visual feedback: `[STILL]` or `[MOTION!]` status indicators

**Hardware Pins Used**:
- `PB_11` (I2C SDA), `PB_10` (I2C SCL) - LSM6DSL communication
- `LED2` - Motion intensity indicator
- `USBTX`, `USBRX` - Serial console output


### **SensorManager.h**
**Purpose**: Header file defining the sensor interface class

**Key Components**:
- **LSM6DSL Register Definitions**: I2C address and control register mappings
- **Public Interface**:
  - `init()` - Initialize and configure sensor
  - `readAccel()` - Read 3-axis acceleration in g units
  - `readGyro()` - Read 3-axis rotation rate in dps (degrees per second)
  - `readGyroRaw()` - Read raw gyroscope values (for FFT processing)
  - `computeMotionIntensity()` - Calculate combined motion metric
- **Sensitivity Constants**:
  - Accelerometer: 0.061 mg/LSB (±2g range)
  - Gyroscope: 8.75 mdps/LSB (±250 dps range)


### **SensorManager.cpp**
**Purpose**: Implementation of LSM6DSL sensor communication via I2C registers

**Key Features**:
- **Sensor Configuration**:
  - Accelerometer: ±2g range @ 52Hz ODR
  - Gyroscope: ±250 dps range @ 52Hz ODR
  - Block data update enabled for consistent readings
- **Data Conversion**: Converts raw 16-bit sensor values to physical units
- **Motion Intensity Algorithm**:
  - Removes gravity component from accelerometer magnitude
  - Combines acceleration and rotation with tuned weighting (0.1x gyro contribution)
- **Error Handling**: Verifies sensor presence via WHO_AM_I register check


### **TimeBuffer.h**
**Purpose**: Header for circular buffer to store sensor data for FFT analysis

**Key Parameters**:
- `SAMPLE_RATE`: 52 Hz (matches sensor ODR)
- `DURATION_SEC`: 3 seconds (window size for tremor detection)
- `RAW_SAMPLES`: 156 samples (52 Hz × 3 sec)
- `FFT_SIZE`: 256 (next power of 2 for FFT efficiency)

**Public Interface**:
- `addSample()` - Add single sensor reading to buffer
- `isFull()` - Check if 3-second window is complete
- `reset()` - Clear buffer for next window
- `getFFTInput()` - Retrieve zero-padded data for FFT processing
- `getFrequencyResolution()` - Calculate frequency bin size (~0.203 Hz/bin)


### **TimeBuffer.cpp**
**Purpose**: Implementation of time-domain data buffering

**Functionality**:
- Maintains circular buffer of 156 float values
- Zero-pads data from 156 to 256 samples for FFT compatibility
- Provides frequency resolution calculation for tremor band analysis:
  - **Tremor detection**: 3-5 Hz (bins 15-25)
  - **Dyskinesia detection**: 5-7 Hz (bins 25-34)

---

## Current Limitations & Next Steps

### Current Implementation
✅ Basic sensor reading at 52Hz  
✅ Real-time motion intensity calculation  
✅ Serial output for debugging  
✅ LED feedback for motion detection  
✅ TimeBuffer class ready for FFT integration  

### Not Yet Implemented
❌ FFT frequency analysis  
❌ Tremor detection (3-5 Hz)  
❌ Dyskinesia detection (5-7 Hz)  
❌ Freezing gait detection  
❌ BLE communication  

### Next Development Steps
1. Integrate TimeBuffer into main loop to collect 3-second windows
2. Implement FFT using ARM CMSIS-DSP library (mbed-dsp)
3. Add frequency band analysis for tremor/dyskinesia classification
4. Implement distinct LED patterns for each condition
5. Add BLE characteristic transmission for mobile app
