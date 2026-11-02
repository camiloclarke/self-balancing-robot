#include <Arduino.h>

const int ENC_A1 = 18, ENC_A2 = 19;   // motor A: green, yellow
const int ENC_B1 = 16, ENC_B2 = 17;   // motor B: green, yellow

volatile long countA = 0, countB = 0;

// runs every time channel 1 changes; channel 2 tells us direction
void IRAM_ATTR isrA() {
  if (digitalRead(ENC_A1) == digitalRead(ENC_A2)) countA++; else countA--;
}
void IRAM_ATTR isrB() {
  if (digitalRead(ENC_B1) == digitalRead(ENC_B2)) countB++; else countB--;
}

void setup() {
  Serial.begin(115200);
  int pins[] = {ENC_A1, ENC_A2, ENC_B1, ENC_B2};
  for (int p : pins) pinMode(p, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A1), isrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B1), isrB, CHANGE);
}

void loop() {
  Serial.printf("A: %ld   B: %ld\n", countA, countB);
  delay(100);
}