/**
 * @file launcher.cpp
 * @brief Implements the application launcher UI and logic for cydOS.
 *
 * Handles SD card app directory listing, app selection, and OTA installation logic.
 */
#include <TFT_eSPI.h>
#include <SdFat.h>
#include "lvgl.h"
#include "utils.h"
#include "event_handlers.h"
#include "ui.h"
#include "SD_utils.h"
#include "OTA_utils.h"
#include <stdlib.h>

extern TFT_eSPI tft;
extern SdFat sd;
static bool is_initialized = false;

void showError(const char *msg);
void showLauncher();
void install_event_handler(lv_event_t *e);
void confirm_install_event_handler(lv_event_t *e);

// Free user data callback
static void free_user_data_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_DELETE) {
        void *user_data = lv_obj_get_user_data(lv_event_get_target(e));
        if (user_data) free(user_data);
    }
}

void showError(const char *msg) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, msg);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t *back_btn = lv_btn_create(scr);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_align(back_btn, LV_ALIGN_CENTER, 0, 20);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_add_event_cb(back_btn, home_button_event_handler, LV_EVENT_CLICKED, NULL);
    // drawNavBar();
    Serial.println("Error displayed with navbar.");
}

void showLauncher() {
    static bool sd_ok = false;
    if (!is_initialized) {
        sd_ok = init_sd_card();
        if (!sd_ok) {
            Serial.println("SD init failed! Disabling launcher features.");
            // Optionally show a warning, but do not abort
            // showError("SD init failed!"); // Remove or comment out
            // return; // Remove or comment out
        }
        is_initialized = true;
    }

    if (!sd_ok) {
        // SD not available, show a warning or minimal UI
        lv_obj_t *scr = lv_scr_act();
        lv_obj_clean(scr);
        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_text(label, "SD card not available.\nApp launcher disabled.");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        return;
    }

    if (!check_and_create_dir("/apps")) {
        showError("Failed to create apps dir");
        return;
    }

    SdFile appsDir;
    if (!open_dir(appsDir, "/apps")) {
        showError("Failed to open apps directory");
        return;
    }

    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, 240, 280);
    lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);

    appsDir.rewind();
    SdFile entry;

    while (entry.openNext(&appsDir, O_RDONLY)) {
        if (entry.isDir()) {
            char dirName[64];
            entry.getName(dirName, sizeof(dirName));

            lv_obj_t *btn = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, dirName);
            char *dirNameCopy = strdup(dirName);
            lv_obj_set_user_data(btn, dirNameCopy);
            lv_obj_add_event_cb(btn, install_event_handler, LV_EVENT_CLICKED, dirNameCopy);
            lv_obj_add_event_cb(btn, free_user_data_event_cb, LV_EVENT_DELETE, NULL);
            Serial.printf("Created button for directory: %s\n", dirName);
        }
        entry.close();
    }

    appsDir.close();
    // drawNavBar();
    Serial.println("SD card directories listed!");
}

void install_event_handler(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    const char *dirName = (const char *)lv_obj_get_user_data(btn);

    Serial.printf("Selected directory: %s\n", dirName);

    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text_fmt(label, "Files in %s:", dirName);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *file_list = lv_list_create(scr);
    lv_obj_set_size(file_list, 240, 180);
    lv_obj_align(file_list, LV_ALIGN_CENTER, 0, 20);

    SdFile dir;
    char full_dir_path[128];
    snprintf(full_dir_path, sizeof(full_dir_path), "/apps/%s", dirName);

    if (!open_dir(dir, full_dir_path)) {
        showError("Failed to open selected directory!");
        return;
    }

    std::vector<FileInfo> files = list_files_in_dir(dir);
    for (const auto &file : files) {
        char file_info[128];
        snprintf(file_info, sizeof(file_info), "%s (%d KB)", file.name, file.size / 1024);
        lv_list_add_btn(file_list, NULL, file_info);
    }

    dir.close();

    lv_obj_t *yes_btn = lv_btn_create(scr);
    lv_obj_set_size(yes_btn, 80, 40);
    lv_obj_align(yes_btn, LV_ALIGN_CENTER, -60, 90);
    lv_obj_t *yes_label = lv_label_create(yes_btn);
    lv_label_set_text(yes_label, "Install");
    lv_obj_set_user_data(yes_btn, (void *)dirName);
    lv_obj_add_event_cb(yes_btn, confirm_install_event_handler, LV_EVENT_CLICKED, (void *)dirName);

    lv_obj_t *no_btn = lv_btn_create(scr);
    lv_obj_set_size(no_btn, 80, 40);
    lv_obj_align(no_btn, LV_ALIGN_CENTER, 60, 90);
    lv_obj_t *no_label = lv_label_create(no_btn);
    lv_label_set_text(no_label, "Cancel");
    lv_obj_add_event_cb(no_btn, home_button_event_handler, LV_EVENT_CLICKED, NULL);

    drawNavBar();
}

void confirm_install_event_handler(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    const char *dirName = (const char *)lv_obj_get_user_data(btn);

    Serial.printf("Confirmed install from directory: %s\n", dirName);

    xTaskCreatePinnedToCore(ota_task, "ota_task", 8192, (void *)dirName, 5, NULL, 1);
}