#include "DoorLock.h" // Include the header for our library
#include <Arduino.h>        // Include Arduino core functions

// --- Global Single Instance of the Internal Class ---
// This is the one and only DoorLockImpl object that will be created.
// It's declared here, in the .cpp file, so it's not directly accessible
// from user sketches, enforcing the single instance pattern.
_DoorLockImpl _theDoorLockInstance; // Default constructor is called automatically

// --- Implementation of _DoorLockImpl Class Methods ---

// Private Default Constructor: Delegates to the full constructor with default values.
_DoorLockImpl::_DoorLockImpl()
    : _DoorLockImpl(DOORLOCK_DEFAULT_CODE, DOORLOCK_DEFAULT_CODE_LENGTH,
                    DOORLOCK_BUTTON1_PIN, DOORLOCK_BUTTON2_PIN, DOORLOCK_BUTTON3_PIN, DOORLOCK_LOCK_BUTTON_PIN,
                    DOORLOCK_GREEN_LED_PIN, DOORLOCK_RED_LED_PIN, DOORLOCK_SERVO_PIN, DOORLOCK_BUZZER_PIN)
{
    // Constructor delegation handles the initialization.
}

// Private Full Constructor: Initializes all member variables and dynamically allocated arrays.
_DoorLockImpl::_DoorLockImpl(const int* code, int codeLength,
                           int button1, int button2, int button3, int lockButton,
                           int greenLED, int redLED, int servoPin, int buzzerPin)
    : _codeLength(codeLength),
      _button1Pin(button1), _button2Pin(button2), _button3Pin(button3), _lockButtonPin(lockButton),
      _greenLEDPin(greenLED), _redLEDPin(redLED), _servoPin(servoPin), _buzzerPin(buzzerPin)
{
    // Allocate memory for the correct secret code and copy it.
    _correctCode = new int[_codeLength];
    for (int i = 0; i < _codeLength; ++i) {
        _correctCode[i] = code[i];
    }

    // Allocate memory for the current attempt and initialize to 0.
    _attempt = new int[_codeLength];
    for (int i = 0; i < _codeLength; ++i) {
        _attempt[i] = 0;
    }

    // Allocate memory for button debouncing states (4 buttons: 1, 2, 3, Lock)
    _lastButtonReading = new int[4];
    _stableButtonState = new int[4];

    // Initialize debounce states to HIGH (assuming INPUT_PULLUP and buttons pull LOW when pressed)
    for (int i = 0; i < 4; ++i) {
        _lastButtonReading[i] = HIGH;
        _stableButtonState[i] = HIGH;
    }
    _inputIndex = 0; // Ensure input index is reset
    _isLocked = true; // Default to locked state
}

// Destructor: Frees dynamically allocated memory to prevent memory leaks.
_DoorLockImpl::~_DoorLockImpl()
{
    delete[] _correctCode;
    delete[] _attempt;
    delete[] _lastButtonReading;
    delete[] _stableButtonState;
}

// Initializes hardware pins and sets initial state.
void _DoorLockImpl::begin()
{
    // Set pin modes for buttons (with internal pull-up resistors for simplicity)
    pinMode(_button1Pin, INPUT_PULLUP);
    pinMode(_button2Pin, INPUT_PULLUP);
    pinMode(_button3Pin, INPUT_PULLUP);
    pinMode(_lockButtonPin, INPUT_PULLUP);

    // Set pin modes for LEDs (output)
    pinMode(_redLEDPin, OUTPUT);
    pinMode(_greenLEDPin, OUTPUT);
    // Buzzer pin as output for tone() function
    pinMode(_buzzerPin, OUTPUT);

    // Attach the servo to its pin
    _doorServo.attach(_servoPin);

    // Set initial states
    turnRedLEDOff();
    turnGreenLEDOff();
    stopSound();
    lockDoor(); // Ensure the door is locked at setup
    resetCodeEntry(); // Clear any previous attempt
    Serial.begin(9600); // Start serial communication
    Serial.println("DoorLock library initialized.");
}

// --- Lock Control Functions ---
void _DoorLockImpl::unlockDoor()
{
    Serial.println("Unlocking door...");
    _doorServo.write(180); // Adjust servo position for unlocked state (e.g., 180 degrees)
    _isLocked = false;
    turnGreenLEDOn();
    turnRedLEDOff();
}

void _DoorLockImpl::lockDoor()
{
    Serial.println("Locking door...");
    _doorServo.write(0); // Adjust servo position for locked state (e.g., 0 degrees)
    _isLocked = true;
    turnGreenLEDOff();
    turnRedLEDOn();
}

bool _DoorLockImpl::isDoorLocked()
{
    return _isLocked;
}

