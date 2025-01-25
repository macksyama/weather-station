#include "display_manager.h"

DisplayManager::DisplayManager(Adafruit_ILI9341& tft, gpio_num_t blPin) : _tft(tft), 
    isFirstDraw(true), backlightPin(blPin), isDisplayOn(false), prev_temp(0), prev_humidity(0), prev_di(100.0),
    prev_outdoor_temp(0), prev_outdoor_humidity(0), prev_outdoor_battery_voltage(0),
    prev_temp_animation_offset(255), prev_humidity_animation_offset(255),
    prev_face_index(255), prev_update_time(0), prev_elapsed_minutes(-1) {}

void DisplayManager::begin() {
    pinMode(backlightPin, OUTPUT);
    digitalWrite(backlightPin, LOW);
    _tft.begin();
    _tft.setRotation(1);
    drawStaticElements();
}

void DisplayManager::turnOn() {
    gpio_hold_dis(backlightPin);
    digitalWrite(backlightPin, HIGH);
    isDisplayOn = true;
}

void DisplayManager::turnOff() {
    digitalWrite(backlightPin, LOW);
    gpio_hold_en(backlightPin);
    isDisplayOn = false;
}

void DisplayManager::drawStaticElements() {
    _tft.fillScreen(ILI9341_BLACK);
    
    // 温度セクション
    cImage_write(&image_temperature_icon, 10, (SECTION_HEIGHT - 96)/2, ILI9341_WHITE);
    _tft.setTextSize(2);
    _tft.setTextColor(ILI9341_WHITE);
    _tft.setCursor(160-10, 40-5);
    _tft.printf("o");
    _tft.setTextSize(4);
    _tft.setCursor(160, 40);
    _tft.printf("C");

    // 湿度セクション
    cImage_write(&image_humidity_icon, 0, SECTION_HEIGHT + 10, ILI9341_WHITE);
    _tft.setTextColor(ILI9341_WHITE);
    _tft.setTextSize(4);
    _tft.setCursor(160, SECTION_HEIGHT + 40);
    _tft.printf("%%");

    // 外気温の静的要素
    _tft.setTextSize(OUTDOOR_TEXT_SIZE);
    _tft.setTextColor(ILI9341_CYAN);
    _tft.setCursor(60, OUTDOOR_TEMP_Y_OFFSET);
    _tft.print("Out");
    _tft.setTextSize(OUTDOOR_TEXT_SIZE - 1);
    _tft.setCursor(160-5, OUTDOOR_TEMP_Y_OFFSET - 1);
    _tft.printf("o");
    _tft.setTextSize(OUTDOOR_TEXT_SIZE);
    _tft.setCursor(160, OUTDOOR_TEMP_Y_OFFSET);
    _tft.print("C");

    // 外気湿度の静的要素
    _tft.setTextSize(OUTDOOR_TEXT_SIZE);
    _tft.setTextColor(ILI9341_CYAN);
    _tft.setCursor(60, OUTDOOR_HUMIDITY_Y_OFFSET);
    _tft.print("Out");
    _tft.setCursor(160, OUTDOOR_HUMIDITY_Y_OFFSET);
    _tft.print("%");
}

void DisplayManager::updateDisplay(float temp, float humidity, 
                                 float outdoor_temp, float outdoor_humidity,
                                 float outdoor_battery_voltage, uint32_t last_ble_received) {
    // シリアル出力を追加
    float di = calculateDI(temp, humidity);
    Serial.println("==== センサー値 ====");
    Serial.printf("室内温度: %.1f°C\n", temp);
    Serial.printf("室内湿度: %.1f%%\n", humidity);
    Serial.printf("不快指数: %.1f\n", di);
    Serial.printf("外気温: %.1f°C\n", outdoor_temp);
    Serial.printf("外気湿度: %.1f%%\n", outdoor_humidity);
    Serial.printf("子機電圧: %.2fV\n", outdoor_battery_voltage);
    Serial.printf("経過時間: %dm\n", (int)(last_ble_received/60));
    Serial.println("================");

    drawTemperatureSection(temp);
    drawHumiditySection(humidity);
    drawDISection(calculateDI(temp, humidity));
    drawOutdoorTemperature(outdoor_temp);
    drawOutdoorHumidity(outdoor_humidity);
    //drawOutdoorBatteryVoltage(outdoor_battery_voltage);
    drawElapsedTime(last_ble_received);
    isFirstDraw = false;
}

