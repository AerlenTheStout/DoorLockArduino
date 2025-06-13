#include "DoorLock.h" // Include the header for our library
#include <Arduino.h>        // Include Arduino core functions

// --- Global Single Instance of the Internal Class ---
// This is the one and only _DoorLockImpl object that will be created.
// It's declared here, in the .cpp file, so it's not directly accessible
// from user sketches, enforcing the single instance pattern.
_DoorLockImpl _theDoorLockInstance; // Default constructor is called automatically

// --- Implementation of _DoorLockImpl Class Methods ---

// Private Default Constructor: Delegates to the full constructor with default values.
_DoorLockImpl::_DoorLockImpl()
    : _DoorLockImpl(DOORLOCK_DEFAULT_CODE, DOORLOCK_DEFAULT_CODE_LENGTH, true, // Default to locked
                    DOORLOCK_BUTTON1_PIN, DOORLOCK_BUTTON2_PIN, DOORLOCK_BUTTON3_PIN, DOORLOCK_LOCK_BUTTON_PIN,
                    DOORLOCK_GREEN_LED_PIN, DOORLOCK_RED_LED_PIN, DOORLOCK_SERVO_PIN, DOORLOCK_BUZZER_PIN)
{
    // Constructor delegation handles the initialization.
}

// Private Full Constructor: Initializes all member variables and dynamically allocated arrays.
_DoorLockImpl::_DoorLockImpl(int* correctCode, int codeLength, bool Locked, int button1, int button2, int button3, int lockButton, int greenLED, int redLED, int servoPin, int buzzerPin)
    : _codeLength(codeLength), _button1(button1), _button2(button2), _button3(button3), _lockButton(lockButton), _greenLED(greenLED), _redLED(redLED), _servoPin(servoPin), _buzzerPin(buzzerPin),_locked(Locked) // Initialize locked state
{
    // Allocate memory for the correct secret code and copy it.
    // WARNING: This assumes `correctCode` is dynamically allocated by the caller or exists globally.
    // If `correctCode` is a local array in the caller, it will go out of scope and become a dangling pointer.
    // For safety, you might want to copy the contents, not just the pointer.
    _correctCode = new int[_codeLength];
    for (int i = 0; i < _codeLength; i++) {
        _correctCode[i] = correctCode[i];
    }

    // Allocate memory for the current attempt and initialize to 0.
    _attempt = new int[_codeLength];
    for (int i = 0; i < _codeLength; i++) {
        _attempt[i] = 0;
    }

    // Allocate memory for button debouncing states (4 buttons: 1, 2, 3, Lock)
    _lastReading = new int[4];
    _stableState = new int[4];

    // Initialize debounce states to HIGH (assuming INPUT_PULLUP and buttons pull LOW when pressed)
    for (int i = 0; i < 4; i++) {
        _lastReading[i] = HIGH;
        _stableState[i] = HIGH;
    }
    _inputIndex = 0; // Ensure input index is reset
}

// Destructor: Frees dynamically allocated memory to prevent memory leaks.
_DoorLockImpl::~_DoorLockImpl()
{
    delete[] _correctCode;
    delete[] _attempt;
    delete[] _lastReading;
    delete[] _stableState;
}

// Original `start()` method: Initializes hardware pins and sets initial state.
void _DoorLockImpl::start()
{
    // Start serial communication (optional, but good for debugging)
    Serial.begin(9600);
    Serial.println("DoorLock library initialized.");

    // Set pin modes for buttons (original had INPUT, generally INPUT_PULLUP is safer for physical buttons)
    // If you explicitly use external pull-down resistors, keep INPUT.
    pinMode(_button1, INPUT_PULLUP);
    pinMode(_button2, INPUT_PULLUP);
    pinMode(_button3, INPUT_PULLUP);
    pinMode(_lockButton, INPUT_PULLUP);

    // Set pin modes for LEDs (output)
    pinMode(_redLED, OUTPUT);
    pinMode(_greenLED, OUTPUT);
    // Buzzer pin as output for tone() function
    pinMode(_buzzerPin, OUTPUT);

    // Attach the servo to its pin
    _servo.attach(_servoPin);

    // Set initial states (consistent with original logic where lock() is called separately)
    digitalWrite(_redLED, LOW);
    digitalWrite(_greenLED, LOW);
    noTone(_buzzerPin);
    _servo.write(0); // Ensure servo is at initial position (locked)
    resetAttempt(); // Clear any previous attempt
}

