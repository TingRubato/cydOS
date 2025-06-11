/**
 * @file AwsIotPublisher.cpp
 * @brief Updated AWS IoT Core MQTT publisher with unified topic structure and device info block.
 */

#include "secrets.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "AwsIotPublisher.h"
#include "config.h"

WiFiClientSecure net;
PubSubClient client(net);

// Helper to ensure MQTT connection is alive
bool ensureMqttConnected() {
    if (!client.connected()) {
        if (!client.connect(THINGNAME)) {
            Serial.println("TLS connection failed! Cannot publish event.");
            Serial.print("MQTT connect state: ");
            Serial.println(client.state());
            return false;
        }
    }
    return true;
}

void begin() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");

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

    net.setCACert(SIMPLE_IOT_ROOT_CA);
    net.setCertificate(SIMPLE_IOT_DEVICE_CERT);
    net.setPrivateKey(SIMPLE_IOT_DEVICE_PRIVATE_KEY);

    client.setServer(AWS_IOT_ENDPOINT, 8883);
    client.setCallback(NULL);  // No callback needed for publish-only client
    client.setKeepAlive(60);  // 60 second keepalive
    client.setSocketTimeout(10);  // 10 second socket timeout
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

String buildTopic() {
    return "iot/" + g_config.department + "/" + g_config.stationId + "/" + g_config.deviceId;
}

/**
 * @brief Publishes an event to AWS IoT Core with device information and custom data.
 * 
 * This function handles the following steps:
 * 1. Initializes the connection by calling begin()
 * 2. Establishes MQTT connection to AWS IoT Core
 * 3. Creates a JSON document with:
 *    - Device information (ID, department, station, location, firmware version)
 *    - Event details (type, function name, timestamp)
 *    - Any additional data passed in the extras parameter
 * 4. Serializes the JSON to a buffer
 * 5. Publishes the message to the configured MQTT topic
 * 6. Handles connection cleanup
 * 
 * @param functionName The name of the function/event being published
 * @param extras Additional JSON data to include in the message
 * @return true if publish was successful, false otherwise
 */
bool publishEvent(const char* functionName, JsonObject& extras) {
    if (!ensureMqttConnected()) {
        return false;
    }

    // Create JSON document with device information
    JsonDocument doc;
    JsonObject deviceInfo = doc["deviceInfo"].to<JsonObject>();
    deviceInfo["deviceId"] = g_config.deviceId;
    deviceInfo["department"] = g_config.department;
    deviceInfo["stationId"] = g_config.stationId;
    deviceInfo["location"] = g_config.location;
    deviceInfo["firmwareVersion"] = g_config.firmwareVersion;

    // Add event-specific information
    doc["event"] = "pressed";
    doc["function"] = functionName;
    doc["timestamp"] = currentTimestamp();

    // Add any additional data from extras parameter
    for (JsonPair kv : extras) {
        doc[kv.key()] = kv.value();
    }

    // Serialize JSON to buffer
    char jsonBuffer[1024];
    serializeJson(doc, jsonBuffer);

    // Get topic and publish message
    String topic = buildTopic();
    Serial.print("Publishing to topic: ");
    Serial.println(topic);
    Serial.print("Payload: ");
    Serial.println(jsonBuffer);
    Serial.print("Payload length: ");
    Serial.println(strlen(jsonBuffer));
    
    // Attempt to publish and handle result
    bool success = client.publish(topic.c_str(), jsonBuffer);
    Serial.print("Publish result: ");
    Serial.println(success ? "success" : "failure");
    if (!success) {
        Serial.print("MQTT client state after publish: ");
        Serial.println(client.state());
    }
    if (success) {
        Serial.println("Event published successfully");
    } else {
        Serial.println("Failed to publish event");
    }

    return success;
}

bool publishEvent(const String& label,
                  const String& eventType,
                  const String& department,
                  int stationId,
                  std::map<String, String> extras)
{
    Serial.print("MQTT client state before connect: ");
    Serial.println(client.state());
    if (!ensureMqttConnected()) {
        return false;
    }
    Serial.print("MQTT client state after connect: ");
    Serial.println(client.state());

    StaticJsonDocument<2048> doc;
    doc["eventType"] = eventType;
    doc["label"] = label;
    doc["department"] = department;
    doc["stationId"] = String(stationId);
    doc["location"] = g_config.location;
    doc["timestamp"] = currentTimestamp();

    for (const auto& kv : extras) {
        doc[kv.first] = kv.second;
    }

    String payload;
    serializeJson(doc, payload);
    Serial.print("Payload length: ");
    Serial.println(payload.length());

    String topic = "bhs/events/" + g_config.location + "/" + department + "/" + String(stationId);
    Serial.print("Publishing to topic: ");
    Serial.println(topic);
    Serial.print("Payload: ");
    Serial.println(payload);
    bool success = client.publish(topic.c_str(), payload.c_str());
    Serial.print("Publish result: ");
    Serial.println(success ? "success" : "failure");
    Serial.print("MQTT client state after publish: ");
    Serial.println(client.state());
    if (!success) {
        Serial.println("Trying test publish to 'test/topic'...");
        bool test = client.publish("test/topic", "{\"test\":1}");
        Serial.print("Test publish result: ");
        Serial.println(test ? "success" : "failure");
        Serial.print("MQTT client state after test publish: ");
        Serial.println(client.state());
    }
    if (success) {
        Serial.println("Event published successfully");
    } else {
        Serial.println("Failed to publish event");
    }

    return success;
}

bool publishHeartbeat() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, attempting to connect before heartbeat publish...");
        WiFi.begin(g_config.wifi_ssid.c_str(), g_config.wifi_password.c_str());
        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) { // 10s timeout
            delay(500);
            Serial.print(".");
        }
        Serial.println();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Failed to connect to WiFi, skipping heartbeat publish.");
            return false;
        } else {
            Serial.println("WiFi connected for heartbeat publish.");
        }
    }
    if (!ensureMqttConnected()) {
        return false;
    }
    Serial.print("MQTT client state after connect: ");
    Serial.println(client.state());
    
    JsonDocument doc;
    doc["deviceId"] = g_config.deviceId;
    doc["timestamp"] = currentTimestamp();
    doc["firmwareVersion"] = g_config.firmwareVersion;
    doc["status"] = "online";
    doc["rssi"] = WiFi.RSSI();
    doc["uptime"] = millis() / 1000;

    String payload;
    serializeJson(doc, payload);

    String topic = "bhs/heartbeat/" + g_config.deviceId;
    Serial.print("Publishing heartbeat to topic: ");
    Serial.println(topic);
    Serial.print("Payload: ");
    Serial.println(payload);

    bool success = client.publish(topic.c_str(), payload.c_str());
    if (success) {
        Serial.println("Heartbeat published successfully");
    } else {
        Serial.println("Failed to publish heartbeat");
    }
    return success;
}

// Add this function for use in main loop or a task
void mqttLoop() {
    client.loop();
}