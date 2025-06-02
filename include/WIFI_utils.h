/**
 * @file WIFI_utils.h
 * @brief WiFi Connection Management Utilities
 * 
 * Provides functions for WiFi network scanning, connection management,
 * and credential storage/retrieval from persistent storage.
 * 
 * @author Your Name
 * @date 2025-06-01
 * @version 1.0
 */

#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <WiFi.h>
#include <vector>
#include <WString.h>  // For String class

/**
 * @brief Initialize WiFi hardware
 * 
 * Sets WiFi mode to STA (Station) and disconnects any existing connection.
 */
void initWiFi();

/**
 * @brief Enable WiFi radio
 */
void enableWiFi();

/**
 * @brief Disable WiFi radio
 */
void disableWiFi();

/**
 * @brief Check if WiFi is enabled
 * @return true if WiFi is enabled, false otherwise
 */
bool isWiFiEnabled();

/**
 * @brief Get current network connection info
 * @return Formatted string with SSID, IP, RSSI etc.
 */
String getCurrentNetworkInfo();

/**
 * @brief Scan for available WiFi networks
 * @return Vector of SSID strings
 */
/**
 * @brief Scan for available WiFi networks
 * @param[out] networks Array to store found SSIDs (max 20)
 * @return Number of networks found
 */
uint8_t scanNetworks(String networks[], uint8_t maxNetworks);

/**
 * @brief Connect to specified WiFi network
 * @param ssid Network SSID
 * @param password Network password
 * @return true if connection succeeded, false otherwise
 * @note Prints SSID/password to Serial for debugging special characters
 */
bool connectToNetwork(const char* ssid, const char* password);

/**
 * @brief Save WiFi credentials to persistent storage
 * @param ssid Network SSID to save
 * @param password Network password to save
 */
void saveWiFiCredentials(const char* ssid, const char* password);

/**
 * @brief Load WiFi password for given SSID
 * @param ssid Network SSID to lookup
 * @param password Output buffer for password
 * @param maxLen Maximum length of password buffer
 * @return true if credentials were found, false otherwise
 */
bool loadWiFiCredentials(const char* ssid, char* password, size_t maxLen);

// TLS-related functions
/**
 * @brief Configure TLS certificates for secure connections
 * @param caCert CA certificate in PEM format
 * @param clientCert Client certificate in PEM format
 * @param clientKey Client private key in PEM format
 */
void setTLSCertificates(const char* caCert, const char* clientCert, const char* clientKey);

/**
 * @brief Disable certificate verification (insecure)
 * @warning Only use for testing/debugging
 */
void setInsecureMode();

/**
 * @brief Verify server certificate
 * @return true if certificate is valid, false otherwise
 */
bool verifyCertificate();

#endif // WIFI_UTILS_H
