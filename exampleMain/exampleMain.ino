#include <Servo.h>
#include "DoorLock.h"
using namespace DoorLock;
// Dont touch anything above this comment, or the program will not work

void setup() {
  start();
}

void unlock() {
  locked = false;
  open();
  greenLEDToggle(true);
  buzzerOn(2000);
  delay(500);
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

  // Check if button 1 is pressed, if it is register that button 1 was pressed
  if(isButton1Pressed()) {
    button1Pressed();
  }

  // Check if button 2 is pressed, if it is register that button 2 was pressed
  if(isButton2Pressed()) {
    button2Pressed();
  }

  // Check if button 3 is pressed, if it is register that button 3 was pressed
  if(isButton3Pressed()) {
    button3Pressed();
  }

  // Check if the lock button is pressed, if it is, check if the door is locked or unlocked
  // if the door is unlocked, Lock it.
  // if the door is locked, check if the attempt is correct, if it is, unlock the door, otherwise do the incorrect action.
  if(isLockButtonPressed()) {
    if (!locked) {
      lock();
    } else if(isAttemptCorrect()) {
      unlock();
    } else if(locked) {
      incorrect();
    }
  }
}