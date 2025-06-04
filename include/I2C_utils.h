/**
 * @file I2C_utils.h
 * @brief I2C utility functions for cydOS
 * 
 * Provides functions for initializing and using I2C communication.
 */

#ifndef I2C_UTILS_H
#define I2C_UTILS_H

#include <Arduino.h>
#include <Wire.h>

// I2C pin definitions
#define I2C_SDA 22  ///< I2C SDA pin
#define I2C_SCL 23  ///< I2C SCL pin
#define I2C_FREQ 100000  ///< I2C frequency (100kHz)

/**
 * @brief Initialize I2C communication
 * 
 * Sets up I2C with default pins and frequency
 * @return true if initialization successful, false otherwise
 */
bool initI2C();

/**
 * @brief Scan for I2C devices
 * 
 * Scans the I2C bus and prints addresses of found devices
 */
void scanI2CDevices();

/**
 * @brief Write data to an I2C device
 * 
 * @param address I2C device address
 * @param data Pointer to data buffer
 * @param length Length of data buffer
 * @return true if write successful, false otherwise
 */
bool writeI2C(uint8_t address, const uint8_t* data, size_t length);

/**
 * @brief Read data from an I2C device
 * 
 * @param address I2C device address
 * @param data Pointer to data buffer
 * @param length Length of data buffer
 * @return true if read successful, false otherwise
 */
bool readI2C(uint8_t address, uint8_t* data, size_t length);

#endif // I2C_UTILS_H 