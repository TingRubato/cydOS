/**
 * @file explorer.h
 * @brief File explorer UI interface for cydOS.
 *
 * Declares functions for directory navigation, file listing, and explorer navigation bar.
 */
#ifndef EXPLORER_H
#define EXPLORER_H

#include <TFT_eSPI.h>
#include <SdFat.h>
// Forward declaration for LVGL event type
struct _lv_event_t;
typedef struct _lv_event_t lv_event_t;

#include "SD_utils.h"
#include "ui.h" // Assuming ui.h includes the keyboard functionality

extern TFT_eSPI tft;
extern SdFat sd;
static bool is_initialized = false;
static char current_path[128] = "/"; // We all start at the root, if you know what I mean
/**
 * @brief Shows an error message on the display.
 * @param msg The error message to display.
 */
void showError(const char *msg);
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Shows the file explorer UI.
 * @param e LVGL event pointer (can be nullptr for initial call).
 */
void showFileExplorer(lv_event_t *e);

#ifdef __cplusplus
}
#endif
/**
 * @brief Handles directory navigation events in the explorer.
 * @param e LVGL event pointer.
 */
void dir_event_handler(lv_event_t *e);
/**
 * @brief Handles the event for creating a new directory.
 * @param e LVGL event pointer.
 */
void create_dir_event_handler(lv_event_t *e);
/**
 * @brief Confirms and processes the creation of a new directory.
 * @param e LVGL event pointer.
 */
void confirm_create_dir_event_handler(lv_event_t *e);
/**
 * @brief Navigates back to the parent directory in the explorer.
 * @param e LVGL event pointer.
 */
void back_to_parent_dir(lv_event_t *e);
/**
 * @brief Draws the navigation bar for the file explorer.
 */
void drawExplorerNavBar();

#endif // EXPLORER_H
