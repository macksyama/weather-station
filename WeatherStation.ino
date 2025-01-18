#include "config.h"
#include "pir_manager.h"
#include "display_manager.h"
#include "ble_manager.h"
#include <Adafruit_AHTX0.h>
#include <SPI.h>

// RTCメモリによる変数定義
RTC_DATA_ATTR struct {
    float indoor_temp;
    float indoor_humidity;
    float outdoor_temp;
    float outdoor_humidity;
    float battery_voltage;
    uint32_t last_ble_received;  // 最後にBLEを受信してからの経過時間（秒）
} rtc_data = {0};

PIRManager pirManager(PIR_PIN);
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);
DisplayManager display(tft, TFT_BL);
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

void initializeDevices() {
    // PIRセンサー初期化
    pirManager.begin();
    
    // AHT20センサー初期化
    if (!initializeAHT20()) {
        Serial.println("AHT20初期化失敗");
        esp_deep_sleep_start();
    }
    
    // ディスプレイ初期化
    SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
    display.begin();
    
    // BLE初期化
    bleManager.begin();
}

void performScheduledTask() {
    Serial.println("定期的なBLEスキャン開始");
    WeatherData outdoorData;
    unsigned long startTime = millis();
    const unsigned long SCAN_TIMEOUT = SLEEP_DURATION_SEC * 1000;

    while ((millis() - startTime) < SCAN_TIMEOUT) {
      // PIRセンサーの状態を確認
        if (pirManager.isMotionDetected()) {
            performPIRTask();
        }

        // BLE scan
        outdoorData = bleManager.scanForWeatherData();
        if (outdoorData.valid) {
            display.updateLastUpdateTime();
            Serial.println("BLEデータ受信成功");
            // 室内センサーデータ取得
            sensors_event_t humidity, temp;
            aht.getEvent(&humidity, &temp);
            // データを保存
            saveCurrentData(temp.temperature, humidity.relative_humidity, outdoorData);
            return;
        }
        delay(1000); // スキャン間隔
    }
    
    Serial.println("タイムアウト：BLEデータ受信失敗");
}

void performPIRTask() {
    Serial.println("PIR検知による起動");
    display.turnOn();
    
    // 室内センサーデータ取得
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    rtc_data.indoor_temp = temp.temperature;
    rtc_data.indoor_humidity = humidity.relative_humidity;

    display.updateDisplay(rtc_data.indoor_temp,
                        rtc_data.indoor_humidity,
                        rtc_data.outdoor_temp,
                        rtc_data.outdoor_humidity,
                        rtc_data.battery_voltage,
                        rtc_data.last_ble_received);
    // 表示時間待機
    delay(DISPLAY_TIMEOUT);
    display.turnOff();
}

void configureSleep() {
    // BLE受信からの経過時間を更新
    rtc_data.last_ble_received += SLEEP_DURATION_SEC;

    // 1minごとのタイマー割り込み設定
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION_SEC * 1000000ULL);
    
    // PIRセンサーによる外部割り込み設定
    esp_deep_sleep_enable_gpio_wakeup(1 << PIR_PIN, ESP_GPIO_WAKEUP_GPIO_HIGH);
}

void powerOffDevices() {
    // ディスプレイバックライトをOFF
    display.turnOff();
    
    // AHT20センサーの電源をOFF
    digitalWrite(AHT20_POWER_PIN, LOW);
    gpio_hold_en(AHT20_POWER_PIN);

    gpio_deep_sleep_hold_en();
}

void saveCurrentData(float temp, float humidity, WeatherData& outdoorData) {
    rtc_data.indoor_temp = temp;
    rtc_data.indoor_humidity = humidity;
    if (outdoorData.valid) {
        rtc_data.outdoor_temp = outdoorData.temperature;
        rtc_data.outdoor_humidity = outdoorData.humidity;
        rtc_data.battery_voltage = outdoorData.battery_voltage;
        rtc_data.last_ble_received = 0;  // BLE受信時点でリセット
    }
}

void setup() {
    Serial.begin(9600);

    // 起動理由の確認
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    gpio_deep_sleep_hold_dis();

    // AHT20電源制御ピンの設定
    gpio_hold_dis(AHT20_POWER_PIN);
    pinMode(AHT20_POWER_PIN, OUTPUT);
    digitalWrite(AHT20_POWER_PIN, HIGH);
    delay(50);

    // I2Cバス初期化
    Wire.begin(SDA_PIN, SCL_PIN);

    // デバイスの初期化
    initializeDevices();
    
    // 定期的なBLEスキャンモード
    if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
        performScheduledTask();
    }
    // PIRセンサーによる起動
    else if (wakeup_reason == ESP_SLEEP_WAKEUP_UNDEFINED || wakeup_reason == ESP_SLEEP_WAKEUP_GPIO) {
        performPIRTask();
    }

    // スリープ前の処理
    Serial.println("Power off devices");
    powerOffDevices();
    
    // スリープモードの設定
    configureSleep();

    // ディープスリープ開始
    Serial.println("Start Deep Sleep");
    esp_deep_sleep_start();
}

void loop() {
  // ディープスリープ使用時はloop不要
}