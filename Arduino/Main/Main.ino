#include "Payload.h"
#include <avr/pgmspace.h>


const int A_PIN = 8;
const int R_PIN = 7;
const int L_PIN = 6;
const int D_PIN = 5;
const int buttonTimeDown = 45;
const int delayAfter = 50;
const int blockOffset = 40;
const int returnTimeMultiplier = 300;

char cursorOnSide = 'L';

void finishBlink() {
  while(1) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}

void goRight() {
  digitalWrite(R_PIN, HIGH);
  delay(buttonTimeDown);
  digitalWrite(R_PIN, LOW);
  delay(delayAfter);
}

void goLeft() {
  digitalWrite(L_PIN, HIGH);
  delay(buttonTimeDown);
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

void returnRight(int delayTime) {
  digitalWrite(R_PIN, HIGH);
  delay(delayTime);
  digitalWrite(R_PIN, LOW);
}

void returnLeft(int delayTime) {
  digitalWrite(L_PIN, HIGH);
  delay(delayTime);
  digitalWrite(L_PIN, LOW);
}

void printBlackLine(char direction) {
  digitalWrite(A_PIN, HIGH);
  delay(45);

  if (direction == 'L') {
    digitalWrite(R_PIN, HIGH);
    delay(6000);
    digitalWrite(R_PIN, LOW);
  }
  else {
    digitalWrite(L_PIN, HIGH);
    delay(6000);
    digitalWrite(L_PIN, LOW);
  }
  digitalWrite(A_PIN, LOW);
  swap(&cursorOnSide);
}



int checkVal(int value, int rawNumber) {
    if ((rawNumber | value) == 255) {
      return 1;
    }
    else {
      return 0;
    }
}

void swap(char *toSwap) {
      if (*toSwap == 'L') {
        *toSwap = 'R';
      }
      else {
        *toSwap = 'L';
      }
    }



int anyRemaining(const uint8_t *passedReadAddress, int offset, char onSide) {
  int i;

  if (onSide == 'L') {
    for (i = offset; i < 40; i++) {
      if ((int)pgm_read_byte(passedReadAddress + i) != 255) return 1;
    }
    return 0;
  }
  else {
    for (i = (39 + offset); i >= 0; i--) {
       if ((int)pgm_read_byte(passedReadAddress + i) != 255) return 1;
    }
    return 0;
  }
}

uint8_t compareArr[8] = {127, 191, 223, 239, 247, 251, 253, 254};

void printRightToLeft(const uint8_t *passedReadAddress) {
    int i;
    int localCount;
    int localOffset = 0;

    for (localCount = 0; localCount < 40; localCount++) {

        for (i = 7; i >= 0; i--) {
            if (!checkVal(compareArr[i], (int)pgm_read_byte(passedReadAddress + localOffset + 39))) {
              pressA();
            }
            goLeft();
        }
        localOffset--;
        if (!anyRemaining(passedReadAddress, localOffset, 'R')) {
          if (localOffset >= -19) {
            returnRight((localOffset*-1)*returnTimeMultiplier);

            // DEBUG PRINT STATEMENTS (TO BE REMOVED)
            Serial.print("Returned: R, Delay: ");
            Serial.print((localOffset*-1)*returnTimeMultiplier);
            Serial.print("\n");
            return;
          }
          else {
            returnLeft((localOffset + blockOffset)*returnTimeMultiplier*-1);

            // DEBUG PRINT STATEMENTS (TO BE REMOVED)
            Serial.print("Returned: L, Delay: ");
            Serial.print((localOffset + blockOffset)*returnTimeMultiplier*-1);
            Serial.print("\n");

            swap(&cursorOnSide);
            return;
          }
        }
    }
    swap(&cursorOnSide);
}

void printLeftToRight(const uint8_t *passedReadAddress) {
  int i;
  int localCount;
  int localOffset = 0;

  for (localCount = 0; localCount < 40; localCount++) {

      for (i = 0; i < 8; i++) {
          if (!checkVal(compareArr[i], (int)pgm_read_byte(passedReadAddress + localOffset))) {
            pressA();
          }
          goRight();
      }
      localOffset++;
      if (!anyRemaining(passedReadAddress, localOffset, 'L') && (localOffset < 35)) {
          if (localOffset <= 19) {
            returnLeft((localOffset)*returnTimeMultiplier);

            // DEBUG PRINT STATEMENTS (TO BE REMOVED)
            Serial.print("Returned: L, Delay: ");
            Serial.print((localOffset)*returnTimeMultiplier);
            Serial.print("\n");

            return;
          }
          else {
            returnRight((blockOffset - localOffset)*returnTimeMultiplier);

            // DEBUG PRINT STATEMENTS (TO BE REMOVED)
            Serial.print("Returned: R, Delay: ");
            Serial.print((blockOffset - localOffset)*returnTimeMultiplier);
            Serial.print("\n");

            swap(&cursorOnSide);
            return;
          }
        }
  }
  swap(&cursorOnSide);
}

// 0 None; 1 White; 2 Black
int checkIfLineClear(const uint8_t *passedReadAddress) {
    int i;
    int blackFlag = 1, whiteFlag = 1;

    for (i = 0; i < 40; i++) {
      if ((int)pgm_read_byte(passedReadAddress + i) != 255) {
        whiteFlag = 0;
      }
      if ((int)pgm_read_byte(passedReadAddress + i) != 0) {
        blackFlag = 0;
      }
    }

    if (whiteFlag) return 1;
    if (blackFlag) return 2;
    return 0;

    }



void doStuff(const uint8_t *currentReadAddress) {
    int i;
    int countingOffset = 0;


    for (i = 0; i < 120; i ++) {
      if (checkIfLineClear(currentReadAddress + countingOffset) == 1) {
        goDown();

        // DEBUG PRINT STATEMENTS (TO BE REMOVED)
        Serial.print("White line detected, skipping row ");
        Serial.print(i+1);

        Serial.print("\n");
      }
      else if (checkIfLineClear(currentReadAddress + countingOffset) == 2) {

        // DEBUG PRINT STATEMENTS (TO BE REMOVED)
        Serial.print("Black line detected, printing row ");
        Serial.print(i+1);
        Serial.print(" from ");
        Serial.print(cursorOnSide);

        printBlackLine(cursorOnSide);
        goDown();
      }
      else {
        if (cursorOnSide == 'L') {
          printLeftToRight(currentReadAddress + countingOffset);
          goDown();
        }
        else {
          printRightToLeft(currentReadAddress + countingOffset);
          goDown();
        }
      }

    countingOffset += blockOffset;

    }
}


void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1);
  digitalWrite(A_PIN, LOW);
  digitalWrite(D_PIN, LOW);
  digitalWrite(L_PIN, LOW);
  digitalWrite(R_PIN, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  while(!Serial.available());
  doStuff(PAYLOAD);
  finishBlink();
}
