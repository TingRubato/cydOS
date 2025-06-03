/**
 * @file home_screen.h
 * @brief Home screen UI interface for cydOS.
 *
 * Declares functions for drawing the home screen, syncing NTP, and showing the home screen with NTP sync.
 */


#include <lvgl.h>

#ifdef __cplusplus
#endif

extern "C" {


#ifdef __cplusplus

/**
 * @brief Draws the main home screen with grid layout and navigation buttons.
 */
void drawHomeScreen();

void NewFunction(lv_obj_t *parent, bool &skipProcessing, int &retFlag);

/**
 * @brief Draws the slide-up menu with navigation options.
 */
void drawSlideUpMenu();

/**
 * @brief Syncs time with the NTP server.
 */
void syncTimeWithNTP();

/**
 * @brief Shows the home screen and synchronizes time with NTP.
 */
void showHomeScreenWithNtpSync();

} /*extern "C"*/
#endif