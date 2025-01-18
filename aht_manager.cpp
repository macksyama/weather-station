#include "aht_manager.h"

AHT20Manager::AHT20Manager(Adafruit_AHTX0& aht, gpio_num_t pin) 
    : sensor(aht), power_pin(pin) {}

bool AHT20Manager::powerOn() {
    gpio_hold_dis(power_pin);
    pinMode(power_pin, OUTPUT);
    digitalWrite(power_pin, HIGH);
    delay(50);  // センサー安定化待ち
    return true;
}

bool AHT20Manager::initialize() {
    int retry = 5;
    while (retry--) {
        if (sensor.begin()) {
            Serial.println("AHT20の初期化完了");
            return true;
        }
        Serial.println("AHT20の初期化失敗リトライ中");
        delay(1000);
    }
    return false;
}

void AHT20Manager::powerOff() {
    digitalWrite(power_pin, LOW);
    gpio_hold_en(power_pin);
}

SensorData AHT20Manager::readData() {
    sensors_event_t humidity, temp;
    sensor.getEvent(&humidity, &temp);
    return {temp.temperature, humidity.relative_humidity};
}

