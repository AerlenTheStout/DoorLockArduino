#include <Servo.h>
#include "src/DoorLock.h"
using namespace DoorLock;
// Dont touch anything above this comment, or the program will not work


// This setup method is an example of how we would set a custom code and start the door lock system.
/* void setup() {
  int array[3] = {3, 2, 3};
  start(array, 3);
} */


// This setup method is an example of another way to set a custom code and start the door lock system.
/* void setup() {
  start();
  int array[3] = {1, 2, 3};
  setCorrectCode(array, 3);
  } */
 
 // This way to start the door lock system is an example of how to set a custom code using the door lock buttons itself
 
 /* void setup() {
  start();
  setPins(2, 3, 4, 5, 9, 8, 11, 13); // Set the pins for buttons, LEDs, servo and buzzer


  Serial.println(digitalRead(5) == HIGH);
  if(digitalRead(5) == HIGH) {
    int index = 0;
    int customCode[] = {0,0,0};
    Serial.println("Press the buttons to set a custom code. Press the lock button when done.");
    while (true)
    {
      Serial.print("looping");
      scanButtons();
      if (isButton1Pressed())
      {
        customCode[index] = 1;
        index++;
      } else if(isButton2Pressed())
      {
        customCode[index] = 2;
        index++;
      } else if (isButton3Pressed())
      {
        customCode[index] = 3;
        index++;
      }
      if (index >= 3 && isLockButtonPressed()) {
        for (int i = 0; i < 3; i++) {
          Serial.print(customCode[i]);
          Serial.print(" ");
        }
        setCorrectCode(customCode, 3);
        break; // Exit the loop when we have 3 digits
      }
    }
  } else {
    // This is an example of how to set a custom code.
    randomSeed(analogRead(0) + millis()); // Better random seed using analog noise + time
    int array[3];
    for (int i = 0; i < 3; i++) {
      array[i] = random(1, 4); // random(1,4) generates 1,2,3
      delay(10); // Small delay to ensure different random values
    }
    setCorrectCode(array, 3);

    Serial.print("Custom code set to: ");
    for (int i = 0; i < 3; i++) {
      Serial.print(array[i]);
      Serial.print(" ");
    }
  }


  // This is an example of how to set the pins for the buttons, LEDs, servo and buzzer.
} */



// This setup method is an example of how to start the door lock system with a custom code and set the pins for the buttons, LEDs, servo and buzzer.
/* void setup() {

  int array[4] = {3, 3, 3, 3}; 
  
  start(array, 4);
  
  // This is an example of how to set a custom code.
  setCorrectCode(array, 4); 

  // This is an example of how to set the pins for the buttons, LEDs, servo and buzzer.
  setPins(2, 3, 4, 5, 9, 8, 11, 13); // Set the pins for buttons, LEDs, servo and buzzer
} */

// This is an example of an extra thing the youth might want to do, this lets them grab the servo object and use it.
/* 
void setup() {
  start();
  Servo& servo = getServo();
  servo.attach(getServoPin()); // Attach the servo to the specified pin

  servo.write(0); // Initialize the servo to the closed position
} */

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