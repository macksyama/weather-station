// weather_station.cpp
#include "weather_station.h"

RTC_DATA_ATTR WeatherStation::RTCData WeatherStation::rtc_data;

WeatherStation::WeatherStation(PIRManager& pir, DisplayManager& disp, 
                             AHT20Manager& aht_manager, BLEManager& ble)
    : pir_manager(pir), display(disp), aht_manager(aht_manager), 
      ble_manager(ble) {
      }

void WeatherStation::begin() {
    initializeDevices();
    // 初回起動時のみ初期化
    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
        rtc_data = {0};
    }
    handleWakeupReason();
    prepareForSleep();
}

void WeatherStation::initializeDevices() {
    pir_manager.begin();
    
    if (!initializeAHT20()) {
        Serial.println("AHT20初期化失敗");
        esp_deep_sleep_start();
    }
    
    SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
    display.begin();
    
    ble_manager.begin();
}

bool WeatherStation::initializeAHT20() {
    return aht_manager.initialize();
}

void WeatherStation::handleWakeupReason() {
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
        performScheduledTask();
    }
    else if (wakeup_reason == ESP_SLEEP_WAKEUP_UNDEFINED || 
             wakeup_reason == ESP_SLEEP_WAKEUP_GPIO) {
        performPIRTask();
    }
}

void WeatherStation::performScheduledTask() {
    Serial.println("定期的なBLEスキャン開始");
    WeatherData outdoorData;
    unsigned long startTime = millis();
    
    while ((millis() - startTime) < SCAN_DURATION_SEC * 1000) {
        if (pir_manager.isMotionDetected()) {
            performPIRTask();
        }

        outdoorData = ble_manager.scanForWeatherData();
        if (outdoorData.valid) {
            SensorData sensor_data = aht_manager.readData();
            saveCurrentData(sensor_data.temperature, sensor_data.humidity, outdoorData);
            return;
        }
        delay(1000);
    }
    rtc_data.last_ble_received += (SCAN_DURATION_SEC + SLEEP_DURATION_SEC);
    Serial.println("タイムアウト：BLEデータ受信失敗");
}

void WeatherStation::performPIRTask() {
    Serial.println("PIR検知による起動");
    display.turnOn();
    
    SensorData sensor_data = aht_manager.readData();
    rtc_data.indoor_temp = sensor_data.temperature;
    rtc_data.indoor_humidity = sensor_data.humidity;

    display.updateDisplay(rtc_data.indoor_temp,
                         rtc_data.indoor_humidity,
                         rtc_data.outdoor_temp,
                         rtc_data.outdoor_humidity,
                         rtc_data.battery_voltage,
                         rtc_data.last_ble_received);
                         
    delay(DISPLAY_TIMEOUT);
    display.turnOff();
}

void WeatherStation::prepareForSleep() {
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION_SEC * 1000000ULL);
    esp_deep_sleep_enable_gpio_wakeup(1 << PIR_PIN, ESP_GPIO_WAKEUP_GPIO_HIGH);
    powerOffDevices();
}

void WeatherStation::powerOffDevices() {
    display.turnOff();
    aht_manager.powerOff();
    gpio_deep_sleep_hold_en();
}

void WeatherStation::saveCurrentData(float temp, float humidity, WeatherData& outdoorData) {
    rtc_data.indoor_temp = temp;
    rtc_data.indoor_humidity = humidity;
    if (outdoorData.valid) {
        rtc_data.outdoor_temp = outdoorData.temperature;
        rtc_data.outdoor_humidity = outdoorData.humidity;
        rtc_data.battery_voltage = outdoorData.battery_voltage;
        rtc_data.last_ble_received = 0;
    }
}
