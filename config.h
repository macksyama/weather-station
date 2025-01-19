#pragma once
#include <cstdint> 

// ピン定義
#define TFT_MISO  GPIO_NUM_5
#define TFT_MOSI  GPIO_NUM_6
#define TFT_SCK   GPIO_NUM_4
#define TFT_CS    GPIO_NUM_7
#define TFT_DC    GPIO_NUM_0
#define TFT_RST   GPIO_NUM_1
#define TFT_BL GPIO_NUM_2  // バックライトのピン
#define PIR_PIN GPIO_NUM_3  // PIRセンサーのピン
#define SDA_PIN GPIO_NUM_8
#define SCL_PIN GPIO_NUM_9
#define AHT20_POWER_PIN GPIO_NUM_10 // AHT20の電源制御用GPIO

// 人感検知後のディスプレイ表示時間(ms)
#define DISPLAY_TIMEOUT (30000 - 2500)// time delay: 2500ms

// 画面レイアウト定数
#define TFT_WIDTH 320
#define TFT_HEIGHT 240
#define LEFT_WIDTH 190
#define RIGHT_WIDTH 130
#define SECTION_HEIGHT 120

// 外気温湿度表示用の定数
#define OUTDOOR_TEXT_SIZE 2
#define OUTDOOR_TEMP_Y_OFFSET 80
#define OUTDOOR_HUMIDITY_Y_OFFSET 200

// アニメーション用の定数
#define TEMP_MAX 40.0
#define ANIMATION_HEIGHT 84.0

#define WEAHTER_SECTION_WIDTH (TFT_WIDTH - LEFT_WIDTH)

// BLE設定
#define SCAN_TIME 10

// スリープ時間（秒）
const uint32_t SLEEP_DURATION_SEC = 60;
// スキャン時間（秒）
const uint32_t SCAN_DURATION_SEC = 60;