// --- Lock Control Functions (Original Names) ---
void _DoorLockImpl::DoorUnlock()
{
    _locked = false;
    _servo.write(180); // Adjust servo position for unlocked state (e.g., 180 degrees)
    digitalWrite(_greenLED, HIGH);
    delay(1000); // Original delay for green LED
    digitalWrite(_greenLED, LOW);
    resetAttempt(); // Original behavior
    Serial.println("Door unlocked.");
}

// Renamed due to `lock` being a reserved word or common function name in global scope
// `void_lock` is just an internal name. The namespace function `DoorLock::lock()` will call this.
void _DoorLockImpl::DoorLock()
{
    _locked = true;
    _servo.write(0); // Adjust servo position for locked state (e.g., 0 degrees)
    digitalWrite(_redLED, HIGH);
    delay(1000); // Original delay for red LED
    digitalWrite(_redLED, LOW);
    resetAttempt(); // Original behavior
    Serial.println("Door locked.");
}

void _DoorLockImpl::open() // Original `open()`
{
    _servo.write(180); // Corresponds to unlock
}

void _DoorLockImpl::close() // Original `close()`
{
    _servo.write(0); // Corresponds to lock
}

// --- Code Entry and Verification Functions (Original Names) ---
void _DoorLockImpl::DoorIncorrect()
{
    digitalWrite(_redLED, HIGH); // Original behavior
    delay(1000);
    digitalWrite(_redLED, LOW);
    resetAttempt(); // Original behavior
    Serial.println("Incorrect code.");
}

void _DoorLockImpl::resetAttempt()
{
    for (int i = 0; i < _codeLength; i++) {
        _attempt[i] = 0; // Clear the attempt array
    }
    _inputIndex = 0;
    Serial.println("Attempt reset.");
}

bool _DoorLockImpl::isAttemptCorrect()
{
    if (_inputIndex != _codeLength) { // Check if the correct number of digits were entered
        Serial.println("Attempt length mismatch.");
        return false;
    }
    for (int i = 0; i < _codeLength; i++) {
        if (_attempt[i] != _correctCode[i]) {
            return false;
        }
    }
    return true;
}

// --- Configuration Setters (Original Names) ---
void _DoorLockImpl::setCorrectCode(int* code)
{
    // Copies the contents of the new code array.
    // This is safer than just assigning the pointer.
    for (int i = 0; i < _codeLength; i++) {
        _correctCode[i] = code[i];
    }
    Serial.println("Secret code updated.");
}

void _DoorLockImpl::setPins(int* pins)
{
    // Assumes `pins` array has a specific order:
    // {button1, button2, button3, lockButton, greenLED, redLED, servoPin, buzzerPin}
    if (pins) { // Basic check to ensure pins is not null
        _button1 = pins[0];
        _button2 = pins[1];
        _button3 = pins[2];
        _lockButton = pins[3];
        _greenLED = pins[4];
        _redLED = pins[5];
        _servoPin = pins[6];
        _buzzerPin = pins[7];

        // Re-initialize pin modes for the newly assigned pins
        pinMode(_button1, INPUT_PULLUP); // Using PULLUP for safety
        pinMode(_button2, INPUT_PULLUP);
        pinMode(_button3, INPUT_PULLUP);
        pinMode(_lockButton, INPUT_PULLUP);
        pinMode(_redLED, OUTPUT);
        pinMode(_greenLED, OUTPUT);
        pinMode(_buzzerPin, OUTPUT);
        _servo.attach(_servoPin); // Re-attach servo to the new pin
        Serial.println("Pin assignments updated.");
    }
}

// --- Button Press Handlers (Original Names) ---
void _DoorLockImpl::button1Pressed()
{
    Serial.println("button 1 pressed");
    if (_inputIndex < _codeLength) {
        _attempt[_inputIndex] = 1;
        _inputIndex++;
        for (int i = 0; i < _codeLength; i++) {
            Serial.print(_attempt[i]);
            Serial.print(",");
        }
        Serial.println();
    } else {
        Serial.println("Input index out of bounds, resetting attempt.");
        resetAttempt(); // Reset if too many digits entered
        // The original logic didn't add the current digit after reset,
        // so it might be lost. Consider if you want to add it here.
    }
    delay(500); // Original delay for button press
}

