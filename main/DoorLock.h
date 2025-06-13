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
int DOORLOCK_DEFAULT_CODE[] = {1, 2, 3};
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
	bool _locked = true; // Current locked/unlocked state of the door (renamed to avoid conflict)
	
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
	
    Servo _servo; // Servo object (original name: servo)
	
    // Private constructor to prevent direct instantiation by users.
    // This ensures only the library creates the single instance.
    // It handles initializing all pins and code.
    _DoorLockImpl(int* correctCode, int codeLength, bool Locked, int button1, int button2, int button3, int lockButton, int greenLED, int redLED, int servoPin, int buzzerPin);
		
		// Default private constructor, delegates to the full constructor with defaults
		_DoorLockImpl();
		
		// Original private helper method
		void scanButtons();
		
		// Public member (original: int* attempt;)
		// Keeping this private within _DoorLockImpl for better encapsulation
		int* _attempt; // Stores the current code attempt
		
		public:
		// Public destructor to properly clean up dynamically allocated memory
		~_DoorLockImpl();
		
		// --- Core Public Methods (Original Names) ---

		// Initializes all hardware pins and sets initial lock state (original: start)
		void start();
		
		// Lock Actions (original names)
		void DoorUnlock();
		void DoorLock(); 
		void open();      // Original name
		void close();     // Original name
		
		// Code entry and verification (original names)
		void DoorIncorrect(); // Original name, includes LED and reset
		void resetAttempt(); // Original name
		bool isAttemptCorrect(); // Original name
		
		// Functions to set configurations (original names)
		void setCorrectCode(int* code);
		void setPins(int* pins); // Assumes pins array has a fixed order of all pins
		
		// Button press handlers (original names)
		void button1Pressed();
		void button2Pressed();
		void button3Pressed();
		
		// Button status checks (original names)
		bool isButton1Pressed();
		bool isButton2Pressed();
		bool isButton3Pressed();
		bool isLockButtonPressed();
		
		// LED Control (original names)
		void redLEDToggle(bool state);
		void greenLEDToggle(bool state);
		
		// Buzzer Control (original names)
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
	
    // Public members from original class (moved to private and handled internally)
    // int codeLength; // Now _codeLength
    // bool locked;     // Now _isLocked
    // int* attempt;    // Now _attempt
};


// --- Public-Facing Namespace for Campers ---
// This namespace provides the simple, direct function calls for campers.
// They will use these functions like `DoorLock::unlock()` or `DoorLock::button1Pressed()`.
namespace DoorLock {

	// Expose the locked state as a public variable
	extern bool locked;

	// Initialization function (original: start)
    // We'll create overloads for different initialization scenarios
    void start(); // Uses default pins and code
    void start(int* correctCode, int codeLength); // Custom code, default pins
    void start(int button1, int button2, int button3, int lockButton,
		int greenLED, int redLED, int servoPin, int buzzerPin); // Custom pins, default code
		void start(int* correctCode, int codeLength, int button1, int button2, int button3,
			int lockButton, int greenLED, int redLED, int servoPin, int buzzerPin); // All custom
			
			// Lock Actions (original names)
			void DoorUnlock();
			void DoorLock();
			void open();
			void close();
			
			// Code Entry and Verification (original names)
			void DoorIncorrect();
			void resetAttempt();
			bool isAttemptCorrect();

    // Configuration Setters (original names)
    void setCorrectCode(int* code);
    void setPins(int* pins); // Assumes pins array has a fixed order of all pins

    // Button press handlers (original names)
    void button1Pressed();
    void button2Pressed();
    void button3Pressed();

    // Button status checks (original names)
    bool isButton1Pressed();
    bool isButton2Pressed();
    bool isButton3Pressed();
    bool isLockButtonPressed();

    // LED Control (original names)
    void redLEDToggle(bool state);
    void greenLEDToggle(bool state);

    // Buzzer Control (original names)
    void buzzerOn(int hz);
    void buzzerOff();

    // Getter methods (original names, forwarding to internal getters)
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
