#ifndef AWS_IOT_PUBLISHER_H
#define AWS_IOT_PUBLISHER_H

#define MQTT_HOST         "a36dnxxcnq6udp-ats.iot.us-east-2.amazonaws.com"        // Broker address
#define MQTT_PORT       8883 

#include <Arduino.h>
#include <time.h>
#include <ui.h>

void begin();
int postHealthStatus(int batteryLevel); // 95%? That's what we all claim to be at
int postButtonEvent(const char* assignedTo, const char* factoryZone);
String currentTimestamp();
void awsIotLoop();

#endif  // AWS_IOT_PUBLISHER_H
