#include <SPI.h>

// Pin definitions for motor control and speed indication
#define BUTTON_A_PIN 2
#define BUTTON_B_PIN 3
#define MOTOR_PIN 5

#define LED_8_PIN A0
#define LED_9_PIN A1
#define LED_10_PIN A2
#define LED_11_PIN A3
#define LED_12_PIN A4

// Pin definitions for the shift register
#define SHIFT_DATA_PIN 8
#define SHIFT_CLOCK_PIN 9
#define SHIFT_LATCH_PIN 10

// Pin definitions for battery monitoring
#define BATTERY_PIN A5  // Analog pin to read battery voltage
#define LED_13_PIN 11   // LED for full charge
#define LED_14_PIN 12   // LED for half charge
#define LED_15_PIN 13   // LED for low charge

// Define voltage thresholds for a 3.7V Li-Ion battery
const float fullChargeVoltage = 2.1;
const float halfChargeVoltage = 1.85;
const float lowChargeVoltage = 1.6;

enum SetupState { SETUP_OFF, SETUP_ON };
enum SpeedState { NO_SPEED, FULL_SPEED, HALF_SPEED, LOW_SPEED, PULSE, MANUAL };

SetupState setupState = SETUP_OFF;
SpeedState speedState = NO_SPEED;
unsigned long lastButtonBPress = 0;
unsigned long lastPulseMillis = 0;
bool pulseState = false;
bool ledState = false;
unsigned long lastLedBlinkMillis = 0;
const unsigned long blinkInterval = 500; // Blinking interval in milliseconds

// Debouncing variables
const unsigned long debounceDelay = 100; // Debounce delay in milliseconds
unsigned long lastDebounceTimeA = 0;
unsigned long lastDebounceTimeB = 0;
bool buttonAState = HIGH;
bool buttonBState = HIGH;
bool lastButtonAState = HIGH;
bool lastButtonBState = HIGH;

// Variables for manual mode hold duration
const unsigned long holdDuration = 1500; // 1.5 seconds
unsigned long buttonAHoldStartTime = 0;

void setup() {
    pinMode(BUTTON_A_PIN, INPUT_PULLUP);
    pinMode(BUTTON_B_PIN, INPUT_PULLUP);
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(LED_8_PIN, OUTPUT);
    pinMode(LED_9_PIN, OUTPUT);
    pinMode(LED_10_PIN, OUTPUT);
    pinMode(LED_11_PIN, OUTPUT);
    pinMode(LED_12_PIN, OUTPUT);

    // Initialize shift register pins
    pinMode(SHIFT_DATA_PIN, OUTPUT);
    pinMode(SHIFT_CLOCK_PIN, OUTPUT);
    pinMode(SHIFT_LATCH_PIN, OUTPUT);

    // Initialize battery monitoring LEDs
    pinMode(LED_13_PIN, OUTPUT);
    pinMode(LED_14_PIN, OUTPUT);
    pinMode(LED_15_PIN, OUTPUT);

    // Initialize serial communication for debugging
    Serial.begin(9600);
}

void loop() {
    unsigned long currentMillis = millis();

    bool readingA = digitalRead(BUTTON_A_PIN);
    bool readingB = digitalRead(BUTTON_B_PIN);

    // Debounce Button A
    if (readingA != lastButtonAState) {
        lastDebounceTimeA = currentMillis;
    }

    if ((currentMillis - lastDebounceTimeA) > debounceDelay) {
        if (readingA != buttonAState) {
            buttonAState = readingA;
            if (buttonAState == LOW) {
                if (setupState == SETUP_ON && speedState == MANUAL) {
                    buttonAHoldStartTime = currentMillis;
                } else {
                    toggleSetup();
                }
            }
        }
    }

    // Debounce Button B
    if (readingB != lastButtonBState) {
        lastDebounceTimeB = currentMillis;
    }

    if ((currentMillis - lastDebounceTimeB) > debounceDelay) {
        if (readingB != buttonBState) {
            buttonBState = readingB;
            if (buttonBState == LOW) {
                handleButtonBPress(currentMillis);
            }
        }
    }

    lastButtonAState = readingA;
    lastButtonBState = readingB;

    if (setupState == SETUP_ON) {
        if (speedState == MANUAL && buttonAState == LOW && (currentMillis - buttonAHoldStartTime) >= holdDuration) {
            // Run motor at full speed when Button A is held down in manual mode
            analogWrite(MOTOR_PIN, 255*0.7);
        } else if (speedState != NO_SPEED) {
            runMotor(currentMillis);
            updateSpeedLEDs(currentMillis);
        } else {
            stopMotor();
            resetSpeedLEDs();
        }

        monitorBattery();
    } else {
        stopMotor();
        resetSpeedLEDs();
        resetBatteryLEDs();
    }

    delay(50);
}

void toggleSetup() {
    setupState = (setupState == SETUP_OFF) ? SETUP_ON : SETUP_OFF;
    Serial.println(setupState == SETUP_ON ? "Setup ON" : "Setup OFF");
    if (setupState == SETUP_OFF) {
        speedState = NO_SPEED;
        resetSpeedLEDs();
        resetBatteryLEDs();
    }
}

