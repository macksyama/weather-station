#pragma once
#include <Adafruit_ILI9341.h>
#include "config.h"
#include "image_data.h"

class DisplayManager {
public:
    DisplayManager(Adafruit_ILI9341& tft, gpio_num_t blPin);
    void begin();
    void turnOn();
    void turnOff();
    void drawStaticElements();
    void updateDisplay(float temp, float humidity, 
                      float outdoor_temp, float outdoor_humidity,
                      float outdoor_battery_voltage, uint32_t last_ble_received);
    bool isOn() const { return isDisplayOn; }
private:
    Adafruit_ILI9341& _tft;
    const gpio_num_t backlightPin;
    bool isDisplayOn;
    bool isFirstDraw;
    float prev_temp, prev_humidity, prev_di;
    float prev_outdoor_temp, prev_outdoor_humidity;
    float prev_outdoor_battery_voltage;
    uint8_t prev_temp_animation_offset;
    uint8_t prev_humidity_animation_offset;
    uint8_t prev_face_index;
    unsigned long prev_update_time;
    int prev_elapsed_minutes;

    void drawTemperatureSection(float temp);
    void drawHumiditySection(float humidity);
    void drawDISection(float di);
    void drawOutdoorTemperature(float temp);
    void drawOutdoorHumidity(float humidity);
    void drawOutdoorBatteryVoltage(float voltage);
    float calculateDI(float temp, float humidity);
    void drawElapsedTime(uint32_t lastUpdateTime);
};
