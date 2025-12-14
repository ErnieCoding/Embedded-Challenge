# FFTBuffer Usage

## What FFTBuffer Produces

After each 3-second window is collected and processed, `FFTBuffer` provides:

- `mag[]` — magnitude spectrum for frequency bins 0..127 (0..~26 Hz for 52 Hz sampling)
- `dominantHz` — strongest frequency (peak) within a configured search range (default: 0.5–12 Hz)
- `dominantMag` — magnitude at `dominantHz`

Notes:
- The implementation performs DC removal (mean subtraction) before FFT.
- Bin 0 (DC) is explicitly ignored (`mag[0] = 0`).
- If you want, you may apply your own signal processing

## Key Constants

In `FFTBuffer.h`:

- `SAMPLE_RATE = 52` Hz
- `DURATION_SEC = 3`
- `RAW_SAMPLES = SAMPLE_RATE * DURATION_SEC` → 156 samples per window
- `FFT_SIZE = 256` (zero-padding is used for samples 156..255)
- `BIN_HZ = SAMPLE_RATE / FFT_SIZE` → 0.203125 Hz/bin

Bin frequency mapping:
- `f(bin) = bin * BIN_HZ`


## Recommended Signal to Feed

Do NOT interleave different axes into a single FFT buffer.

Use either:

### 2 FFTs total (magnitude per sensor)
Compute magnitudes and feed one scalar per tick:

- `acc_mag = sqrt(ax^2 + ay^2 + az^2)`
- `gyro_mag = sqrt(gx^2 + gy^2 + gz^2)`

## Minimal Integration Pattern

### 1) Declare buffers

```cpp
#include "FFTBuffer.h"

FFTBuffer acc_buffer;
FFTBuffer gyro_buffer;
``` 

### 2) Sample loop (52 Hz)

Each tick:

- read one sensor sample

- compute the value you want to analyze

- call addSample() ONCE per buffer per tick

- once full, call process() and read results

```cpp
// Run at ~52 Hz (every ~19.23 ms)
float ax, ay, az, gx, gy, gz;
sensors.readAccel(ax, ay, az);
sensors.readGyro(gx, gy, gz);

float acc_mag  = sqrtf(ax*ax + ay*ay + az*az);
float gyro_mag = sqrtf(gx*gx + gy*gy + gz*gz);

acc_buffer.addSample(acc_mag);
gyro_buffer.addSample(gyro_mag);

if (acc_buffer.isFull() && gyro_buffer.isFull()) {
    acc_buffer.process();
    gyro_buffer.process();

    // Results are now valid until reset()
    // acc_buffer.dominantHz, acc_buffer.dominantMag, acc_buffer.mag[]
    // gyro_buffer.dominantHz, gyro_buffer.dominantMag, gyro_buffer.mag[]

    acc_buffer.reset();
    gyro_buffer.reset();
}
```

## Important Constraints

You must collect samples over real time at 52 Hz. Do NOT fill the buffer in a tight loop.

Do NOT mix axes in the same buffer. Each buffer must represent one time-series.

Do NOT call reset() before consuming results.