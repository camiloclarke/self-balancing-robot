#include <Arduino.h>

const int PWMA = 32, AIN1 = 25, AIN2 = 33;
const int PWMB = 14, BIN1 = 26, BIN2 = 27;
const int STBY = 4;
const int FREQ = 20000, RES = 8;

void setMotor(int pwmPin, int in1, int in2, int speed) {
  digitalWrite(in1, speed > 0);
  digitalWrite(in2, speed < 0);
  ledcWrite(pwmPin, abs(speed));
}

void ramp(const char* name, int pwmPin, int in1, int in2, int dir) {
  for (int pwm = 0; pwm <= 255; pwm += 5) {
    setMotor(pwmPin, in1, in2, dir * pwm);
    Serial.printf("%s %s pwm=%d\n", name, dir > 0 ? "fwd" : "rev", pwm);
    delay(300);
  }
  setMotor(pwmPin, in1, in2, 0);
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  int pins[] = {AIN1, AIN2, BIN1, BIN2, STBY};
  for (int p : pins) pinMode(p, OUTPUT);
  digitalWrite(STBY, HIGH);          // enable driver
  ledcAttach(PWMA, FREQ, RES);
  ledcAttach(PWMB, FREQ, RES);
  delay(2000);
}

void loop() {
  ramp("A", PWMA, AIN1, AIN2, 1);
  ramp("A", PWMA, AIN1, AIN2, -1);
  ramp("B", PWMB, BIN1, BIN2, 1);
  ramp("B", PWMB, BIN1, BIN2, -1);
}