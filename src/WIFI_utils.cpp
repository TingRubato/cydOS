#include "WIFI_utils.h"
#include <SdFat.h>
#include <SPIFFS.h>
#include <vector> // Include vector header

extern SdFat sd;

void initWiFi() {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    delay(100);
}

void enableWiFi() {
    WiFi.begin();
}

void disableWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

bool isWiFiEnabled() {
    return WiFi.status() != WL_NO_SHIELD;
}

String getCurrentNetworkInfo() {
    if (WiFi.status() != WL_CONNECTED) {
        return "Not connected";
    }
    String info = "SSID: " + WiFi.SSID() + "\n";
    info += "IP Address: " + WiFi.localIP().toString() + "\n";
    info += "Gateway: " + WiFi.gatewayIP().toString() + "\n";
    info += "Subnet: " + WiFi.subnetMask().toString() + "\n";
    info += "RSSI: " + String(WiFi.RSSI()) + " dBm";
    return info;
}

std::vector<String> scanNetworks() {
    std::vector<String> networks;
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
        networks.push_back(WiFi.SSID(i));
    }
    return networks;
}

bool connectToNetwork(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    int timeout = 10;
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        delay(1000);
        --timeout;
    }
    return WiFi.status() == WL_CONNECTED;
}

void saveWiFiCredentials(const char* ssid, const char* password) {
    if (!SPIFFS.begin()) {
        SPIFFS.format();
        SPIFFS.begin();
    }
    File file = SPIFFS.open("/config/wifi.csv", FILE_APPEND);
    if (!file) {
        return;
    }
    file.printf("%s,%s\n", ssid, password);
    file.close();
}

bool loadWiFiCredentials(const char* ssid, char* password, size_t maxLen) {
    if (!SPIFFS.begin()) {
        return false;
    }
    File file = SPIFFS.open("/config/wifi.csv", FILE_READ);
    if (!file) {
        return false;
    }
    while (file.available()) {
        String line = file.readStringUntil('\n');
        int separator = line.indexOf(',');
        String savedSSID = line.substring(0, separator);
        String savedPassword = line.substring(separator + 1);
        if (savedSSID == ssid) {
            savedPassword.toCharArray(password, maxLen);
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}