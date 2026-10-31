#include <Arduino.h>

const int LED = 2;  // onboard LED on ESP32 DevKit V1

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  digitalWrite(LED, HIGH);
  Serial.println("on");
  delay(500);
  digitalWrite(LED, LOW);
  Serial.println("off");
  delay(500);
}