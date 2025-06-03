/**
 * @file settings_WIFI.h
 * @brief WiFi settings UI header for cydOS.
 *
 * Declares functions for WiFi settings user interface.
 */
#ifndef SETTINGS_WIFI_H
#define SETTINGS_WIFI_H

// Forward declaration to avoid including the full LVGL header
struct _lv_event_t;
typedef struct _lv_event_t lv_event_t;

/**
 * @brief Shows the WiFi settings UI.
 * @param e (Optional) LVGL event pointer. If nullptr, shows the settings directly.
 */
void showWiFiSettings(lv_event_t *e = nullptr);

#endif // SETTINGS_WIFI_H