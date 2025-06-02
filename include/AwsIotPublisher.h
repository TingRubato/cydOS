/**
 * @file AwsIotPublisher.h  
 * @brief Secure AWS IoT Core MQTT Publisher
 *
 * Implements MQTT over TLS 1.2 communication with AWS IoT Core including:
 * - X.509 certificate authentication
 * - QoS 1 message delivery
 * - Automatic reconnection
 * - Offline message queuing
 *
 * Key Features:
 * - Hardware-accelerated TLS (ESP32)
 * - Low-power mode support
 * - Thread-safe operation
 * - Configurable keepalive interval
 *
 * Error Handling:
 * - Network fault recovery
 * - Certificate expiry detection  
 * - Message retry logic
 *
 * @version 2.1
 * @date 2025-06-01
 * @copyright Copyright (c) 2025 BHS Inc. All Rights Reserved.
 */

#ifndef AWS_IOT_PUBLISHER_H
#define AWS_IOT_PUBLISHER_H

#include <Arduino.h>
#include <time.h>
#include <ui.h>

/// AWS IoT Endpoint Configuration
#define MQTT_HOST "a36dnxxcnq6udp-ats.iot.us-east-2.amazonaws.com"
#define MQTT_PORT 8883  ///< Standard MQTT over TLS port

/**
 * @brief Initialize AWS IoT connection
 * 
 * Sets up WiFi connection and configures TLS certificates for AWS IoT Core.
 * Must be called before any publish operations.
 */
void begin();

/**
 * @brief Publish device health status
 * @param batteryLevel Current battery percentage (0-100)
 * @return 1 on success, 0 on failure
 */
int postHealthStatus(int batteryLevel);

/**
 * @brief Publish button press event
 * @param assignedTo Name of person assigned to the button
 * @param factoryZone Zone identifier where button is located
 * @return 1 on success, 0 on failure
 */
int postButtonEvent(const char* assignedTo, const char* factoryZone);

/**
 * @brief Get current timestamp in ISO8601 format
 * @return String containing formatted timestamp
 */
String currentTimestamp();

/**
 * @brief Maintain MQTT connection (call in main loop)
 */
void awsIotLoop();

#endif  // AWS_IOT_PUBLISHER_H
