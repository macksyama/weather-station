#pragma once
#include <WiFi.h>

class WiFiManager {
private:
    const char* ssid;
    const char* password;
    bool isConnected;

public:
    WiFiManager(const char* wifi_ssid, const char* wifi_password);
    bool begin();
    void disconnect();
    bool isWiFiConnected() const { return isConnected; }
};
