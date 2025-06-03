/**
 * @file launcher.h
 * @brief Application launcher UI header for cydOS.
 *
 * Declares functions for launching and installing applications from SD card.
 */
#ifndef LAUNCHER_H
#define LAUNCHER_H

/**
 * @brief Shows the application launcher UI.
 */
void showLauncher();

/**
 * @brief Shows an error message in the launcher UI.
 * @param msg The error message to display.
 */
void showError(char const* msg);

#endif