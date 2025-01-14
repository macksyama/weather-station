#include "ble_manager.h"

BLEManager::BLEManager() 
    : pBLEScan(nullptr)
    , lastValidData() {  // デフォルトコンストラクタ呼び出し
}

void BLEManager::begin() {
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(1500);
    pBLEScan->setWindow(1500);
}

WeatherData BLEManager::scanForWeatherData() {
    WeatherData currentData = {0.0, 0.0, 0.0, 0.0, false}; // 一時データ
    lastValidData.valid = false;
    
    Serial.println("\n==== BLEスキャン開始 ====");
    BLEScanResults* foundDevices = pBLEScan->start(SCAN_TIME, false);
    Serial.printf("検出デバイス数: %d\n", foundDevices->getCount());
    
    for (int i = 0; i < foundDevices->getCount(); i++) {
        BLEAdvertisedDevice device = foundDevices->getDevice(i);
        Serial.printf("デバイス%d: %s\n", i, device.getName().c_str());
        
        if (device.haveName() && device.getName() == "WeatherBeacon") {
            Serial.println("WeatherBeaconを検出!");
            
            if (device.haveManufacturerData()) {
                String manufacturerData = device.getManufacturerData();
                Serial.print("生データ: ");
                for (size_t j = 0; j < manufacturerData.length(); j++) {
                    Serial.printf("%02X ", (uint8_t)manufacturerData[j]);
                }
                Serial.println();
                
                if (manufacturerData.length() > 13) {
                    lastValidData.valid = true;  // 有効フラグを設定

                    int16_t tempInt = (manufacturerData[7] << 8) | manufacturerData[8];
                    uint8_t humInt = manufacturerData[9];
                    int16_t pressInt = (manufacturerData[10] << 8) | manufacturerData[11];
                    uint16_t battInt = (manufacturerData[12] << 8) | manufacturerData[13];
                    
                    currentData.temperature = tempInt / 100.0;
                    currentData.humidity = humInt;
                    currentData.pressure = (pressInt / 10.0) + 1000.0;
                    currentData.battery_voltage = battInt / 100.0;
                    
                    // 有効なデータを受信した場合のみ、lastValidDataを更新
                    if (currentData.temperature != 0.0) lastValidData.temperature = currentData.temperature;
                    if (currentData.humidity != 0.0) lastValidData.humidity = currentData.humidity;
                    if (currentData.pressure != 0.0) lastValidData.pressure = currentData.pressure;
                    if (currentData.battery_voltage != 0.0) lastValidData.battery_voltage = currentData.battery_voltage;

                    Serial.println("==== 受信データ ====");
                    Serial.printf("温度: %.2f°C\n", lastValidData.temperature);
                    Serial.printf("湿度: %.1f%%\n", lastValidData.humidity);
                    Serial.printf("気圧: %.1fhPa\n", lastValidData.pressure);
                    Serial.printf("電圧: %.2fV\n", lastValidData.battery_voltage);
                    Serial.println("==================");
                    break;
                } else {
                    Serial.println("データ長が不足しています");
                }
            } else {
                Serial.println("Manufacturerデータがありません");
            }
        }
    }
    
    pBLEScan->clearResults();
    return lastValidData;  // 常に最後の有効なデータを返す
}