// --- Code Entry Functions ---
void _DoorLockImpl::enterDigit(int digit)
{
    if (_inputIndex < _codeLength) {
        _attempt[_inputIndex++] = digit;
        Serial.print("Input: ");
        for (int i = 0; i < _inputIndex; ++i) {
            Serial.print(_attempt[i]);
        }
        Serial.println();
    } else {
        Serial.println("Code attempt buffer full. Resetting.");
        resetCodeEntry(); // Automatically reset if too many digits entered
        _attempt[_inputIndex++] = digit; // Add the current digit to the new attempt
    }
}

void _DoorLockImpl::resetCodeEntry()
{
    _inputIndex = 0;
    for (int i = 0; i < _codeLength; ++i) {
        _attempt[i] = 0; // Clear the attempt array
    }
    Serial.println("Code attempt reset.");
}

bool _DoorLockImpl::checkEnteredCode()
{
    if (_inputIndex != _codeLength) {
        Serial.println("Incorrect: Code length mismatch.");
        return false; // Not enough digits entered
    }
    for (int i = 0; i < _codeLength; ++i) {
        if (_attempt[i] != _correctCode[i]) {
            Serial.println("Incorrect: Mismatched digit.");
            return false;
        }
    }
    Serial.println("Code correct!");
    return true;
}

// --- LED Control Functions ---
void _DoorLockImpl::turnRedLEDOn()
{
    digitalWrite(_redLEDPin, HIGH);
}

void _DoorLockImpl::turnRedLEDOff()
{
    digitalWrite(_redLEDPin, LOW);
}

void _DoorLockImpl::turnGreenLEDOn()
{
    digitalWrite(_greenLEDPin, HIGH);
}

void _DoorLockImpl::turnGreenLEDOff()
{
    digitalWrite(_greenLEDPin, LOW);
}

// --- Sound Feedback Functions ---
void _DoorLockImpl::playSuccessSound()
{
    tone(_buzzerPin, 1500, 100); // High pitch, short duration
}

void _DoorLockImpl::playIncorrectSound()
{
    tone(_buzzerPin, 500, 500); // Low pitch, longer duration
}

void _DoorLockImpl::stopSound()
{
    noTone(_buzzerPin);
}

// --- Internal Button Debouncing Logic ---
void _DoorLockImpl::_updateButtonStates()
{
    // Map button indices to their actual pin numbers
    int buttonPins[] = {_button1Pin, _button2Pin, _button3Pin, _lockButtonPin};
    const unsigned long DEBOUNCE_DELAY = 50; // Milliseconds

    for (int i = 0; i < 4; ++i) {
        int currentReading = digitalRead(buttonPins[i]);

        // If the reading has changed, reset the last reading time for this button
        if (currentReading != _lastButtonReading[i]) {
            // No explicit timestamp needed if we use a simple delay.
            // For a non-blocking debounce, you'd use millis() and a timestamp array.
            // For this simple example, we'll use a blocking delay for simplicity,
            // but in a real-time system, a non-blocking debounce is preferred.
        }

        // If the reading has been stable for the debounce delay, update the stable state
        // This simple delay helps filter out noise.
        delay(5); // Small delay to allow the signal to stabilize a bit
        if (currentReading == digitalRead(buttonPins[i])) { // Check again after a small delay
            _stableButtonState[i] = currentReading;
        }
        _lastButtonReading[i] = currentReading; // Update last reading for next cycle
    }
}

// Helper functions to check debounced button state
bool _DoorLockImpl::_isButtonPinDown(int pinArrayIndex) {
    _updateButtonStates(); // Always update states before checking to get latest debounced reading
    return _stableButtonState[pinArrayIndex] == LOW; // Assuming pull-up resistors (button pressed = LOW)
}

// Public methods for checking specific button states (use _isButtonPinDown internally)
bool _DoorLockImpl::isButton1Down() { return _isButtonPinDown(0); }
bool _DoorLockImpl::isButton2Down() { return _isButtonPinDown(1); }
bool _DoorLockImpl::isButton3Down() { return _isButtonPinDown(2); }
bool _DoorLockImpl::isLockButtonDown() { return _isButtonPinDown(3); }


// --- Configuration Update Functions ---
void _DoorLockImpl::setSecretCode(const int* newCode, int newCodeLength) {
    if (newCodeLength != _codeLength) {
        delete[] _correctCode; // Free old memory
        delete[] _attempt;     // Free old attempt buffer
        _codeLength = newCodeLength;
        _correctCode = new int[_codeLength]; // Allocate new memory for correct code
        _attempt = new int[_codeLength];     // Allocate new memory for attempt
    }
    // Copy the new code into the allocated memory
    for (int i = 0; i < _codeLength; ++i) {
        _correctCode[i] = newCode[i];
    }
    resetCodeEntry(); // Reset any ongoing attempt
    Serial.println("Secret code updated.");
}

