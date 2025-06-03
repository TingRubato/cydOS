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
 * @brief Force display buffer flush.
 *
 * Blocks until the flush operation completes. This ensures that all pending display updates are rendered.
 *
 * @note Must be called from the main thread to avoid concurrency issues.
 */
void flushDisplay();

/**
 * @brief Enable all indicator LEDs at maximum brightness.
 *
 * Uses direct GPIO writes for lowest latency. Useful for signaling or debugging.
 */
void torchON();

/**
 * @brief Disable all indicator LEDs.
 *
 * Implements a graceful power-down sequence for the LEDs to avoid abrupt transitions.
 */
void torchOFF();

/**
 * @brief Show system activity animation (posting progress).
 *
 * Uses non-blocking timers to animate the display, indicating that a posting operation is in progress.
 *
 * @note Must call flushDisplay() after to ensure the animation is rendered.
 */
void showPostingAnimation();

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */
