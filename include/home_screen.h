/**
 * @file home_screen.h
 * @brief Header file for home screen UI components
 */


#include <lvgl.h>

#ifdef __cplusplus
#endif

extern "C" {


#ifdef __cplusplus

/**
 * @brief Draws the main home screen with grid layout and navigation buttons
 */
void drawHomeScreen();

/**
 * @brief Draws the slide-up menu with navigation options
 */
void drawSlideUpMenu();

/**
 * @brief Syncs time with NTP server
 */
void syncTimeWithNTP();

/**
 * @brief Shows the home screen with NTP sync
 */
void showHomeScreenWithNtpSync();

} /*extern "C"*/
#endif