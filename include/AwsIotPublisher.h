#include <map>

/**
 * @file AwsIotPublisher.h  
 * @brief AWS IoT Core MQTT Publisher with unified topic structure
 *
 * Implements MQTT over TLS communication with AWS IoT Core including:
 * - X.509 certificate authentication
 * - Device info block in all messages
 * - Unified topic structure
 * - Automatic reconnection
 *
 * Key Features:
 * - Hardware-accelerated TLS (ESP32)
 * - Configurable device info
 * - Thread-safe operation
 *
 * Error Handling:
 * - Network fault recovery
 * - Connection retry logic
 *
 * @version 1.0
 * @date 2024-03-19
 */

#ifndef AWS_IOT_PUBLISHER_H
#define AWS_IOT_PUBLISHER_H

#include <Arduino.h>
#include <time.h>
#include <map>
#include <ArduinoJson.h>

/**
 * @brief Initialize AWS IoT connection.
 *
 * Sets up WiFi connection and configures TLS certificates for AWS IoT Core.
 * Must be called before any publish operations.
 */
void begin();

/**
 * @brief Get current timestamp in ISO8601 format.
 *
 * @return String containing formatted timestamp.
 */
String currentTimestamp();

/**
 * @brief Build the MQTT topic string.
 *
 * @return String containing the formatted topic.
 */
String buildTopic();

/**
 * @brief Publish an event with additional JSON data.
 *
 * @param functionName Name of the function/event.
 * @param extras Additional JSON data to include in the message.
 * @return true on success, false on failure.
 */
bool publishEvent(const char* functionName, JsonObject& extras);

/**
 * @brief Publish an event with standard parameters.
 *
 * @param label Human-readable label (e.g., "QA Inspection")
 * @param eventType Internal type (e.g., "inspection", "ticket")
 * @param department Department identifier (e.g., "welding")
 * @param stationId Station identifier
 * @param extras Optional map of additional key-value pairs
 * @return true on success, false on failure.
 */
bool publishEvent(
    const String& label,           // Human-readable label (e.g., "QA Inspection")
    const String& eventType,       // Internal type (e.g., "inspection", "ticket")
    const String& department,      // e.g., "welding"
    int stationId,                 // e.g., 1
    std::map<String, String> extras // Optional extra info like priority or notes
);

/**
 * @brief Publish a heartbeat message.
 *
 * @return true on success, false on failure.
 */
bool publishHeartbeat();

#endif  // AWS_IOT_PUBLISHER_H
