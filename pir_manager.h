#pragma once

#include "config.h"
#include "Arduino.h"

class PIRManager {
public:
    PIRManager(gpio_num_t pirPin);
    void begin();
    bool isMotionDetected();

private:
    const gpio_num_t _pirPin;
};
