#ifndef ARDUINO_DOORLOCK_H
#define ARDUINO_DOORLOCK_H

#include <Arduino.h> // Required for Arduino specific functions like pinMode, digitalWrite, etc.
#include <Servo.h>   // Required for the Servo library

// --- Global Constants for Default Pin Assignments and Code ---
// These make it easy for campers to see what pins are used by default
// and to easily change them if they want to.
const int DOORLOCK_BUTTON1_PIN = 4;
const int DOORLOCK_BUTTON2_PIN = 3;
const int DOORLOCK_BUTTON3_PIN = 2;
const int DOORLOCK_LOCK_BUTTON_PIN = 5;
const int DOORLOCK_RED_LED_PIN = 8;
const int DOORLOCK_GREEN_LED_PIN = 7;
const int DOORLOCK_SERVO_PIN = 9;
const int DOORLOCK_BUZZER_PIN = 12;

// Default secret code for the door lock (e.g., 1-2-3)
const int DOORLOCK_DEFAULT_CODE[] = {1, 2, 3};
const int DOORLOCK_DEFAULT_CODE_LENGTH = 3;

// --- Internal Implementation Class ---
// This class holds all the actual state and logic for the door lock.
// It's given a leading underscore to indicate it's for internal library use,
// not something users should directly create instances of.
class _DoorLockImpl
{
private:
    int* _correctCode;   // Dynamically allocated array for the secret code
    int _codeLength;     // Length of the secret code
    int _inputIndex = 0; // Current index for code input attempt
	
    // Pin assignments for hardware components
    int _button1;
    int _button2;
    int _button3;
    int _lockButton;
    int _redLED;
    int _greenLED;
    int _servoPin;
    int _buzzerPin;

    // Variables for button debouncing (original names: lastReading, stableState)
    int* _lastReading; // Array to store last reading for each button
    int* _stableState; // Array to store stable state for each button
	unsigned long _lastDebounceTs[4] = {0, 0, 0, 0}; // Timestamps for debouncing
	bool _buttonJustPressedFlags[4] = {false, false, false, false}; // Flags for one-shot button press detection

    Servo _servo; // Servo object (original name: servo)

    // Original private helper method

    // Public member (original: int* attempt;)
    // Keeping this private within _DoorLockImpl for better encapsulation
    int* _attempt; // Stores the current code attempt
	
public: // Changed constructors to PUBLIC access
    bool locked = true; // Current locked/unlocked state of the door (renamed to avoid conflict)
    // Public constructors for internal class, allowing global instantiation
    _DoorLockImpl(int* correctCode, int codeLength, bool Locked,
                  int button1, int button2, int button3, int lockButton,
                  int greenLED, int redLED, int servoPin, int buzzerPin);

	_DoorLockImpl(const int* correctCode, int codeLength, bool Locked, int button1, int button2, int button3, int lockButton, int greenLED, int redLED, int servoPin, int buzzerPin)
		: _DoorLockImpl(const_cast<int*>(correctCode), codeLength, Locked, button1, button2, button3, lockButton, greenLED, redLED, servoPin, buzzerPin) {};

    _DoorLockImpl(); // Default constructor, now public

    // Public destructor to properly clean up dynamically allocated memory
    ~_DoorLockImpl();

    // --- Core Public Methods (Original Names) ---

    void start();
    void scanButtons();

    void DoorUnlock();
    void DoorLock(); 
    void DoorIncorrect();

    void resetAttempt();
    bool isAttemptCorrect();
    
    void setCorrectCode(int *code, int codeLength);
    void setPins(int button1, int button2, int button3, int lockButton, int greenLED, int redLED, int servoPin, int buzzerPin);
    
    
    bool isButton1Pressed();
    bool isButton2Pressed();
    bool isButton3Pressed();
    bool isLockButtonPressed();

    void button1Pressed();
    void button2Pressed();
    void button3Pressed();
    
    void open();
    void close();

    void redLEDToggle(bool state);
    void greenLEDToggle(bool state);

    void buzzerOn(int hz);
    void buzzerOff();

    // Getter methods (original names)
    int getButton1() { return _button1; }
    int getButton2() { return _button2; }
    int getButton3() { return _button3; }
    int getLockButton() { return _lockButton; }
    int getGreenLED() { return _greenLED; }
    int getRedLED() { return _redLED; }
    int getServoPin() { return _servoPin; }
    int getBuzzerPin() { return _buzzerPin; }

    // Public member (original: int* attempt;)
    // int* _attempt; // This is now private and managed internally.
};


// --- Public-Facing Namespace for Campers ---
// This namespace provides the simple, direct function calls for campers.
// They will use these functions like `DoorLock::unlock()` or `DoorLock::button1Pressed()`.
namespace DoorLock {
	// This variable stores the current locked state of the door.
	extern bool locked;


    void start(); 
    void start(int* correctCode, int codeLength);
    void start(int button1, int button2, int button3, int lockButton,
               int greenLED, int redLED, int servoPin, int buzzerPin);
    void start(int* correctCode, int codeLength, int button1, int button2, int button3,
               int lockButton, int greenLED, int redLED, int servoPin, int buzzerPin);

	void scanButtons();

    
    void DoorUnlock();
    void DoorLock();
    void open();
    void close();

    void DoorIncorrect();
    void resetAttempt();
    bool isAttemptCorrect();

    void setCorrectCode(int* code);
    void setPins(int* pins); // Assumes pins array has a fixed order of all pins

    void button1Pressed();
    void button2Pressed();
    void button3Pressed();

    bool isButton1Pressed();
    bool isButton2Pressed();
    bool isButton3Pressed();
    bool isLockButtonPressed();

    void redLEDToggle(bool state);
    void greenLEDToggle(bool state);

    void buzzerOn(int hz);
    void buzzerOff();

    int getButton1();
    int getButton2();
    int getButton3();
    int getLockButton();
    int getGreenLED();
    int getRedLED();
    int getServoPin();
    int getBuzzerPin();

} // end namespace DoorLock

#endif // ARDUINO_DOORLOCK_H
