#pragma once
#include <BLEDevice.h>
#include "config.h"

struct WeatherData {
    float temperature = 0.0f;
    float humidity = 0.0f;
    float pressure = 0.0f;
    float battery_voltage = 0.0f;
    bool valid = false;
};

class BLEManager {
public:
    BLEManager();
    void begin();
    WeatherData scanForWeatherData();
private:
    BLEScan* pBLEScan;
    WeatherData lastValidData;  // 最後の有効なデータを保持する変数
    WeatherData parseManufacturerData(const String& data);
};
