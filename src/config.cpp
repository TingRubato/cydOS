#include "config.h"
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>

DeviceConfig g_config;

bool loadConfig(const char* path) {
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount SPIFFS");
        return false;
    }
    File file = SPIFFS.open(path, "r");
    if (!file) {
        Serial.println("Failed to open config file");
        return false;
    }
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();
    if (err) {
        Serial.println("Failed to parse config file");
        return false;
    }
    g_config.wifi_ssid = doc["wifi_ssid"] | "";
    g_config.wifi_password = doc["wifi_password"] | "";
    g_config.deviceId = doc["deviceId"] | "";
    g_config.department = doc["department"] | "";
    g_config.stationId = doc["stationId"] | "";
    g_config.location = doc["location"] | "";
    g_config.firmwareVersion = doc["firmwareVersion"] | "";
    return true;
} 