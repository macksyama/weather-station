#include "wifi_manager.h"

WiFiManager::WiFiManager(const char* wifi_ssid, const char* wifi_password)
    : ssid(wifi_ssid), password(wifi_password), isConnected(false) {}

bool WiFiManager::begin() {
    WiFi.begin(ssid, password);
    int retry_count = 0;
    while (WiFi.status() != WL_CONNECTED && retry_count < 20) {
        delay(500);
        retry_count++;
    }
    isConnected = (WiFi.status() == WL_CONNECTED);
    return isConnected;
}

void WiFiManager::disconnect() {
    WiFi.disconnect(true);
    isConnected = false;
}
