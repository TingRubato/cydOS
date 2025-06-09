/**
 * @file AwsIotPublisher.cpp
 * @brief Implements AWS IoT Core MQTT publishing for cydOS.
 *
 * Handles secure MQTT connection, device health, and button event publishing.
 */
#include "AwsIotPublisher.h"
#include "secrets.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ui.h>
#include "config.h"

WiFiClientSecure net;
PubSubClient client(net);

void begin() {
    Serial.begin(115200); // Wake up sleepyhead
    
    // Initialize WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    
    // Wait for WiFi connection with timeout
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20) {
        delay(500);
        Serial.print(".");
        timeout++;
    }
    Serial.println();
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection failed!");
        return;
    }
    
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());

    // Configure TLS
    net.setCACert(SIMPLE_IOT_ROOT_CA);
    net.setCertificate(SIMPLE_IOT_DEVICE_CERT);
    net.setPrivateKey(SIMPLE_IOT_DEVICE_PRIVATE_KEY);
    
    client.setServer(AWS_IOT_ENDPOINT, 8883);
    
    // Try to connect to AWS IoT with retries
    Serial.println("Connecting to AWS IoT endpoint via TLS...");
    int retries = 0;
    while (!client.connect(THINGNAME) && retries < 3) {
        Serial.print("TLS connection attempt ");
        Serial.print(retries + 1);
        Serial.println(" failed!");
        delay(1000);
        retries++;
    }
    
    if (client.connected()) {
        Serial.println("TLS connection succeeded!");
        client.disconnect();
    } else {
        Serial.println("All TLS connection attempts failed!");
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

int postHealthStatus(int batteryLevel) {
    begin();

    if (!client.connect(THINGNAME)) {
        Serial.println("TLS connection failed! Cannot publish health status.");
        return 0;
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

    String topic = g_config.topic_name + "/health";
    bool success = client.publish(topic.c_str(), jsonBuffer);
    if (success) {
        Serial.println("Health status published successfully");
    } else {
        Serial.println("Failed to publish health status");
    }

    client.disconnect();
    return success ? 1 : 0;
}

int postCallSupervisor(const char* supervisorId, const char* factoryZone, const char* reason) {
    begin();

    if (!client.connect(THINGNAME)) {
        Serial.println("TLS connection failed! Cannot publish supervisor call.");
        return 0;
    }

    JsonDocument doc;
    doc["deviceType"]    = "caller-button";
    doc["welderId"]      = "welder-01";
    doc["supervisorId"]  = supervisorId;
    doc["factoryZone"]   = factoryZone;
    doc["reason"]        = reason;
    doc["event"]         = "supervisor_call";
    doc["timestamp"]     = currentTimestamp();

    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    String topic = g_config.topic_name + "/supervisor-calls";
    bool success = client.publish(topic.c_str(), jsonBuffer);
    if (success) {
        Serial.println("Supervisor call published successfully");
    } else {
        Serial.println("Failed to publish supervisor call");
    }

    client.disconnect();
    return success ? 1 : 0;
}

int postCreateTicket(const char* ticketType, const char* priority, const char* description) {
    begin();

    if (!client.connect(THINGNAME)) {
        Serial.println("TLS connection failed! Cannot publish ticket creation.");
        return 0;
    }

    JsonDocument doc;
    doc["deviceType"]    = "caller-button";
    doc["welderId"]      = "welder-01";
    doc["ticketType"]    = ticketType;
    doc["priority"]      = priority;
    doc["description"]   = description;
    doc["event"]         = "ticket_created";
    doc["timestamp"]     = currentTimestamp();

    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    String topic = g_config.topic_name + "/tickets";
    bool success = client.publish(topic.c_str(), jsonBuffer);
    if (success) {
        Serial.println("Ticket creation published successfully");
    } else {
        Serial.println("Failed to publish ticket creation");
    }

    client.disconnect();
    return success ? 1 : 0;
}

int postButtonEvent(const char* assignedTo, const char* factoryZone) {
    begin();

    if (!client.connect(THINGNAME)) {
        Serial.println("TLS connection failed! Cannot publish button event.");
        return 0;
    }

    JsonDocument doc;
    doc["deviceType"] = "caller-button";
    doc["welderId"]   = "welder-01";
    doc["assignedTo"] = assignedTo;
    doc["factoryZone"] = factoryZone;
    doc["event"]      = "button_press";
    doc["timestamp"]  = currentTimestamp();

    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);

    String topic = g_config.topic_name + "/qa-queue";
    bool success = client.publish(topic.c_str(), jsonBuffer);
    if (success) {
        Serial.println("Button event published successfully");
    } else {
        Serial.println("Failed to publish button event");
    }

    client.disconnect();
    return success ? 1 : 0;
}
