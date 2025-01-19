#include "weather_api_manager.h"

const char* WeatherAPIManager::JMA_ENDPOINT = "https://www.jma.go.jp/bosai/forecast/data/forecast/130000.json";

WeatherAPIManager::WeatherAPIManager() {}

WeatherAPIData WeatherAPIManager::getWeatherData() {
    WeatherAPIData data = {.valid = false};
    
    if (http.begin(JMA_ENDPOINT)) {
        int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            data = parseWeatherData(payload);
        }
        http.end();
    }
    return data;
}

WeatherAPIData WeatherAPIManager::parseWeatherData(const String& payload) {
    WeatherAPIData data = {.valid = false};
    DynamicJsonDocument doc(8192);
    
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
        JsonArray areas = doc[0]["timeSeries"][0]["areas"];
        for (JsonObject area : areas) {
            if (area["area"]["code"] == "130010") {
                data.weather = area["weathers"][0].as<String>();
                data.temperature = doc[0]["timeSeries"][2]["areas"][0]["temps"][0].as<float>();
                data.humidity = 0; // 湿度データは別途計算が必要
                data.valid = true;
                break;
            }
        }
    }
    return data;
}
