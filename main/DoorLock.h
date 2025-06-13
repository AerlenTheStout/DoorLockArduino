
#ifndef ARDUINO_DOORLOCK_H
#define ARDUINO_DOORLOCK_H

class DoorLock  
{
	private:
		int* correctCode;
		int inputIndex = 0;

		int button1 = 4;
		int button2 = 3;
		int button3 = 2;
		int lockButton = 5;
		int redLED = 8;
		int greenLED = 7;
		int servoPin = 9;
		int buzzerPin = 12;

		int* lastReading;
		int* stableState;

		Servo servo;
		void scanButtons();
	public:

		DoorLock(int* correctCode, int codeLength = 3, bool Locked = true, int button1 = 4, int button2 = 3, int button3 = 2, int lockButton = 5, int greenLED = 7, int redLED = 8, int servoPin = 9, int buzzerPin = 12);
		~DoorLock();
		void start();
		void unlock();
		void lock();
		void incorrect();
        void resetAttempt();
        void setCorrectCode(int* code);
		void setPins(int* pins);
		void button1Pressed();
		void button2Pressed();
		void button3Pressed();
		bool isAttemptCorrect();
		bool isButton1Pressed();
		bool isButton2Pressed();
		bool isButton3Pressed();
		bool isLockButtonPressed();

		void open();
		void close();
		void redLEDToggle(bool state);
		void greenLEDToggle(bool state);
		void buzzerOn(int hz);
		void buzzerOff();

		int getButton1() { return button1; }
		int getButton2() { return button2; }
		int getButton3() { return button3; }
		int getLockButton() { return lockButton; }
		int getGreenLED() { return greenLED; }
		int getRedLED() { return redLED; }
		int getServoPin() { return servoPin; }
		int getBuzzerPin() { return buzzerPin; }
		int codeLength;
		bool locked;
		int* attempt;



};
#endif
