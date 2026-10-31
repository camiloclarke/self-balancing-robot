#include <Arduino.h>
#include <Wire.h>

const uint8_t MPU = 0x68;
float gyroBias = 0;
float angle = 0;
unsigned long lastMicros;

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(MPU);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void readIMU(int16_t &ay, int16_t &az, int16_t &gx) {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);                 // start of accel data
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, (uint8_t)14);
  int16_t d[7];
  for (int i = 0; i < 7; i++) d[i] = (Wire.read() << 8) | Wire.read();
  ay = d[1]; az = d[2]; gx = d[4];  // ax, ay, az, temp, gx, gy, gz
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);               // SDA, SCL

  Wire.beginTransmission(MPU);
  if (Wire.endTransmission() != 0) {
    Serial.println("MPU6050 not found - check wiring");
    while (true) delay(1000);
  }
  writeReg(0x6B, 0);                // wake it up

  Serial.println("Calibrating, keep still...");
  int16_t ay, az, gx;
  for (int i = 0; i < 500; i++) {
    readIMU(ay, az, gx);
    gyroBias += gx;
    delay(2);
  }
  gyroBias /= 500;
  lastMicros = micros();
}

void loop() {
  int16_t ay, az, gx;
  readIMU(ay, az, gx);

  float dt = (micros() - lastMicros) / 1e6;
  lastMicros = micros();

  float accAngle = atan2(ay, az) * 180 / PI;                 // tilt from gravity: noisy, no drift
  float gyroRate = (gx - gyroBias) / 131.0;                  // deg/s: smooth, but drifts
  angle = 0.98 * (angle + gyroRate * dt) + 0.02 * accAngle;  // complementary filter

  Serial.printf(">acc:%.2f\n>filtered:%.2f\n", accAngle, angle);
  delay(10);
}