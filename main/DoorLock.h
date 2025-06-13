
#ifndef ARDUINO_DOORLOCK_H
#define ARDUINO_DOORLOCK_H

class DoorLock  
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

    // Initializes all hardware pins and sets initial lock state (original: start)
    void start();
    void scanButtons();

    // Lock Actions (original names)
    void DoorUnlock();
    void DoorLock(); // Renamed to avoid clash with namespace function 'lock'
    void open();      // Original name
    void close();     // Original name

    // Code entry and verification (original names)
    void DoorIncorrect(); // Original name, includes LED and reset
    void resetAttempt(); // Original name
    bool isAttemptCorrect(); // Original name

    // Functions to set configurations (original names)
    void setCorrectCode(int* code); // Updated to accept int*
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

    // Public member (original: int* attempt;)
    // int* _attempt; // This is now private and managed internally.
};


// --- Public-Facing Namespace for Campers ---
// This namespace provides the simple, direct function calls for campers.
// They will use these functions like `DoorLock::unlock()` or `DoorLock::button1Pressed()`.
namespace DoorLock {
	// Expose the locked state as a variable (not recommended, but per user request)
	extern bool locked;
	// Initialization function (original: start)
    // We'll create overloads for different initialization scenarios
    void start(); // Uses default pins and code
    void start(int* correctCode, int codeLength); // Custom code, default pins
    void start(int button1, int button2, int button3, int lockButton,
               int greenLED, int redLED, int servoPin, int buzzerPin); // Custom pins, default code
    void start(int* correctCode, int codeLength, int button1, int button2, int button3,
               int lockButton, int greenLED, int redLED, int servoPin, int buzzerPin); // All custom

	void scanButtons();

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
    void setCorrectCode(int* code); // Parameter type matches _DoorLockImpl
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
