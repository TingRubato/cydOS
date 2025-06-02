/**
 * @file utils.h
 * @brief Hardware Abstraction Utilities for Embedded Systems
 * 
 * Provides low-level hardware control functions with:
 * - Minimal dependencies
 * - Atomic operations where needed  
 * - Hardware fault protection
 * - Power-aware implementations
 *
 * Key Features:
 * - Display control
 * - LED/GPIO management
 * - Power state handling
 *
 * Design Principles:
 * - No dynamic memory allocation
 * - Reentrant and thread-safe
 * - Minimal runtime overhead
 * - Hardware abstraction layer
 *
 * @version 1.2
 * @date 2025-06-01
 * @copyright Copyright (c) 2025 BHS Inc. All Rights Reserved.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Force display buffer flush
 * @note Blocks until flush completes
 * @warning Must be called from main thread
 */
void flushDisplay();

/**
 * @brief Enable all indicator LEDs at maximum brightness
 * @note Uses direct GPIO writes for lowest latency
 */
void torchON();

/** 
 * @brief Disable all indicator LEDs
 * @note Implements graceful power-down sequence
 */
void torchOFF();

/**
 * @brief Show system activity animation
 * @brief Shows posting progress animation
 * @note Uses non-blocking timers
 * @warning Must call flushDisplay() after
 */
void showPostingAnimation();

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */



void flushDisplay();
void torchON();
void torchOFF();
void showPostingAnimation();

#endif
