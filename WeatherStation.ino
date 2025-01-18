#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "aht_manager.h"
#include "display_manager.h"
#include "ble_manager.h"
#include "pir_manager.h"
#include "weather_station.h"

// グローバルオブジェクトの初期化
PIRManager pirManager(PIR_PIN);
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);
DisplayManager display(tft, TFT_BL);
Adafruit_AHTX0 aht;
AHT20Manager ahtManager(aht, AHT20_POWER_PIN);
BLEManager bleManager;
WeatherStation station(pirManager, display, ahtManager, bleManager);

void setup() {
    Serial.begin(9600);
    
    // AHT20への電源供給
    ahtManager.powerOn();
    Wire.begin(SDA_PIN, SCL_PIN);

    // WeatherStationの初期化と実行
    station.begin();
    
    // ディープスリープ開始
    Serial.println("Deep Sleep開始");
    esp_deep_sleep_start();
}

void loop() {
    // ディープスリープ使用時はloop不要
}