void DisplayManager::drawTemperatureSection(float temp) {
    float temp_constrained = constrain(temp, 0.0, TEMP_MAX);
    uint8_t animation_offset = (uint8_t)(ANIMATION_HEIGHT - ANIMATION_HEIGHT * (temp_constrained / TEMP_MAX));
    
    if (isFirstDraw || abs(temp - prev_temp) >= 0.1) {
        _tft.fillRect(60, 40, 90, 40, ILI9341_BLACK);
        int intPart = (int)temp;
        _tft.setTextSize(4);
        _tft.setTextColor(ILI9341_WHITE);
        _tft.setCursor(60, 40);
        _tft.print(intPart);
        
        int decPart = (int)((temp - intPart) * 10);
        _tft.setTextSize(2);
        _tft.setCursor(60 + (intPart < 10 ? 24 : 48), 50);
        _tft.printf(".%d", decPart);
        
        prev_temp = temp;
    }
    
    if (isFirstDraw || prev_temp_animation_offset != animation_offset) {
        cImage_writeSlide(&image_temperature_value, 18, (SECTION_HEIGHT - 96)/2 + 4, ILI9341_RED, animation_offset);
        prev_temp_animation_offset = animation_offset;
    }
}

void DisplayManager::drawHumiditySection(float humidity) {
  uint8_t animation_offset = (uint8_t)(84.0 - 84.0 * (humidity / 100.0));
  
  // 湿度値が変化した場合のみ数値を更新
  if (isFirstDraw || abs(humidity - prev_humidity) >= 0.1) {
    _tft.fillRect(60, SECTION_HEIGHT + 40, 70, 40, ILI9341_BLACK);
    // 整数部分を大きく表示
    int intPart = (int)humidity;
    _tft.setTextSize(4);
    _tft.setTextColor(ILI9341_WHITE);
    _tft.setCursor(60, SECTION_HEIGHT + 40);
    _tft.print(intPart);
    
    // 小数点と小数部分を小さく表示
    int decPart = (int)((humidity - intPart) * 10);
    _tft.setTextSize(2);
    _tft.setCursor(60 + (intPart < 10 ? 24 : 48), SECTION_HEIGHT + 50);
    _tft.printf(".%d", decPart);
    
    prev_humidity = humidity;
  }
  
  // アニメーションの値が変化した場合のみバーを更新
  if (isFirstDraw || prev_humidity_animation_offset != animation_offset) {
    cImage_writeSlide(&image_humidity_value, 0 + 3, (SECTION_HEIGHT + 10) + 8, ILI9341_BLUE, animation_offset);
    prev_humidity_animation_offset = animation_offset;
  }
}

void DisplayManager::drawOutdoorTemperature(float outdoor_temp) {
  if (isFirstDraw || abs(outdoor_temp - prev_outdoor_temp) >= 0.1) {
    _tft.setTextSize(OUTDOOR_TEXT_SIZE);
    _tft.setTextColor(ILI9341_CYAN);
    // 数値部分のみを更新
    _tft.fillRect(100, OUTDOOR_TEMP_Y_OFFSET, 50, 15, ILI9341_BLACK);
    _tft.setCursor(100, OUTDOOR_TEMP_Y_OFFSET);
    _tft.print(outdoor_temp, 1);
    
    prev_outdoor_temp = outdoor_temp;
  }
}

void DisplayManager::drawOutdoorHumidity(float outdoor_humidity) {
  if (isFirstDraw || abs(outdoor_humidity - prev_outdoor_humidity) >= 0.1) {
    _tft.setTextSize(OUTDOOR_TEXT_SIZE);
    _tft.setTextColor(ILI9341_CYAN);
    // 数値部分のみを更新
    _tft.fillRect(100, OUTDOOR_HUMIDITY_Y_OFFSET, 50, 15, ILI9341_BLACK);
    _tft.setCursor(100, OUTDOOR_HUMIDITY_Y_OFFSET);
    _tft.print(outdoor_humidity, 1);
    
    prev_outdoor_humidity = outdoor_humidity;
  }
}

void DisplayManager::drawOutdoorBatteryVoltage(float outdoor_battery_voltage) {
  if (isFirstDraw || abs(outdoor_battery_voltage - prev_outdoor_battery_voltage) >= 0.01) {
    _tft.setTextSize(OUTDOOR_TEXT_SIZE);
    _tft.setTextColor(ILI9341_CYAN);
    // 数値部分のみを更新
    _tft.fillRect(WEAHTER_SECTION_WIDTH, 10, 50, 15, ILI9341_BLACK);
    _tft.setCursor(WEAHTER_SECTION_WIDTH, 10);
    _tft.print(outdoor_battery_voltage, 2);
    
    prev_outdoor_humidity = outdoor_battery_voltage;
  }
}

