#pragma once
#include <HTTPClient.h>
#include <ArduinoJson.h>

struct WeatherAPIData {
    bool valid;
    String weather;
    String weatherCode;      // 当日の天気コード
    int rain_prob_within_6h;     // -6hの降水確率
    int rain_prob_within_12h;    // -12hの降水確率
};

class WeatherAPIManager {
private:
    static const char* JMA_ENDPOINT;
    HTTPClient http;
    
public:
    WeatherAPIManager();
    WeatherAPIData getWeatherData();
private:
    WeatherAPIData parseWeatherData(const String& payload);
};