void _DoorLockImpl::button2Pressed()
{
    Serial.println("button 2 pressed");
    if (_inputIndex < _codeLength) {
        _attempt[_inputIndex] = 2;
        _inputIndex++;
        for (int i = 0; i < _codeLength; i++) {
            Serial.print(_attempt[i]);
            Serial.print(",");
        }
        Serial.println();
    } else {
        Serial.println("Input index out of bounds, resetting attempt.");
        resetAttempt();
    }
    delay(500); // Original delay
}

void _DoorLockImpl::button3Pressed()
{
    Serial.println("button 3 pressed");
    if (_inputIndex < _codeLength) {
        _attempt[_inputIndex] = 3;
        _inputIndex++;
        for (int i = 0; i < _codeLength; i++) {
            Serial.print(_attempt[i]);
            Serial.print(",");
        }
        Serial.println();
    } else {
        Serial.println("Input index out of bounds, resetting attempt.");
        resetAttempt();
    }
    delay(500); // Original delay
}

// --- Button Status Checks (Original Names) ---
// This uses the scanButtons for debouncing before returning the state
bool _DoorLockImpl::isButton1Pressed()
{
    scanButtons(); // Update debounced states
    return _stableState[0] == LOW; // Assuming pull-up, so LOW is pressed
}

bool _DoorLockImpl::isButton2Pressed()
{
    scanButtons();
    return _stableState[1] == LOW;
}

bool _DoorLockImpl::isButton3Pressed()
{
    scanButtons();
    return _stableState[2] == LOW;
}

bool _DoorLockImpl::isLockButtonPressed()
{
    scanButtons();
    return _stableState[3] == LOW;
}

// --- LED Control (Original Names) ---
void _DoorLockImpl::redLEDToggle(bool state)
{
    digitalWrite(_redLED, state ? HIGH : LOW);
}

void _DoorLockImpl::greenLEDToggle(bool state)
{
    digitalWrite(_greenLED, state ? HIGH : LOW);
}

// --- Buzzer Control (Original Names) ---
void _DoorLockImpl::buzzerOn(int hz)
{
    tone(_buzzerPin, hz);
}

void _DoorLockImpl::buzzerOff()
{
    noTone(_buzzerPin);
}

// --- Internal Debouncing Logic (Original Name) ---
void _DoorLockImpl::scanButtons()
{
    int buttonPins[] = {_button1, _button2, _button3, _lockButton};
    // Debounce logic from your original (or a common non-blocking pattern if preferred)
    // For simplicity with original structure, we'll re-implement the basic debounce.
    const unsigned long DEBOUNCE_DELAY = 50; // milliseconds

    for (int i = 0; i < 4; i++) {
        int reading = digitalRead(buttonPins[i]);

        // If the reading has changed from the last time
        if (reading != _lastReading[i]) {
            // Start a timer (or, for simplicity in this synchronous loop, just re-check after delay)
            // A truly non-blocking debounce would track millis() here.
            // For this direct copy, we'll mimic the original intent with a small delay.
            delay(5); // Small delay to let signal stabilize
            if (reading == digitalRead(buttonPins[i])) { // Check again after delay
                _stableState[i] = reading;
            }
        }
        _lastReading[i] = reading; // Save the current reading for the next loop
    }
}


// --- Implementation of Global Functions in DoorLock Namespace ---
// These functions are what campers will call directly from their sketch.
// Each function simply forwards the call to the single '_theDoorLockInstance'.