void DisplayManager::drawDISection(float di) {
  // 不快指数に応じて表情アイコンを選択
  uint8_t face_index;
  const CImage* face;
  
  if (di < 70.0) {
    face_index = 0;
    face = &image_di_face1;
  } else if (di < 75.0) {
    face_index = 1;
    face = &image_di_face2;
  } else if (di < 80.0) {
    face_index = 2;
    face = &image_di_face3;
  } else {
    face_index = 3;
    face = &image_di_face4;
  }
  
  // 表情が変化した場合のみアイコンを更新
  if (isFirstDraw || prev_face_index != face_index) {
    _tft.fillRect(LEFT_WIDTH + 30, (TFT_HEIGHT-face->height), face->width, face->height, ILI9341_BLACK);
    cImage_write(face, LEFT_WIDTH + 30, (TFT_HEIGHT-face->height), ILI9341_WHITE);
    prev_face_index = face_index;
  }
  
  // 不快指数の値が変化した場合のみ数値を更新
  // if (isFirstDraw || abs(di - prev_di) >= 0.1) {
  //   _tft.fillRect(TFT_WIDTH - 30, TFT_HEIGHT - 20, 30, 20, ILI9341_BLACK);
  //   _tft.setTextSize(2);
  //   _tft.setTextColor(ILI9341_WHITE);
  //   _tft.setCursor(TFT_WIDTH - 30, TFT_HEIGHT - 20);
  //   _tft.printf("%.0f", di);
  //   prev_di = di;
  // }
}

float DisplayManager::calculateDI(float temp, float humidity) {
    return 0.81 * temp + 0.01 * humidity * (0.99 * temp - 14.3) + 46.3;
}

void DisplayManager::drawElapsedTime(uint32_t last_ble_received) {
    int elapsed_minutes = last_ble_received / 60;
    
    if (isFirstDraw || elapsed_minutes != prev_elapsed_minutes) {
        _tft.setTextSize(OUTDOOR_TEXT_SIZE);
        _tft.setTextColor(ILI9341_CYAN);
        _tft.fillRect(LEFT_WIDTH - 50, 10, 50, 15, ILI9341_BLACK);
        _tft.setCursor(LEFT_WIDTH - 50, 10);
        _tft.printf("%dm", elapsed_minutes);
        
        prev_elapsed_minutes = elapsed_minutes;
    }
}

