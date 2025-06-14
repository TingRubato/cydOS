/**
 * @file WIFI_utils.cpp
 * @brief Implements WiFi management utilities for cydOS.
 *
 * Provides functions for WiFi initialization, connection, scanning, and credential management.
 */
#include <WIFI_utils.h>
#include "file_utils.h"
#include <SdFat.h>
#include <vector>
#include <WiFi.h>
#if defined(ESP32)
extern "C" {
#include <esp_wifi.h>
}
#endif

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

uint8_t scanNetworks(String networks[], uint8_t maxNetworks) {
    int n = WiFi.scanNetworks();
    uint8_t count = min(n, (int)maxNetworks);
    for (uint8_t i = 0; i < count; ++i) {
        networks[i] = WiFi.SSID(i);
    }
    return count;
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
                Serial.println("[WiFi] Connected successfully!");
                wifi_mode_t mode = WiFi.getMode();
                Serial.print("[WiFi] Mode: ");
                switch (mode) {
                    case WIFI_OFF: Serial.print("OFF"); break;
                    case WIFI_STA: Serial.print("Station"); break;
                    case WIFI_AP: Serial.print("Access Point"); break;
                    case WIFI_AP_STA: Serial.print("AP+Station"); break;
                    default: Serial.print("Unknown"); break;
                }
                Serial.print(", Channel: ");
                Serial.print(WiFi.channel());
                Serial.print(", RSSI: ");
                Serial.println(WiFi.RSSI());
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
    // Print error details if connection failed
    wl_status_t status = WiFi.status();
    Serial.print("[WiFi] Failed to connect after ");
    Serial.print(maxRetries);
    Serial.println(" attempts.");
    Serial.print("[WiFi] Final status: ");
    Serial.println(status);
    switch (status) {
        case WL_NO_SSID_AVAIL:
            Serial.println("[WiFi] SSID not available.");
            break;
        case WL_CONNECT_FAILED:
            Serial.println("[WiFi] Connection failed (wrong password or AP not responding).");
            break;
        case WL_CONNECTION_LOST:
            Serial.println("[WiFi] Connection lost.");
            break;
        case WL_DISCONNECTED:
            Serial.println("[WiFi] Disconnected.");
            break;
        case WL_IDLE_STATUS:
            Serial.println("[WiFi] Idle status.");
            break;
        default:
            Serial.println("[WiFi] Unknown error.");
            break;
    }
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
