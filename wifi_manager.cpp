#include "wifi_manager.h"

WiFiManager::WiFiManager(const char* wifi_ssid, const char* wifi_password)
    : ssid(wifi_ssid), password(wifi_password), isConnected(false) {}

bool WiFiManager::begin() {
    Serial.print("WiFiに接続しています...");
    WiFi.begin(ssid, password);
    int retry_count = 0;
    while (WiFi.status() != WL_CONNECTED && retry_count < 20) {
        delay(500);
        Serial.print(".");
        retry_count++;
    }
    isConnected = (WiFi.status() == WL_CONNECTED);
    Serial.println("");
    if (isConnected) {
        Serial.println("WiFi接続成功");
        Serial.print("IPアドレス: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi接続失敗");
    }
    return isConnected;
}

void WiFiManager::disconnect() {
    WiFi.disconnect(true);
    isConnected = false;
}
