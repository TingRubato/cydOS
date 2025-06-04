#pragma once
#include <Arduino.h>

struct DeviceConfig {
    String wifi_ssid;
    String wifi_password;
    String location;
    String station;
    String topic_name;
};

extern DeviceConfig g_config;
bool loadConfig(const char* path = "/config/device_config.json"); 