namespace DoorLock {
    // Overloaded start() functions for various initialization options
    void start() {
        _theDoorLockInstance.start();
    }
    void start(int* correctCode, int codeLength) {
        // Creates a temporary pins array with default values
        int pins[] = {DOORLOCK_BUTTON1_PIN, DOORLOCK_BUTTON2_PIN, DOORLOCK_BUTTON3_PIN,
                      DOORLOCK_LOCK_BUTTON_PIN, DOORLOCK_GREEN_LED_PIN, DOORLOCK_RED_LED_PIN,
                      DOORLOCK_SERVO_PIN, DOORLOCK_BUZZER_PIN};
        _theDoorLockInstance._DoorLockImpl::setPins(pins); // Use qualified call to avoid ambiguity
        _theDoorLockInstance.setCorrectCode(correctCode);
        _theDoorLockInstance.start();
    }
    void start(int button1, int button2, int button3, int lockButton,
               int greenLED, int redLED, int servoPin, int buzzerPin) {
        // Creates a temporary correctCode array with default values
        int code[] = {DOORLOCK_DEFAULT_CODE[0], DOORLOCK_DEFAULT_CODE[1], DOORLOCK_DEFAULT_CODE[2]};
        int codeLength = DOORLOCK_DEFAULT_CODE_LENGTH;

        int pins[] = {button1, button2, button3, lockButton, greenLED, redLED, servoPin, buzzerPin};
        _theDoorLockInstance.setCorrectCode(code); // Set default code
        _theDoorLockInstance._DoorLockImpl::setPins(pins); // Set custom pins
        _theDoorLockInstance.start();
    }
    void start(int* correctCode, int codeLength, int button1, int button2, int button3,
               int lockButton, int greenLED, int redLED, int servoPin, int buzzerPin) {
        int pins[] = {button1, button2, button3, lockButton, greenLED, redLED, servoPin, buzzerPin};
        _theDoorLockInstance.setCorrectCode(correctCode);
        _theDoorLockInstance._DoorLockImpl::setPins(pins);
        _theDoorLockInstance.start();
    }

    // Lock Actions
    void unlock() {
        _theDoorLockInstance.DoorUnlock();
    }
    void lock() {
        _theDoorLockInstance.DoorLock(); // Calls the internally renamed function
    }
    void open() {
        _theDoorLockInstance.open();
    }
    void close() {
        _theDoorLockInstance.close();
    }

    // Code Entry and Verification
    void Incorrect() {
        _theDoorLockInstance.DoorIncorrect();
    }
    void resetAttempt() {
        _theDoorLockInstance.resetAttempt();
    }
    bool isAttemptCorrect() {
        return _theDoorLockInstance.isAttemptCorrect();
    }

    // Configuration Setters
    void setCorrectCode(int* code) {
        _theDoorLockInstance.setCorrectCode(code);
    }
    void setPins(int* pins) {
        _theDoorLockInstance.setPins(pins);
    }

    // Button Press Handlers
    void button1Pressed() {
        _theDoorLockInstance.button1Pressed();
    }
    void button2Pressed() {
        _theDoorLockInstance.button2Pressed();
    }
    void button3Pressed() {
        _theDoorLockInstance.button3Pressed();
    }

    // Button Status Checks
    bool isButton1Pressed() {
        return _theDoorLockInstance.isButton1Pressed();
    }
    bool isButton2Pressed() {
        return _theDoorLockInstance.isButton2Pressed();
    }
    bool isButton3Pressed() {
        return _theDoorLockInstance.isButton3Pressed();
    }
    bool isLockButtonPressed() {
        return _theDoorLockInstance.isLockButtonPressed();
    }

    // LED Control
    void redLEDToggle(bool state) {
        _theDoorLockInstance.redLEDToggle(state);
    }
    void greenLEDToggle(bool state) {
        _theDoorLockInstance.greenLEDToggle(state);
    }

    // Buzzer Control
    void buzzerOn(int hz) {
        _theDoorLockInstance.buzzerOn(hz);
    }
    void buzzerOff() {
        _theDoorLockInstance.buzzerOff();
    }

    // Getter methods (forwarding to internal getters)
    int getButton1() { return _theDoorLockInstance.getButton1(); }
    int getButton2() { return _theDoorLockInstance.getButton2(); }
    int getButton3() { return _theDoorLockInstance.getButton3(); }
    int getLockButton() { return _theDoorLockInstance.getLockButton(); }
    int getGreenLED() { return _theDoorLockInstance.getGreenLED(); }
    int getRedLED() { return _theDoorLockInstance.getRedLED(); }
    int getServoPin() { return _theDoorLockInstance.getServoPin(); }
    int getBuzzerPin() { return _theDoorLockInstance.getBuzzerPin(); }

} // end namespace DoorLock