void handleButtonBPress(unsigned long currentMillis) {
    if (currentMillis - lastButtonBPress >= 500) {
        if (speedState == NO_SPEED) {
            speedState = FULL_SPEED;
        } else {
            speedState = static_cast<SpeedState>((speedState + 1) % 6); // Cycle through the speed states
        }
        lastButtonBPress = currentMillis;

        switch (speedState) {
            case FULL_SPEED:
                Serial.println("Full Speed Selected");
                break;
            case HALF_SPEED:
                Serial.println("Half Speed Selected");
                break;
            case LOW_SPEED:
                Serial.println("Low Speed Selected");
                break;
            case PULSE:
                Serial.println("Pulse Selected");
                break;
            case MANUAL:
                Serial.println("Manual Mode Selected");
                break;
            case NO_SPEED:
                Serial.println("No Speed Selected");
                break;
        }
        updateLEDIndicators();
    }
}

void runMotor(unsigned long currentMillis) {
    switch (speedState) {
        case FULL_SPEED:
            analogWrite(MOTOR_PIN, 255*0.75);
            break;
        case HALF_SPEED:
            analogWrite(MOTOR_PIN, 128);
            break;
        case LOW_SPEED:
            analogWrite(MOTOR_PIN, 64*1.25);
            break;
        case PULSE:
            if (currentMillis - lastPulseMillis >= 1000) {
                lastPulseMillis = currentMillis;
                pulseState = !pulseState;
            }
            analogWrite(MOTOR_PIN, pulseState ? 255 : 0);
            break;
        case MANUAL:
            analogWrite(MOTOR_PIN, 0); // Manual mode motor should not run unless Button A is held down for more than 1.5 seconds
            break;
        case NO_SPEED:
            // Do nothing
            break;
    }
}

void stopMotor() {
    analogWrite(MOTOR_PIN, 0);
}

void updateLEDIndicators() {
    digitalWrite(LED_8_PIN, speedState == FULL_SPEED ? HIGH : LOW);
    digitalWrite(LED_9_PIN, speedState == HALF_SPEED ? HIGH : LOW);
    digitalWrite(LED_10_PIN, speedState == LOW_SPEED ? HIGH : LOW);
    digitalWrite(LED_11_PIN, speedState == PULSE ? HIGH : LOW);
    digitalWrite(LED_12_PIN, speedState == MANUAL ? HIGH : LOW);
}

void updateSpeedLEDs(unsigned long currentMillis) {
    if (currentMillis - lastLedBlinkMillis >= blinkInterval) {
        lastLedBlinkMillis = currentMillis;
        ledState = !ledState;
    }

    uint8_t pattern = 0;
    switch (speedState) {
        case FULL_SPEED:
            pattern = ledState ? 0b00111111 : 0b00000000; // LEDs 1-6 blinking, LED7 off
            break;
        case HALF_SPEED:
            pattern = ledState ? 0b00111000 : 0b00000000; // LEDs 4-6 blinking, LEDs 1, 2, 3, 7 off
            break;
        case LOW_SPEED:
            pattern = ledState ? 0b00100000 : 0b00000000; // LED 6 blinking, LEDs 1, 2, 3, 4, 5, 7 off
            break;
        case PULSE:
            pattern = 0b00000000; // All LEDs off
            break;
        case MANUAL:
            pattern = ledState ? 0b01000000 : 0b00000000; // LED 7 blinking, LEDs 1-6 off
            break;
        case NO_SPEED:
            pattern = 0b00000000; // All LEDs off
            break;
    }
    updateShiftRegister(pattern);
}

void resetSpeedLEDs() {
    updateShiftRegister(0b00000000);
}

void updateShiftRegister(uint8_t pattern) {
    digitalWrite(SHIFT_LATCH_PIN, LOW);
    shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, MSBFIRST, pattern);
    digitalWrite(SHIFT_LATCH_PIN, HIGH);
}

void monitorBattery() {
    float batteryVoltage = analogRead(BATTERY_PIN) * (5.0 / 1023.0) * 2; // Calculate battery voltage

      if (batteryVoltage >= fullChargeVoltage) {
        // Full charge: All 3 LEDs on
        digitalWrite(LED_13_PIN, HIGH);
        digitalWrite(LED_14_PIN, HIGH);
        digitalWrite(LED_15_PIN, HIGH);
    } else if (batteryVoltage >= halfChargeVoltage) {
        // Half charge: 2 LEDs on
        digitalWrite(LED_13_PIN, HIGH);
        digitalWrite(LED_14_PIN, HIGH);
        digitalWrite(LED_15_PIN, LOW);
    } else if (batteryVoltage <= lowChargeVoltage) {
        // Low charge: 1 LED on
        digitalWrite(LED_13_PIN, HIGH);
        digitalWrite(LED_14_PIN, LOW);
        digitalWrite(LED_15_PIN, LOW);
    } else {
        // Battery needs charging: All 3 LEDs flashing
        unsigned long currentMillis = millis();
        if (currentMillis - lastLedBlinkMillis >= blinkInterval) {
            lastLedBlinkMillis = currentMillis;
            ledState = !ledState;
            digitalWrite(LED_13_PIN, ledState ? HIGH : LOW);
            digitalWrite(LED_14_PIN, ledState ? HIGH : LOW);
            digitalWrite(LED_15_PIN, ledState ? HIGH : LOW);
        }
    }
}

void resetBatteryLEDs() {
    digitalWrite(LED_13_PIN, LOW);
    digitalWrite(LED_14_PIN, LOW);
    digitalWrite(LED_15_PIN, LOW);
}
