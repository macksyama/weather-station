// weather_station.h
#pragma once

#include <Adafruit_AHTX0.h>
#include "pir_manager.h"
#include "display_manager.h"
#include "aht_manager.h"
#include "ble_manager.h"
#include "config.h"

class WeatherStation {
private:
    struct RTCData {
        float indoor_temp;
        float indoor_humidity;
        float outdoor_temp;
        float outdoor_humidity;
        float battery_voltage;
        uint32_t last_ble_received;
    };

    static RTC_DATA_ATTR RTCData rtc_data;
    PIRManager& pir_manager;
    DisplayManager& display;
    AHT20Manager& aht_manager;
    BLEManager& ble_manager;

public:
    WeatherStation(PIRManager& pir, DisplayManager& disp, 
                  AHT20Manager& aht_manager, BLEManager& ble);
    void begin();

private:
    void initializeDevices();
    void handleWakeupReason();
    void prepareForSleep();
    void performScheduledTask();
    void performPIRTask();
    void powerOffDevices();
    bool initializeAHT20();
    void saveCurrentData(float temp, float humidity, WeatherData& outdoorData);
};
