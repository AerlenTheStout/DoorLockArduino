#include <Servo.h>
#include "DoorLock.h"
using namespace DoorLock;
// #define and &&
// #define or ||

void setup() {
  start();
}

void unlock() {
  locked = false;
  open();
  greenLEDToggle(true);
  buzzerOn(1000);
  delay(2000);
  buzzerOff();
  greenLEDToggle(false);
  resetAttempt();
}
void lock() {
  locked = true;
  close();
  redLEDToggle(true);
  buzzerOn(500);
  delay(2000);
  buzzerOff();
  redLEDToggle(false);
  resetAttempt();
}
void incorrect() {
  redLEDToggle(true);
  buzzerOn(2000);
  delay(1000);
  buzzerOff();
  redLEDToggle(false);
  resetAttempt();
}


void loop() {
  scanButtons(); // Update button states
  // put your main code here, to run repeatedly:
  if(isButton1Pressed()) {
    button1Pressed();
  }

  if(isButton2Pressed()) {
    button2Pressed();
  }

  if(isButton3Pressed()) {
    button3Pressed();
  }

  if(isLockButtonPressed()) {
    if (!locked) {
      lock();
    } else if(isAttemptCorrect()) {
      unlock();
    } else if(locked) {
      incorrect();
    }
    delay(250);
  }
}