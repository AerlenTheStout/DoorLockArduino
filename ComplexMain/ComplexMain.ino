#include <Servo.h>
#include <Arduino.h>
// #define and &&
// #define or ||

const int button1 = 4;
const int button2 = 3;
const int button3 = 2;
const int lockButton = 5;
const int redLED = 8;
const int greenLED = 7;
const int servoPin = 9;

Servo servo;
void setup() {
  Serial.begin(9600);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(lockButton, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  servo.attach(servoPin);
  servo.write(0);
}



//You dont have to worry about anything above this line, it is pruely just project setup
int attempt[3] = {0, 0, 0}; // Array to store the user's input
int correctCode[3] = {1, 2, 3}; // Correct code to unlock the door
int inputIndex = 0; // Index for the user's input
bool locked = true; // State of the door lock
int codeLength = 3; // Length of the code to be entered



void unlock(){
  locked = false;
  servo.write(180);
  digitalWrite(greenLED, HIGH);
  delay(2000);
  digitalWrite(greenLED, LOW);
  resetAttempt();
}

void lock(){
  locked = true;
  servo.write(0);
  digitalWrite(redLED,HIGH);
  delay(2000);
  digitalWrite(redLED, LOW);
  resetAttempt();
}

void incorrect(){
  digitalWrite(redLED,HIGH);
  delay(1000);
  digitalWrite(redLED, LOW);
  resetAttempt();
}

void resetAttempt(){
  for(int i = 0; i < sizeof(attempt); i++){
    attempt[i] = 0;
  }
}





void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(button1)){
    Serial.println("button 1 pressed");
    if (inputIndex < codeLength) {
      attempt[inputIndex] = 1;
      inputIndex++;
    }
    delay(500);
  }

  if(digitalRead(button2)){
    Serial.println("button 2 pressed");
    if (inputIndex < codeLength) {
      attempt[inputIndex] = 2;
      inputIndex++;
    }
    delay(500);
  }

  if(digitalRead(button3)){
    Serial.println("button 3 pressed");
    if(inputIndex < codeLength) {
      attempt[inputIndex] = 3;
      inputIndex++;
    }
    delay(500);
  }

  if(digitalRead(lockButton)){
    if (!locked) {
      lock();
    }
    if(locked and attempt[0] == correctCode[0] and attempt[1] == correctCode[1] and attempt[2] == correctCode[2]) {
      unlock();
    } else incorrect();
    delay(250);
  }
}