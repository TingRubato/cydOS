#pragma once
#include <Arduino.h>

struct DeviceConfig {
    String wifi_ssid;
    String wifi_password;
    String deviceId;
    String department;
    String stationId;
    String location;
    String firmwareVersion;
};

extern DeviceConfig g_config;
bool loadConfig(const char* path = "/config/device_config.json"); 