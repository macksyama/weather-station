#include "pir_manager.h"

PIRManager::PIRManager(gpio_num_t pin) : _pirPin(pin) {}

void PIRManager::begin() {
    pinMode(_pirPin, INPUT_PULLDOWN);
}

bool PIRManager::isMotionDetected() {
    return digitalRead(_pirPin) == HIGH;
}
