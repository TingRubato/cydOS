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
 * @brief Handle launcher button press
 * @param e LVGL event object
 */
void launcher_event_handler(lv_event_t *e);

/**
 * @brief Handle file selection touch events 
 * @param e LVGL event object
 */
void touch_event_handler(lv_event_t *e);

/**
 * @brief Handle home navigation button
 * @param e LVGL event object
 */
void home_button_event_handler(lv_event_t *e);

/** 
 * @brief Control LED torch functionality
 * @param e LVGL event object
 */
void torch_event_handler(lv_event_t *e);

/**
 * @brief Handle file explorer launch
 * @param e LVGL event object 
 */
void explorer_event_handler(lv_event_t *e);

/**
 * @brief Show device info screen
 * @param e LVGL event object
 */
void info_event_handler(lv_event_t *e);

/**
 * @brief Show settings screen
 * @param e LVGL event object
 */
void settings_event_handler(lv_event_t *e);

#ifdef __cplusplus
}
#endif

#endif /* EVENT_HANDLERS_H */
