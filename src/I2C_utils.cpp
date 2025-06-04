/**
 * @file I2C_utils.cpp
 * @brief Implementation of I2C utility functions
 */

#include "I2C_utils.h"

bool initI2C() {
    Wire.begin(I2C_SDA, I2C_SCL, I2C_FREQ);
    return true;
}

void scanI2CDevices() {
    Serial.println("Scanning I2C bus...");
    byte error, address;
    int deviceCount = 0;

    for(address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(address, HEX);
            deviceCount++;
        }
    }

    if (deviceCount == 0) {
        Serial.println("No I2C devices found");
    } else {
        Serial.print("Found ");
        Serial.print(deviceCount);
        Serial.println(" device(s)");
    }
}

bool writeI2C(uint8_t address, const uint8_t* data, size_t length) {
    Wire.beginTransmission(address);
    size_t written = Wire.write(data, length);
    byte error = Wire.endTransmission();
    
    return (written == length && error == 0);
}

bool readI2C(uint8_t address, uint8_t* data, size_t length) {
    size_t bytesRead = Wire.requestFrom(address, length);
    
    if (bytesRead != length) {
        return false;
    }

    for (size_t i = 0; i < length; i++) {
        if (Wire.available()) {
            data[i] = Wire.read();
        } else {
            return false;
        }
    }
    
    return true;
} 