#include "pir_manager.h"

PIRManager::PIRManager(gpio_num_t pirPin, gpio_num_t backlightPin)
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
    gpio_hold_dis(_backlightPin);
    digitalWrite(_backlightPin, HIGH);
    displayActive = true;
}

void PIRManager::deactivateDisplay() {
    Serial.println("タイムアウトによりディスプレイをオフにします");
    digitalWrite(_backlightPin, LOW);
    gpio_hold_en(_backlightPin);
    displayActive = false;
}
