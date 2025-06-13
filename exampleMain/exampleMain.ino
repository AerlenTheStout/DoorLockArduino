#include <Servo.h>
#include "src/DoorLock.h"
using namespace DoorLock;
// Dont touch anything above this comment, or the program will not work

void setup() {
  start();
}

void unlock() {
  locked = false;
  open(); // This turns the servo to open
  greenLEDToggle(true); // Turn on the green LED
  buzzerOn(2000); // Turn on the buzzer at 2000Hz
  delay(500); // Wait for 500ms
  buzzerOff(); // Turn off the buzzer
  greenLEDToggle(false); // Turn off the green LED
  resetAttempt(); // Reset the attempt array that holds the previous entered code.
}

void lock() {
  locked = true;
  close(); // This turns the servo to close
  redLEDToggle(true); // Turn on the red LED
  buzzerOn(500); // Turn on the buzzer at 500Hz
  delay(2000); // Wait for 2000ms
  buzzerOff(); // Turn off the buzzer
  redLEDToggle(false); // Turn off the red LED
  resetAttempt(); // Reset the attempt array that holds the previous entered code.
}

void incorrect() {
  redLEDToggle(true); // Turn on the red LED
  buzzerOn(2000); // Turn on the buzzer at 2000Hz
  delay(1000); // Wait for 1000ms
  buzzerOff(); // Turn off the buzzer
  redLEDToggle(false); // Turn off the red LED
  resetAttempt(); // Reset the attempt array that holds the previous entered code.
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