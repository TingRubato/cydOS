/**
 * @file ui.h
 * @brief User Interface Core Functions
 * 
 * Contains navigation bar management and UI initialization.
 * Designed for minimal memory usage and deterministic behavior.
 *
 * @note All functions must be called from main thread
 * @version 1.2
 * @date 2025-06-01
 */

#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reset navigation bar object pointers.
 *
 * Resets all internal pointers related to the navigation bar. Must be called before any UI initialization to ensure a clean state.
 *
 * @note Must be called before any UI initialization.
 */
void resetNavBarPointers();

/**
 * @brief Draw main navigation bar.
 *
 * Draws the main navigation bar using LVGL objects. This function should be called after LVGL has been initialized.
 *
 * @note Uses LVGL objects, must be called after LVGL init.
 */
void drawNavBar();

#ifdef __cplusplus
}
#endif

#endif /* UI_H */
