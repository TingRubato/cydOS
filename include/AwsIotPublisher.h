#ifndef AWS_IOT_PUBLISHER_H
#define AWS_IOT_PUBLISHER_H

#define MQTT_HOST         "a36dnxxcnq6udp-ats.iot.us-east-2.amazonaws.com"        // Broker address
#define MQTT_PORT       8883 

#include <Arduino.h>
#include <time.h>

void begin();
void postHealthStatus(int batteryLevel = 95);
void postButtonEvent(const char* assignedTo = "default-user", const char* factoryZone = "default-zone");
String currentTimestamp();
void awsIotLoop();

#endif  // AWS_IOT_PUBLISHER_H