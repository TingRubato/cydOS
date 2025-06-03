/**
 * @file event_handlers.h
 * @brief LVGL Event Handlers for User Interface
 * 
 * Contains all event callback functions for UI interactions.
 * Designed for minimal memory footprint and thread safety.
 * 
 * @note All handlers must be non-blocking and reentrant
 * @version 1.1
 * @date 2025-06-01
 */

#ifndef EVENT_HANDLERS_H
#define EVENT_HANDLERS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handle launcher button press event.
 *
 * Processes the event when the launcher button is pressed in the UI.
 *
 * @param e LVGL event object pointer.
 */
void launcher_event_handler(lv_event_t *e);

/**
 * @brief Handle file selection touch events.
 *
 * Processes touch events for file selection in the UI.
 *
 * @param e LVGL event object pointer.
 */
void touch_event_handler(lv_event_t *e);

/**
 * @brief Handle home navigation button event.
 *
 * Processes the event when the home navigation button is pressed.
 *
 * @param e LVGL event object pointer.
 */
void home_button_event_handler(lv_event_t *e);

/**
 * @brief Control LED torch functionality.
 *
 * Handles events related to toggling the LED torch.
 *
 * @param e LVGL event object pointer.
 */
void torch_event_handler(lv_event_t *e);

/**
 * @brief Handle file explorer launch event.
 *
 * Processes the event to launch the file explorer UI.
 *
 * @param e LVGL event object pointer.
 */
void explorer_event_handler(lv_event_t *e);

/**
 * @brief Show device info screen event.
 *
 * Handles the event to display the device information screen.
 *
 * @param e LVGL event object pointer.
 */
void info_event_handler(lv_event_t *e);

/**
 * @brief Show settings screen event.
 *
 * Handles the event to display the settings screen.
 *
 * @param e LVGL event object pointer.
 */
void settings_event_handler(lv_event_t *e);

#ifdef __cplusplus
}
#endif

#endif /* EVENT_HANDLERS_H */