void DisplayManager::drawWeatherSection(const WeatherAPIData& apiData) {
  if (!apiData.valid) return;

  // 天気コードが変化した場合のみアイコンを更新
  if (isFirstDraw || apiData.weatherCode != prev_weather_code) {
    // 天気コードに応じたアイコンを選択
    const CImage* icon = nullptr;
    const CImage* icon_s = nullptr;
    const CImage* icon_t = nullptr;
    int code = apiData.weatherCode.toInt();
    uint16_t color_s;
    uint16_t color_t;
    // 晴れ系(100番台)の詳細分類
    if (code >= 100 && code < 200) {
        if (code == 110 || code == 111) {
            icon = &image_weather_icon_111;
            icon_s = &image_weather_icon_111_s;
            color_s = ILI9341_LIGHTGREY;
            icon_t = &image_weather_icon_111_t;
            color_t = ILI9341_LIGHTGREY;
        } else if (code == 101 || code == 132) {
            icon = &image_weather_icon_101;
            icon_s = &image_weather_icon_101_s;
            color_s = ILI9341_LIGHTGREY;
        } else if (code == 102 || code == 106 || code == 108 || code == 120 || code == 121) {
            icon = &image_weather_icon_102;
            icon_s = &image_weather_icon_102_s;
            color_s = ILI9341_CYAN;
        } else if (code == 103 || code == 107 || code == 140) {
            icon = &image_weather_icon_103;
            icon_s = &image_weather_icon_103_s;
            color_s = ILI9341_CYAN;
        }  else if (code == 104) {
            icon = &image_weather_icon_104;
        }  else if (code == 114 || code == 112 || code == 113 || code == 118 || code == 119 || code == 122 || code == 125 || code == 126 || code == 127 || code == 128) {
            icon = &image_weather_icon_114;
            icon_s = &image_weather_icon_114_s;
            color_s = ILI9341_CYAN;
            icon_t = &image_weather_icon_114_t;
            color_t = ILI9341_LIGHTGREY;
        } else if (code == 117) {
            icon = &image_weather_icon_117;
        } else {
            icon = &image_weather_icon_100;
        }
    }
    // 曇り系(200番台)の詳細分類
    else if (code >= 200 && code < 300) {
        if (code == 201) {
            icon = &image_weather_icon_201;
            icon_s = &image_weather_icon_201_s;
            color_s = ILI9341_ORANGE;
        } else if (code == 202 || code == 206 || code == 208 || code == 220 || code == 221) {
            icon = &image_weather_icon_202;
            icon_s = &image_weather_icon_202_s;
            color_s = ILI9341_CYAN;
        } else if (code == 203 || code == 207 || code == 240) {
            icon = &image_weather_icon_203;
            icon_s = &image_weather_icon_203_s;
            color_s = ILI9341_CYAN;
        } else if (code == 204) {
            icon = &image_weather_icon_204;
        } else if (code == 210 || code == 211) {
            icon = &image_weather_icon_211;
            icon_s = &image_weather_icon_211_s;
            color_s = ILI9341_ORANGE;
        } else if (code == 212 || code == 213 || code == 214 || code == 218 || code == 219 || code == 222 || code == 224 || code == 225 || code == 226) {
            icon = &image_weather_icon_214;
            icon_s = &image_weather_icon_214_s;
            color_s = ILI9341_CYAN;
        } else if (code == 217) {
            icon = &image_weather_icon_217;
        } else {
            icon = &image_weather_icon_200;
        }
    }
    // 雨系(300番台)の詳細分類
    else if (code >= 300 && code < 400) {
        if (code == 301) {
            icon = &image_weather_icon_301;
            icon_s = &image_weather_icon_301_s;
            color_s = ILI9341_ORANGE;
        } else if (code == 302) {
            icon = &image_weather_icon_302;
            icon_s = &image_weather_icon_301_s;
            color_s = ILI9341_LIGHTGREY;
        } else if (code == 303) {
            icon = &image_weather_icon_303;
        } else if (code == 306) {
            icon = &image_weather_icon_306;
        } else if (code == 311 || code == 316 || code == 320 || code == 323 || code == 324 || code == 325) {
            icon = &image_weather_icon_311;
            icon_s = &image_weather_icon_311_s;
            color_s = ILI9341_ORANGE;
        } else if (code == 313 || code == 317 || code == 321) {
            icon = &image_weather_icon_313;
            icon_s = &image_weather_icon_313_s;
            color_s = ILI9341_LIGHTGREY;
            icon_t = &image_weather_icon_313_t;
            color_t = ILI9341_LIGHTGREY;
        } else if (code == 315) {
            icon = &image_weather_icon_315;
        } else {
            icon = &image_weather_icon_300;
        }
    }
    // 雪系(400番台)は1アイコンで代用
    else if (code >= 400 && code < 500) {
        icon = &image_weather_icon_400;
    }
    
    // アイコンが選択された場合のみ描画
    if (icon) {
      _tft.fillRect(WEATHER_ICON_X, WEATHER_ICON_Y, icon->width, icon->height, ILI9341_BLACK);
      
      uint16_t color;
      if (code >= 100 && code < 200) {
        color = ILI9341_ORANGE;
      } else if (code >= 200 && code < 300) {
        color = ILI9341_LIGHTGREY;
      } else if (code >= 300 && code < 400) {
        color = ILI9341_CYAN;
      } else {
        color = ILI9341_WHITE;
      }
      cImage_write(icon, WEATHER_ICON_X, WEATHER_ICON_Y, color);

      if (icon_s) {
        cImage_writeSlide(icon_s, WEATHER_ICON_X, WEATHER_ICON_Y, color_s, WEATHER_ICON_Y);
      }
      if (icon_t) {
        cImage_writeSlide(icon_t, WEATHER_ICON_X, WEATHER_ICON_Y, color_t, WEATHER_ICON_Y);
      }
    }

    prev_weather_code = apiData.weatherCode;
  }

  // 降水確率が変化した場合のみ更新
  if (isFirstDraw || 
      prev_rain_prob_6h != apiData.rain_prob_within_6h || 
      prev_rain_prob_12h != apiData.rain_prob_within_12h) {
    _tft.fillRect(RAIN_PROB_X, RAIN_PROB_Y, (TFT_WIDTH - RAIN_PROB_X), 20, ILI9341_BLACK);
    _tft.setTextSize(2);
    _tft.setTextColor(ILI9341_WHITE);
    _tft.setCursor(RAIN_PROB_X, RAIN_PROB_Y);
    _tft.printf("%d%% -> %d%%", 
                apiData.rain_prob_within_6h, 
                apiData.rain_prob_within_12h);
    
    prev_rain_prob_6h = apiData.rain_prob_within_6h;
    prev_rain_prob_12h = apiData.rain_prob_within_12h;
  }
}
