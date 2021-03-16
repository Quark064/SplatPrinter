#include "Payload.h"
#include <avr/pgmspace.h>


const int A_PIN = 8;
const int R_PIN = 7;
const int L_PIN = 6;
const int D_PIN = 5;
const int buttonTimeDown = 45;
const int delayAfter = 50;

void goRight() {
  digitalWrite(R_PIN, HIGH);
  delay(buttonTimeDown);
  digitalWrite(R_PIN, LOW);
  delay(delayAfter);
}

void returnLeft() {
  digitalWrite(L_PIN, HIGH);
  delay(6000);
  digitalWrite(L_PIN, LOW);
  delay(delayAfter);
}

void goDown() {
  digitalWrite(D_PIN, HIGH);
  delay(buttonTimeDown);
  digitalWrite(D_PIN, LOW);
  delay(delayAfter);
}

void pressA() {
  digitalWrite(A_PIN, HIGH);
  delay(buttonTimeDown);
  digitalWrite(A_PIN, LOW);
  delay(delayAfter);
}

int checkVal(int value, int rawNumber) {
    if ((rawNumber | value) == 255) {
      return 1;
    }
    else {
      return 0;
    }
}

void doStuff(const uint8_t *PAYLOAD) {
    uint8_t compareArr[8] = {127, 191, 223, 239, 247, 251, 253, 254};
    int masterCounter = 0, subCounter = 0, i = 0;

    for (masterCounter = 0; masterCounter < 120; masterCounter++) {
      for (subCounter = 0; subCounter < 40; subCounter++) {
        for (i = 0; i < 8; i++) {
           if (checkVal(compareArr[i], (int)pgm_read_byte(PAYLOAD + subCounter + (masterCounter * 40))) == 0) {
            pressA();
           }
           goRight();
        }
      }
    if (masterCounter != 199) {
        goDown();
        returnLeft();
       }
    }
}


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1);
  digitalWrite(A_PIN, LOW);
  digitalWrite(D_PIN, LOW);
  digitalWrite(L_PIN, LOW);
  digitalWrite(R_PIN, LOW);
}

void loop() {
  while(!Serial.available());
  doStuff(PAYLOAD);
  while(1);
}