void _DoorLockImpl::setPinAssignments(int button1, int button2, int button3, int lockButton,
                                    int greenLED, int redLED, int servoPin, int buzzerPin) {
    _button1Pin = button1;
    _button2Pin = button2;
    _button3Pin = button3;
    _lockButtonPin = lockButton;
    _greenLEDPin = greenLED;
    _redLEDPin = redLED;
    _servoPin = servoPin;
    _buzzerPin = buzzerPin;

    // Re-initialize pin modes for the newly assigned pins
    pinMode(_button1Pin, INPUT_PULLUP);
    pinMode(_button2Pin, INPUT_PULLUP);
    pinMode(_button3Pin, INPUT_PULLUP);
    pinMode(_lockButtonPin, INPUT_PULLUP);
    pinMode(_redLEDPin, OUTPUT);
    pinMode(_greenLEDPin, OUTPUT);
    pinMode(_buzzerPin, OUTPUT);
    _doorServo.attach(_servoPin); // Re-attach servo to the new pin
    Serial.println("Pin assignments updated.");
}


// --- Implementation of Global Functions in DoorLock Namespace ---
// These functions are what campers will call directly from their sketch.
// Each function simply forwards the call to the single '_theDoorLockInstance'.

namespace DoorLock {
    // Overloaded begin() functions for various initialization options
    void begin() {
        _theDoorLockInstance.begin();
    }
    void begin(const int* customCode, int customCodeLength) {
        _theDoorLockInstance.setSecretCode(customCode, customCodeLength);
        _theDoorLockInstance.begin();
    }
    void begin(int button1, int button2, int button3, int lockButton,
               int greenLED, int redLED, int servoPin, int buzzerPin) {
        _theDoorLockInstance.setPinAssignments(button1, button2, button3, lockButton,
                                              greenLED, redLED, servoPin, buzzerPin);
        _theDoorLockInstance.begin();
    }
    void begin(const int* customCode, int customCodeLength,
               int button1, int button2, int button3, int lockButton,
               int greenLED, int redLED, int servoPin, int buzzerPin) {
        _theDoorLockInstance.DoorLockInternal::setSecretCode(customCode, customCodeLength); // Use fully qualified name to avoid ambiguity if 'setCode' was also a global function.
        _theDoorLockInstance.setPinAssignments(button1, button2, button3, lockButton,
                                              greenLED, redLED, servoPin, buzzerPin);
        _theDoorLockInstance.begin();
    }

    // Lock Actions
    void unlock() {
        _theDoorLockInstance.unlockDoor();
    }
    void lock() {
        _theDoorLockInstance.lockDoor();
    }
    bool isLocked() {
        return _theDoorLockInstance.isDoorLocked();
    }

    // Code Entry
    void input(int digit) {
        _theDoorLockInstance.enterDigit(digit);
    }
    void resetCode() {
        _theDoorLockInstance.resetCodeEntry();
    }
    bool checkCode() {
        return _theDoorLockInstance.checkEnteredCode();
    }

    // LED Control
    void turnRedLEDOn() {
        _theDoorLockInstance.turnRedLEDOn();
    }
    void turnRedLEDOff() {
        _theDoorLockInstance.turnRedLEDOff();
    }
    void turnGreenLEDOn() {
        _theDoorLockInstance.turnGreenLEDOn();
    }
    void turnGreenLEDOff() {
        _theDoorLockInstance.turnGreenLEDOff();
    }

    // Sound Feedback
    void playSuccessSound() {
        _theDoorLockInstance.playSuccessSound();
    }
    void playFailSound() {
        _theDoorLockInstance.playIncorrectSound();
    }
    void stopSound() {
        _theDoorLockInstance.stopSound();
    }

    // Button Status Checks
    bool isButton1Down() {
        return _theDoorLockInstance.isButton1Down();
    }
    bool button2IsPressed() {
        return _theDoorLockInstance.isButton2Down();
    }
    bool button3IsPressed() {
        return _theDoorLockInstance.isButton3Down();
    }
    bool lockButtonIsPressed() {
        return _theDoorLockInstance.isLockButtonDown();
    }

    // Optional: Setters for code/pins after begin() if needed
    void setCode(const int* newCode, int newCodeLength) {
        _theDoorLockInstance.setSecretCode(newCode, newCodeLength);
    }

    void setPins(int button1, int button2, int button3, int lockButton,
                 int greenLED, int redLED, int servoPin, int buzzerPin) {
        _theDoorLockInstance.setPinAssignments(button1, button2, button3, lockButton,
                                              greenLED, redLED, servoPin, buzzerPin);
    }

} // end namespace DoorLock
