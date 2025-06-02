#include "WIFI_utils.h"
#include "file_utils.h"
#include <SdFat.h>
#include <LittleFS.h>
#include <vector>
#include <WiFi.h>
#include <FS.h>
#include <algorithm>

using std::min;

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
    static uint8_t retryCount = 0;
    const uint8_t maxRetries = 5;
    const uint16_t baseDelayMs = 1000;
    
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    Serial.print("Using password: ");
    Serial.println(password);

    WiFi.disconnect(true);
    WiFi.begin(ssid, password);

    for (uint8_t i = 0; i < maxRetries; i++) {
        uint32_t startTime = millis();
        uint8_t min_i_3 = (i < 3) ? i : 3;
        while (WiFi.status() != WL_CONNECTED && 
              (millis() - startTime) < (baseDelayMs * (1 << min_i_3))) {
            delay(100);
            if (WiFi.status() == WL_CONNECTED) {
                retryCount = 0;
                return true;
            }
        }
        
        if (i < maxRetries - 1) {
            WiFi.disconnect(true);
            delay(100);
            WiFi.begin(ssid, password);
        }
    }
    
    retryCount = (retryCount + 1 < 10) ? retryCount + 1 : 10;
    return false;
}


// #include "file_utils.h"

bool loadWiFiCredentials(const char* ssid, char* password, size_t maxLen) {
    String content = FileUtils::readFile("/config/wifi.csv");
    if (content.isEmpty()) return false;

    int startPos = 0;
    while (startPos < content.length()) {
        int endPos = content.indexOf('\n', startPos);
        if (endPos == -1) endPos = content.length();
        
        String line = content.substring(startPos, endPos);
        int separator = line.indexOf(',');
        
        if (separator != -1) {
            String savedSSID = line.substring(0, separator);
            String savedPassword = line.substring(separator + 1);
            
            if (savedSSID == ssid) {
                savedPassword.toCharArray(password, maxLen);
                return true;
            }
        }
        startPos = endPos + 1;
    }
    return false;
}

void saveWiFiCredentials(const char* ssid, const char* password) {
    String entry = String(ssid) + "," + password + "\n";
    if (!FileUtils::writeFile("/config/wifi.csv", entry.c_str(), true)) {
        Serial.println("Failed to save WiFi credentials");
    }
}
