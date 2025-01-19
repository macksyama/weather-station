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
                data.valid = true;
                // 天気コードの取得（1番目の配列の1番目のtimeSeriesの1番目のweatherCodes）
                data.weatherCode = doc[0]["timeSeries"][0]["areas"][0]["weatherCodes"][0].as<String>();
                // 降水確率の取得（1番目の配列の2番目のtimeSeriesの降水確率）
                JsonArray pops = doc[0]["timeSeries"][1]["areas"][0]["pops"];
                if (pops.size() >= 2) {
                    data.rain_prob_within_6h = pops[0].as<int>();
                    data.rain_prob_within_12h = pops[1].as<int>();
                }
                break;
            }
        }
    }
    return data;
}
