#pragma once

#include "config.h"
#include <Arduino.h>
#include <cstdint>

class PIRManager {
public:
    PIRManager(gpio_num_t pirPin, gpio_num_t backlightPin);
    void begin();
    void update();
    bool isDisplayActive();
    void activateDisplay();
    void deactivateDisplay();

private:
    gpio_num_t _pirPin;
    gpio_num_t _backlightPin;
    bool displayActive;
    unsigned long lastMotionTime;
};
