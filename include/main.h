/**
 * @file main.h
 * @brief Hardware pin definitions and macros for cydOS (ESP32-2432S028R).
 *
 * Defines pin assignments for touchscreen, SD card, and onboard LEDs.
 * These macros are used throughout the firmware for hardware abstraction and clarity.
 *
 * - Touchscreen: XPT2046 controller
 * - SD Card: SPI interface
 * - LEDs: Status indicators
 */
#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

// Touchscreen (XPT2046) pin definitions
#define XPT2046_IRQ  36  ///< Touchscreen interrupt pin
#define XPT2046_MOSI 32  ///< Touchscreen SPI MOSI pin
#define XPT2046_MISO 39  ///< Touchscreen SPI MISO pin
#define XPT2046_CLK  25  ///< Touchscreen SPI clock pin
#define XPT2046_CS   33  ///< Touchscreen chip select pin

// SD Card pin definition
#define SD_CS        5   ///< SD card chip select pin

// Onboard LED pin definitions
#define LED_BLUE   17    ///< Blue status LED
#define LED_RED    4     ///< Red status LED
#define LED_GREEN  16    ///< Green status LED

#endif // MAIN_H