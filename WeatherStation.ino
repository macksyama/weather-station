#include "config.h"
#include "display_manager.h"
#include "ble_manager.h"
#include <Adafruit_AHTX0.h>
#include <SPI.h>

Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);
DisplayManager display(tft);
Adafruit_AHTX0 aht;
BLEManager bleManager;

// AHT20センサーの初期化関数
bool initializeAHT20() {
    int retry = 5; // 最大5回再試行
    while (retry--) {
        if (aht.begin()) {
            Serial.println("AHT20の初期化完了");
            return true;
        }
        Serial.println("AHT20の初期化に失敗しました。再試行中...");
        delay(1000);
    }
    Serial.println("AHT20の初期化に失敗しました。プログラム停止");
    return false;
}

void setup() {
    Serial.begin(9600);
    
    // AHT20センサー初期化
    if (!initializeAHT20()) {
        while (1) delay(10);
    }
    
    // ディスプレイ初期化
    SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
    display.begin();
    
    // BLE初期化
    bleManager.begin();
}

void loop() {
    // メモリ使用状況のモニタリング
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    
    // センサーデータ取得
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    
    // BLEスキャンとデータ更新
    WeatherData outdoorData = bleManager.scanForWeatherData();
    if (outdoorData.valid) {
        display.updateLastUpdateTime();
        Serial.println("外気データ更新時刻を記録しました");
    }
    
    // ディスプレイ更新
    display.updateDisplay(temp.temperature, 
                         humidity.relative_humidity,
                         outdoorData.temperature,
                         outdoorData.humidity,
                         outdoorData.battery_voltage);
    
    delay(5000);
}