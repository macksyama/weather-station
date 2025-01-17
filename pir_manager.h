#pragma once

#include "config.h"
#include <Arduino.h>
#include <cstdint>

class PIRManager {
public:
    PIRManager(uint8_t pirPin, uint8_t backlightPin);
    void begin();
    void update();
    bool isDisplayActive() const { return displayActive; }

private:
    const uint8_t _pirPin;
    const uint8_t _backlightPin;
    bool displayActive;
    unsigned long lastMotionTime;
    
    void activateDisplay();
    void deactivateDisplay();
};
