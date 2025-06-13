#include <Servo.h> // this lets us use the Servo
#include "Library/DoorLock.h" // This includes the DoorLock library, which contains all the methods we need to control the door lock system.
using namespace DoorLock; // This imports the Door Lock interface, which makes the methods like isButton1Pressed() and start() available to use.
// Dont touch anything above this comment, or the program will not work

// This method is created and used by the arduino. Anything inside this method will run once when the arduino starts up.
void setup() {
  start(); // This start method call starts the door lock system and connects this code to the arduino.
}

// This is a method you are going to have to implement yourself.
// This method should be run when the door lock is unlocked.
// inside you can put anything you want to happen when the door lock is unlocked.
void unlock() {

  /*Your Code goes here!*/

}

// This is a method you are going to have to implement yourself.
// This method should be run when the door lock is locked.
// inside you can put anything you want to happen when the door lock is locked.
void lock() {

  /*Your Code goes here!*/

}

// This is a method you are going to have to implement yourself.
// This method should be run when the door lock is incorrect.
// inside you can put anything you want to happen when the door lock is incorrect.
void incorrect() {

  /*Your Code goes here!*/

}

// This method is called repeatedly by the arduino very quickly. Anything inside this method will run over and over again.
void loop() {
  scanButtons();// This method updates the button states and checks if any buttons are pressed so that the other methods like button1Pressed() will function.

  /*Your Code goes here!*/

}