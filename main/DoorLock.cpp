#include <Arduino.h>
#include <Servo.h>
#include "DoorLock.h"


DoorLock::DoorLock(int* correctCode, int codeLength, bool Locked, int button1, int button2, int button3, int lockButton, int greenLED, int redLED, int servoPin, int buzzerPin)
    : button1(button1), button2(button2), button3(button3), lockButton(lockButton), greenLED(greenLED), redLED(redLED), servoPin(servoPin), buzzerPin(buzzerPin)
{
    this->codeLength = codeLength;
    this->correctCode = new int[codeLength];
    this->attempt = new int[codeLength];

    this->locked = Locked;
    for (int i = 0; i < codeLength; i++)
    {
        this->correctCode[i] = correctCode[i];
        this->attempt[i] = 0;
    }

    lastReading = new int[4]{LOW, LOW, LOW, LOW}; // Initialize last readings for buttons
    stableState = new int[4]{LOW, LOW, LOW, LOW}; // Initialize stable states for buttons
    inputIndex = 0;
}

DoorLock::~DoorLock()
{
    delete[] attempt;
    delete[] correctCode;
}

void DoorLock::start() {
  Serial.begin(9600);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(lockButton, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  servo.attach(servoPin);
  servo.write(0);
}

void DoorLock::setCorrectCode(int* code) {
  for (int i = 0; i < codeLength; i++) {
    correctCode[i] = code[i];
  }
}

void DoorLock::setPins(int* pins) {
  button1 = pins[0];
  button2 = pins[1];
  button3 = pins[2];
  lockButton = pins[3];
  greenLED = pins[4];
  redLED = pins[5];
  servoPin = pins[6];
}

void DoorLock::unlock(){
  locked = false;
  servo.write(180);
  digitalWrite(greenLED, HIGH);
  delay(1000);
  digitalWrite(greenLED, LOW);
  resetAttempt();
}

void DoorLock::lock(){
  locked = true;
  servo.write(0);
  digitalWrite(redLED,HIGH);
  delay(1000);
  digitalWrite(redLED, LOW);
  resetAttempt();
}

void DoorLock::incorrect(){
  digitalWrite(redLED,HIGH);
  delay(1000);
  digitalWrite(redLED, LOW);
  resetAttempt();
}


void DoorLock::resetAttempt(){
  for(int i = 0; i < codeLength; i++){
    this->attempt[i] = 0;
  }
  inputIndex = 0;
  Serial.println("Attempt reset.");
}

void DoorLock::button1Pressed() {
  Serial.println("button 1 pressed");
  Serial.print("Input index: ");
  Serial.println(inputIndex);
  Serial.print("Code length: ");
  Serial.println(codeLength);
  if (inputIndex < codeLength) {
    this->attempt[inputIndex] = 1;
    inputIndex++;
    for(int i = 0; i < codeLength; i++)
    {
      Serial.print(this->attempt[i]);
      Serial.print(",");
    }
    Serial.println();
    
  }else{
    Serial.println("Input index out of bounds, resetting attempt.");
  }
  delay(500);
}

void DoorLock::button2Pressed() {
  Serial.println("button 2 pressed");
  if (inputIndex < codeLength){
    this->attempt[inputIndex] = 2;
    inputIndex++;
    for(int i = 0; i < codeLength; i++)
    {
      Serial.print(this->attempt[i]);
      Serial.print(",");
    }
    Serial.println();
  }
  else {
    Serial.println("Input index out of bounds, resetting attempt.");
  }
  delay(500);
}

void DoorLock::button3Pressed() {
  Serial.println("button 3 pressed");
  if (inputIndex < codeLength) {
    this->attempt[inputIndex] = 3;
    inputIndex++;
    for (int i = 0; i < codeLength; i++) {
      Serial.print(this->attempt[i]);
      Serial.print(",");
    }
    Serial.println();
  } else {
    Serial.println("Input index out of bounds, resetting attempt.");
  }
  delay(500);
}

bool DoorLock::isAttemptCorrect() {
  for (int i = 0; i < codeLength; i++) {
      if (this->attempt[i] != this->correctCode[i]) {
          return false;
      }
  }
  return true;
}

bool DoorLock::isButton1Pressed() {
  return digitalRead(button1);
}
bool DoorLock::isButton2Pressed() {
  return digitalRead(button2);
}
bool DoorLock::isButton3Pressed() {
  return digitalRead(button3);
}
bool DoorLock::isLockButtonPressed() {
  return digitalRead(lockButton);
}

void DoorLock::open() { servo.write(180); }
void DoorLock::close() { servo.write(0); }

void DoorLock::redLEDToggle(bool state) {
  if (state) {digitalWrite(redLED, HIGH);} 
  else {digitalWrite(redLED, LOW);}
}

void DoorLock::greenLEDToggle(bool state) {
  if (state) {digitalWrite(greenLED, HIGH);} 
  else {digitalWrite(greenLED, LOW);}
}

void DoorLock::buzzerOn(int hz) {
  tone(buzzerPin, hz);
}
void DoorLock::buzzerOff() {
  noTone(buzzerPin);
}

// void DoorLock::scanButtons() {
//     int button1Read = digitalRead(button1);
//     int button2Read = digitalRead(button2);
//     int button3Read = digitalRead(button3);
//     int lockButtonRead = digitalRead(lockButton);
//     int readings[] = {button1Read, button2Read, button3Read, lockButtonRead};

//     static unsigned long lastDebounceTs[] = {0, 0, 0, 0};
//     const int DEBOUNCE_DELAY = 50; // milliseconds
//     for (int i = 0; i < 4; i++) {
//       if (readings[i] != lastReading[i]) {
//         lastDebounceTs[i] = millis();
//       }
//       if (millis() - lastDebounceTs[i] > DEBOUNCE_DELAY) {
//         if (readings[i] != stableState[i]) {
//           stableState[i] = readings[i];
//         }
//       }
//     }
//   }
