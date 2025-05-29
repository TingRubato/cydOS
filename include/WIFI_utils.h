#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <WiFi.h>
#include <vector> // Include vector header

void initWiFi();
void enableWiFi();
void disableWiFi();
bool isWiFiEnabled();
String getCurrentNetworkInfo();
std::vector<String> scanNetworks();
bool connectToNetwork(const char* ssid, const char* password);
void saveWiFiCredentials(const char* ssid, const char* password);
bool loadWiFiCredentials(const char* ssid, char* password, size_t maxLen);

// TLS-related function declarations
void setTLSCertificates(const char* caCert, const char* clientCert, const char* clientKey);
void setInsecureMode();
bool verifyCertificate();

#endif // WIFI_UTILS_H