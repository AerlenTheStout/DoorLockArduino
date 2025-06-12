#include <Servo.h>
#include <DoorLock.h>
// #define and &&
// #define or ||

DoorLock doorLock = DoorLock(new int[3]{1, 2, 3}, 3, true);

void unlock() {
  doorLock.locked = false;
  doorLock.open();
  doorLock.redLEDToggle(false);
  tone(doorLock.getBuzzerPin(), 1000);
  delay(2000);
  digitalWrite(doorLock.getGreenLED(), LOW);
  doorLock.resetAttempt();
}
void lock() {
  doorLock.locked = true;
  doorLock.close();
  digitalWrite(doorLock.getRedLED(), HIGH);
  delay(2000);
  digitalWrite(doorLock.getRedLED(), LOW);
  doorLock.resetAttempt();
}
void incorrect() {
  digitalWrite(doorLock.getRedLED(), HIGH);
  delay(1000);
  digitalWrite(doorLock.getRedLED(), LOW);
  doorLock.resetAttempt();
}

void setup() {
  doorLock.start();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(doorLock.isButton1Pressed()) {
    doorLock.button1Pressed();
  }

  if(doorLock.isButton2Pressed()) {
    doorLock.button2Pressed();
  }

  if(doorLock.isButton3Pressed()) {
    doorLock.button3Pressed();
  }

  if(doorLock.isLockButtonPressed()) {
    if (!doorLock.locked) {
      doorLock.lock();
      doorLock.buzz(1000);
    }else if(doorLock.isAttemptCorrect()) {
      doorLock.unlock();
      doorLock.buzz(200);
      delay(100);
      doorLock.buzz(200);
    } else if(doorLock.locked) {
      doorLock.incorrect();
      doorLock.buzz(1000);
    }
    delay(250);
  }
}