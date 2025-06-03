/**
 * @file settings.h
 * @brief Device Settings Management
 * 
 * Provides unified interface for:
 * - Persistent configuration storage
 * - Settings UI components
 * - Hardware parameter control
 *
 * Features:
 * - NVS-backed storage
 * - Thread-safe access
 * - Power-loss protection
 *
 * @version 2.0
 * @date 2025-06-01
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize system settings and persistent storage.
 *
 * Loads settings from NVS and prepares the settings subsystem.
 */
void initSettings();

/**
 * @brief Show the main settings UI.
 *
 * @param e (Optional) LVGL event pointer. If nullptr, shows the settings directly.
 */
void showSettings(lv_event_t *e = nullptr);

/**
 * @brief Save display brightness setting.
 *
 * @param brightness Brightness value to save (0-255).
 */
void saveBrightness(int brightness);

/**
 * @brief Load display brightness setting.
 *
 * @return The saved brightness value (0-255).
 */
int loadBrightness();

/**
 * @brief Show the display settings UI.
 *
 * @param e (Optional) LVGL event pointer. If nullptr, shows the settings directly.
 */
void showDisplaySettings(lv_event_t *e = nullptr);

/**
 * @brief Show the connectivity settings UI.
 *
 * @param e (Optional) LVGL event pointer. If nullptr, shows the settings directly.
 */
void showConnectivity(lv_event_t *e = nullptr);

/**
 * @brief Show the sensors settings UI.
 *
 * @param e (Optional) LVGL event pointer. If nullptr, shows the settings directly.
 */
void showSensorsSettings(lv_event_t *e = nullptr);

/**
 * @brief Show the SD card settings UI.
 *
 * @param e (Optional) LVGL event pointer. If nullptr, shows the settings directly.
 */
void showSDCardSettings(lv_event_t *e = nullptr);

/**
 * @brief Show the backup settings UI.
 *
 * @param e (Optional) LVGL event pointer. If nullptr, shows the settings directly.
 */
void showBackupSettings(lv_event_t *e = nullptr);

#ifdef __cplusplus
}
#endif

#endif // SETTINGS_H
