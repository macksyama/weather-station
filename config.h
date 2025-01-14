#pragma once

// ピン定義
#define TFT_MISO  5
#define TFT_MOSI  6
#define TFT_SCK   4
#define TFT_CS    7
#define TFT_DC    0
#define TFT_RST   1

// 画面レイアウト定数
#define TFT_WIDTH 320
#define TFT_HEIGHT 240
#define LEFT_WIDTH 200
#define RIGHT_WIDTH 120
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
