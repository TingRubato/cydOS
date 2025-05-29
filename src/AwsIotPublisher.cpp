#include "AwsIotPublisher.h"
#include "secrets.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

WiFiClientSecure net;
PubSubClient client(net);

void begin() {
    Serial.begin(115200);
    delay(2000);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(WIFI_SSID);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        for (int i = 0; i < 10; i++) {
            if (WiFi.status() == WL_CONNECTED) break;
            delay(1000);
            Serial.print(".");
        }
        attempts++;
        Serial.print("WiFi.status() is: ");
        Serial.println(WiFi.status());
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to wifi");
    } else {
        Serial.println("Failed to connect to WiFi after 10 attempts.");
        while (1);
    }

    net.setCACert(SIMPLE_IOT_ROOT_CA);
    net.setCertificate(SIMPLE_IOT_DEVICE_CERT);
    net.setPrivateKey(SIMPLE_IOT_DEVICE_PRIVATE_KEY);
    
    client.setServer(AWS_IOT_ENDPOINT, 8883);
    
    Serial.println("Connecting to AWS IoT endpoint via TLS...");
    if (!client.connect(THINGNAME)) {
        Serial.println("TLS connection failed!");
    } else {
        Serial.println("TLS connection succeeded!");
        client.disconnect();
    }
}

String currentTimestamp() {
    time_t now = time(nullptr);
    struct tm *t = gmtime(&now);
    char ts[30];
    snprintf(ts, sizeof(ts), "%04d-%02d-%02dT%02d:%02d:%02dZ",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
    return String(ts);
}

void postHealthStatus(int batteryLevel) {
    begin();

    if (!client.connect(THINGNAME)) {
        Serial.println("TLS connection failed! Cannot publish health status.");
        return;
    }

    JsonDocument doc;
    doc["deviceType"] = "caller-button";
    doc["welderId"]   = "welder-01";
    doc["status"]     = "online";
    doc["timestamp"]  = currentTimestamp();
    doc["battery"]    = batteryLevel;
    doc["rssi"]       = WiFi.RSSI();

    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    String topic = "caller/button/welder-01/status";
    if (client.publish(topic.c_str(), jsonBuffer)) {
        Serial.println("Health status published successfully");
    } else {
        Serial.println("Failed to publish health status");
    }

    client.disconnect();
}

void postButtonEvent(const char* assignedTo, const char* factoryZone) {
    begin();

    if (!client.connect(THINGNAME)) {
        Serial.println("TLS connection failed! Cannot publish button event.");
        return;
    }

    JsonDocument doc;
    doc["deviceType"]  = "caller-button";
    doc["welderId"]    = "welder-01";
    doc["assignedTo"]  = assignedTo;
    doc["factoryZone"] = factoryZone;
    doc["event"]       = "pressed";
    doc["timestamp"]   = currentTimestamp();

    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    String topic = "caller/button/welder-01/event";
    if (client.publish(topic.c_str(), jsonBuffer)) {
        Serial.println("Button event published successfully");
    } else {
        Serial.println("Failed to publish button event");
    }

    client.disconnect();
}