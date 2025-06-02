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

// System Settings
void initSettings();
void showSettings(lv_event_t *e = nullptr);

// Display Settings
void saveBrightness(int brightness);
int loadBrightness();
void showDisplaySettings(lv_event_t *e = nullptr);

// Connectivity Settings  
void showConnectivity(lv_event_t *e = nullptr);

// Hardware Settings
void showSensorsSettings(lv_event_t *e = nullptr);
void showSDCardSettings(lv_event_t *e = nullptr);

// Data Management
void showBackupSettings(lv_event_t *e = nullptr);

#ifdef __cplusplus
}
#endif

#endif // SETTINGS_H
