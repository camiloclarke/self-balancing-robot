# Self-Balancing Robot

A two-wheeled self-balancing robot built around an ESP32. The aim is to take the numerical methods and state estimation from my earlier projects ([Pendulum-ODE-Solver](https://github.com/camiloclarke/Pendulum-ODE-Solver), [Kalman-Filter-Pendulum](https://github.com/camiloclarke/Kalman-Filter-Pendulum)) and apply them to real, noisy hardware: sensor fusion for tilt estimation, then PID control to keep an inverted pendulum upright.

**Status:** in progress. Each subsystem is being tested on its own before full assembly.

## Hardware

| Part | Role |
|---|---|
| ESP32 DevKit V1 | Microcontroller (Arduino framework, PlatformIO) |
| MPU6050 (GY-521 / HW-123) | 3-axis accelerometer + gyroscope for tilt |
| TB6612FNG | Dual H-bridge motor driver |
| 2× N20 gear motors with magnetic encoders | Drive wheels + wheel speed feedback |
| 2× 18650 Li-ion (2S, 7.4 V) | Main power |
| Buck converter | Battery → 5 V for the ESP32 (not yet fitted) |

## Wiring (current bench setup)

**MPU6050 → ESP32** (I2C)

| MPU6050 | ESP32 |
|---|---|
| VCC | 5V (board has its own 3.3 V regulator) |
| GND | GND |
| SCL | GPIO 22 |
| SDA | GPIO 21 |

**TB6612FNG → ESP32**

| TB6612 | ESP32 / other |
|---|---|
| VCC (logic) | 3V3 |
| VM (motor supply) | 5V from USB for bench testing, battery later |
| GND | GND (common ground) |
| STBY | GPIO 4 (set HIGH in software, so motors can be cut in code) |
| PWMA / AIN1 / AIN2 | GPIO 32 / 25 / 33 |
| PWMB / BIN1 / BIN2 | GPIO 14 / 26 / 27 |
| AO1 / AO2, BO1 / BO2 | Motor A and Motor B power wires |

**N20 encoder cable colours** (checked against the labels on the encoder board; not the usual convention, so worth noting):

| Pad | Wire |
|---|---|
| M1 | White |
| M2 | Red |
| VCC | **Black** |
| GND | **Blue** |
| C1 (A) | Green |
| C2 (B) | Yellow |

**Encoders → ESP32** (same colours on both motors)

| Encoder wire | Motor A | Motor B |
|---|---|---|
| Black (VCC) | 3V3 | 3V3 |
| Blue (GND) | GND | GND |
| Green (C1 / A) | GPIO 18 | GPIO 16 |
| Yellow (C2 / B) | GPIO 19 | GPIO 17 |

## Progress log

### 1. Toolchain (Blink + serial)
PlatformIO set up in VS Code, onboard LED blinking and serial output at 115200 baud.

### 2. IMU + complementary filter
- Read raw accelerometer and gyro data directly from the MPU6050 registers over I2C.
- Gyro bias calibrated at startup by averaging 500 readings while still.
- Tilt from the accelerometer: `atan2(ay, az)`. Correct on average but noisy and affected by vibration.
- Tilt from the gyro: integrate angular rate. Smooth but drifts over time.
- Fused with a complementary filter: `angle = 0.98 × (angle + gyroRate × dt) + 0.02 × accAngle`. The gyro is trusted short-term and the accelerometer slowly corrects drift.
- Confirmed the code measures rotation about the board's **X axis**, so the IMU must be mounted with the X arrow parallel to the wheel axle.

### 3. Motor driver test + deadband
- Each motor ramped from PWM 0 → 255 in both directions using 20 kHz, 8-bit PWM.
- **Deadband ≈ 30 / 255 PWM** on 5 V USB power: below this the motors don't overcome static friction. This matters for balancing, since small corrections near upright will be lost unless the controller compensates for it.
- To re-measure on the 7.4 V battery (expected to be lower) and again with wheels and the robot's weight on.
- Note: the ESP32 Arduino core 3.x replaced `ledcSetup` / `ledcAttachPin` with `ledcAttach(pin, freq, res)`, and `ledcWrite` now takes the pin rather than a channel.

### 4. Encoders
- Quadrature encoders read via interrupts on channel A (both edges), with channel B giving direction.
- Measured **~720 counts per output shaft revolution** (≈24 counts per motor revolution × ~30:1 gearbox), consistent across both motors.
- Encoder VCC on 3.3 V so the signal outputs are safe for the ESP32 inputs.

## Next steps
- [x] Read wheel encoders with interrupts
- [ ] Check both encoders count positive when the robot drives forwards (fix signs in code)
- [ ] Battery + buck converter power wiring (set buck output to 5 V with a multimeter first)
- [ ] Replace the complementary filter with my own Kalman filter
- [ ] Build the chassis (battery high, IMU rigid and close to the axle)
- [ ] PID balancing: P → PD → small I, then an outer speed loop to stop drift
- [ ] Wireless telemetry to a Python dashboard
