#include "pir_manager.h"

PIRManager::PIRManager(uint8_t pirPin, uint8_t backlightPin)
    : _pirPin(pirPin)
    , _backlightPin(backlightPin)
    , displayActive(false)
    , lastMotionTime(0)
{
}

void PIRManager::begin() {
    pinMode(_pirPin, INPUT_PULLDOWN);
    pinMode(_backlightPin, OUTPUT);
    digitalWrite(_backlightPin, LOW);
}

void PIRManager::update() {
    if (digitalRead(_pirPin) == HIGH) {
        if (!displayActive) {
            activateDisplay();
        }
        lastMotionTime = millis();
    }

    if (displayActive && (millis() - lastMotionTime > DISPLAY_TIMEOUT)) {
        deactivateDisplay();
    }
}

void PIRManager::activateDisplay() {
    Serial.println("人を検知しました");
    digitalWrite(_backlightPin, HIGH);
    displayActive = true;
}

void PIRManager::deactivateDisplay() {
    Serial.println("タイムアウトによりディスプレイをオフにします");
    digitalWrite(_backlightPin, LOW);
    displayActive = false;
}
