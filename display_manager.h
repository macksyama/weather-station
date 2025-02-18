#pragma once
#include <Adafruit_ILI9341.h>
#include "config.h"
#include "image_data.h"
#include "weather_api_manager.h" 

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
    void drawWeatherSection(const WeatherAPIData& apiData);
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
    // 天気予報関連の変数
    String prev_weather_code;
    int prev_rain_prob_6h = -1;
    int prev_rain_prob_12h = -1;
    
    // 天気予報セクションの描画位置
    static const int WEATHER_ICON_X = LEFT_WIDTH + 10;
    static const int WEATHER_ICON_Y = 0;
    static const int RAIN_PROB_X = LEFT_WIDTH + 10;
    static const int RAIN_PROB_Y = 105;

    void drawTemperatureSection(float temp);
    void drawHumiditySection(float humidity);
    void drawDISection(float di);
    void drawOutdoorTemperature(float temp);
    void drawOutdoorHumidity(float humidity);
    void drawOutdoorBatteryVoltage(float voltage);
    float calculateDI(float temp, float humidity);
    void drawElapsedTime(uint32_t lastUpdateTime);
};
