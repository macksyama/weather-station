#pragma once
#include <HTTPClient.h>
#include <ArduinoJson.h>

struct WeatherAPIData {
    String weather;
    float temperature;
    float humidity;
    bool valid;
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
