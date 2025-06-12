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
    bool _isLocked = true; // Current locked/unlocked state of the door

    // Pin assignments for hardware components
    int _button1Pin;
    int _button2Pin;
    int _button3Pin;
    int _lockButtonPin;
    int _redLEDPin;
    int _greenLEDPin;
    int _servoPin;
    int _buzzerPin;

    // Variables for button debouncing
    // These store the last raw reading and the stable (debounced) state of buttons
    int* _lastButtonReading; // Array to store last reading for each button
    int* _stableButtonState; // Array to store stable state for each button

    Servo _doorServo; // Servo object to control the lock mechanism

    // Private constructor to prevent direct instantiation by users.
    // This ensures only the library creates the single instance.
    // It handles initializing all pins and code.
    _DoorLockImpl(const int* code, int codeLength,
                  int button1, int button2, int button3, int lockButton,
                  int greenLED, int redLED, int servoPin, int buzzerPin);

    // Default private constructor, delegates to the full constructor with defaults
    _DoorLockImpl();

    // Private helper method for debouncing all buttons
    void _updateButtonStates();

    // Private helper method to get the debounced state of a specific button pin
    // This abstracts the indexing for the user-facing methods.
    bool _isButtonPinDown(int pinArrayIndex);

public:
    // Public destructor to properly clean up dynamically allocated memory
    ~_DoorLockImpl();

    // --- Core Public Methods (called by the global namespace functions) ---

    // Initializes all hardware pins and sets initial lock state
    void begin();

    // Actions to control the door lock mechanism
    void unlockDoor();
    void lockDoor();
    bool isDoorLocked(); // Checks if the door is currently locked

    // Functions for entering and checking the code
    void enterDigit(int digit); // Records a digit entered by the user
    void resetCodeEntry();      // Clears the current code attempt
    bool checkEnteredCode();    // Verifies if the entered code matches the secret code

    // Functions for LED feedback
    void turnRedLEDOn();
    void turnRedLEDOff();
    void turnGreenLEDOn();
    void turnGreenLEDOff();

    // Functions for buzzer sound feedback
    void playSuccessSound();    // Plays a sound for correct code
    void playIncorrectSound();  // Plays a sound for incorrect code
    void stopSound();           // Stops any ongoing sound

    // Functions to directly check button states (debounced)
    // These are exposed if campers want to build custom button handling
    bool isButton1Down();
    bool isButton2Down();
    bool isButton3Down();
    bool isLockButtonDown();

    // Functions to update the secret code or pin assignments after initialization
    void setSecretCode(const int* newCode, int newCodeLength);
    void setPinAssignments(int button1, int button2, int button3, int lockButton,
                           int greenLED, int redLED, int servoPin, int buzzerPin);

    // Public member (consider making this private with a getter/setter if needed for campers)
    int* _attempt; // Stores the current code attempt
};

// --- Public-Facing Namespace for Campers ---
// This namespace provides the simple, direct function calls for campers.
// They will use these functions like `DoorLock::unlock()` or `DoorLock::input(1)`.
namespace DoorLock {
    // Initialization functions (call ONE of these in your setup() function)
    // 1. Simplest: Uses all default pins and the default secret code (1-2-3)
    void begin();
    // 2. Custom Code: Allows setting a different secret code, uses default pins
    void begin(const int* customCode, int customCodeLength);
    // 3. Custom Pins: Allows setting all pins, uses default secret code
    void begin(int button1, int button2, int button3, int lockButton,
               int greenLED, int redLED, int servoPin, int buzzerPin);
    // 4. Fully Custom: Allows setting both a custom code and custom pins
    void begin(const int* customCode, int customCodeLength,
               int button1, int button2, int button3, int lockButton,
               int greenLED, int redLED, int servoPin, int buzzerPin);

    // --- Actions for the Door Lock ---
    void unlock();          // Unlocks the door (moves servo to unlocked position)
    void lock();            // Locks the door (moves servo to locked position)
    bool isLocked();        // Returns true if the door is currently locked

    // --- Functions for Code Entry ---
    void input(int digit);  // Records a digit (1, 2, or 3) into the current attempt
    void resetCode();       // Clears the current entered code attempt
    bool checkCode();       // Checks if the current attempt matches the secret code

    // --- LED Control ---
    void turnRedLEDOn();    // Turns the red LED on
    void turnRedLEDOff();   // Turns the red LED off
    void turnGreenLEDOn();  // Turns the green LED on
    void turnGreenLEDOff(); // Turns the green LED off

    // --- Sound Feedback ---
    void playSuccessSound(); // Plays a sound for successful code entry
    void playFailSound();    // Plays a sound for incorrect code entry
    void stopSound();        // Stops any ongoing sound

    // --- Button Status Checks (debounced) ---
    // These functions return true if the corresponding button is currently pressed down.
    bool isButton1Down();
    bool button2IsPressed(); // Alternative naming for variety
    bool button3IsPressed();
    bool lockButtonIsPressed();
} // end namespace DoorLock

#endif // ARDUINO_DOORLOCK_H
