#pragma once

#include <Adafruit_AHTX0.h>
#include <driver/gpio.h>

// センサーデータを格納する構造体
struct SensorData {
    float temperature;
    float humidity;
};

class AHT20Manager {
private:
    Adafruit_AHTX0& sensor;
    const gpio_num_t power_pin;

public:
    // コンストラクタ
    AHT20Manager(Adafruit_AHTX0& aht, gpio_num_t pin);
    // センサーの初期化
    bool initialize();
    // 電源ON
    bool powerOn();
    // 電源OFF
    void powerOff();
    // センサーデータの読み取り
    SensorData readData();
};
