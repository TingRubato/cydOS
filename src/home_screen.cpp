/**
 * @file home_screen.cpp
 * @brief Implements the main home screen UI and event logic for cydOS.
 *
 * Handles the main tabbed interface, button event processing, and NTP time synchronization.
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <lvgl.h>
#include "home_screen.h"
#include "event_handlers.h"
#include "utils.h"
#include <Arduino.h>
#include "settings.h"
#include "SD_utils.h"
#include <time.h>
#include <string.h>
#include "bhs.h"
#include <NTPClient.h>
#include "ui.h"
#include "WIFI_utils.h"
#include "AwsIotPublisher.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "config.h"

QueueHandle_t buttonQueue = NULL; // Define the queue here for use in this file and others

// Global variables for NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
struct tm timeinfo;

// Store popup box and label globally for status update
static lv_obj_t *g_status_box = NULL;
static lv_obj_t *g_status_label = NULL;

// Task handle for button event processing
static TaskHandle_t buttonEventTaskHandle = NULL;

void buttonEventTask(void *pvParameters) {
    int buttonEvent;
    while (1) {
        if (xQueueReceive(buttonQueue, &buttonEvent, portMAX_DELAY) == pdTRUE) {
            bool success = false;
            switch (buttonEvent) {
                case 0:
                    success = publishEvent(String("QA Inspection"), String("inspection"), String("welding"), 1, std::map<String, String>{});
                    break;
                case 1:
                    success = publishEvent(String("Supervisor Call"), String("supervisor_call"), String("welding"), 1, std::map<String, String>{});
                    break;
                case 2:
                    success = publishEvent(String("Create Ticket"), String("ticket"), String("welding"), 1, std::map<String, String>{{"priority", "medium"}, {"note", "Equipment requires maintenance"}});
                    break;
                case 3:
                    success = publishEvent(String("Health Check"), String("health_status"), String("welding"), 1, std::map<String, String>{});
                    break;
                default:
                    break;
            }
            // Directly update status label and schedule cleanup
            if (g_status_label && g_status_box) {
                if (success) {
                    lv_label_set_text(g_status_label, "Success!");
                    lv_obj_set_style_text_color(g_status_label, lv_palette_main(LV_PALETTE_GREEN), 0); // Green
                } else {
                    lv_label_set_text(g_status_label, "Failed!");
                    lv_obj_set_style_text_color(g_status_label, lv_palette_main(LV_PALETTE_RED), 0); // Red
                }
                // Cleanup after 1 sec and refresh screen
                lv_timer_t *cleanup_timer = lv_timer_create([](lv_timer_t *ctimer) {
                    lv_obj_del((lv_obj_t *)ctimer->user_data);
                    lv_timer_del(ctimer);
                    g_status_box = NULL;
                    g_status_label = NULL;
                    lv_obj_invalidate(lv_scr_act()); // Refresh the active screen
                }, 1000, g_status_box);
            }
        }
    }
}

void ensureButtonQueueAndTask() {
    if (!buttonQueue) {
        buttonQueue = xQueueCreate(10, sizeof(int));
    }
    if (!buttonEventTaskHandle) {
        xTaskCreatePinnedToCore(buttonEventTask, "ButtonEventTask", 12288, NULL, 2, &buttonEventTaskHandle, 1);
    }
}

static void fourth_tab_btnm_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
        if (dsc->class_p == &lv_btnmatrix_class && dsc->type == LV_BTNMATRIX_DRAW_PART_BTN) {
            if (dsc->id == 0) { // Call QA
                dsc->rect_dsc->radius = 0;
                dsc->rect_dsc->bg_color = (lv_btnmatrix_get_selected_btn(obj) == dsc->id) ? lv_palette_darken(LV_PALETTE_BLUE, 3) : lv_palette_main(LV_PALETTE_BLUE);
                dsc->rect_dsc->shadow_width = 6;
                dsc->rect_dsc->shadow_ofs_x = 3;
                dsc->rect_dsc->shadow_ofs_y = 3;
                dsc->label_dsc->color = lv_color_white();
            }
            else if (dsc->id == 1) { // Call Tech
                dsc->rect_dsc->radius = LV_RADIUS_CIRCLE;
                dsc->rect_dsc->bg_color = (lv_btnmatrix_get_selected_btn(obj) == dsc->id) ? lv_palette_darken(LV_PALETTE_RED, 3) : lv_palette_main(LV_PALETTE_RED);
                dsc->label_dsc->color = lv_color_white();
            }
            else if (dsc->id == 2) { // Create Support Ticket
                dsc->rect_dsc->radius = 8;
                dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_GREEN);
                dsc->label_dsc->color = lv_color_white();
            }
        }
    }
    if (code == LV_EVENT_VALUE_CHANGED) {
        uint16_t btn_id = lv_btnmatrix_get_selected_btn(obj);
        const char *txt = lv_btnmatrix_get_btn_text(obj, btn_id);
        if (txt) {
            LV_LOG_USER("Button pressed: %s", txt);
        }
    }
}

static void draw_part_event_cb(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->part == LV_PART_ITEMS) {
        uint32_t row = dsc->id / lv_table_get_col_cnt(obj);
        uint32_t col = dsc->id - row * lv_table_get_col_cnt(obj);

        if(row == 0) {
            dsc->label_dsc->align = LV_TEXT_ALIGN_CENTER;
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), dsc->rect_dsc->bg_color, LV_OPA_20);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
        else if(col == 0) {
            dsc->label_dsc->align = LV_TEXT_ALIGN_RIGHT;
        }

        if((row != 0 && row % 2) == 0) {
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), dsc->rect_dsc->bg_color, LV_OPA_10);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
    }
}

void syncTimeWithNTP() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, skipping NTP sync.");
        return;
    }
    Serial.println("Syncing time with NTP server...");
    timeClient.begin();
    timeClient.update();

    time_t localEpoch = timeClient.getEpochTime() - 5 * 3600; // -5 hours to represent CDT
    localtime_r(&localEpoch, &timeinfo);

    struct timeval tv = {
        .tv_sec = localEpoch,
        .tv_usec = 0
    };
    settimeofday(&tv, NULL);

    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
    Serial.println("Current time: " + String(timeStr));
}

String getCurrentTimeString() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, skipping NTP sync in getCurrentTimeString.");
        return "N/A";
    }
    Serial.println("Syncing time with NTP server...");

    timeClient.begin();
    timeClient.update();

    time_t localEpoch = timeClient.getEpochTime() - 5 * 3600; // -5 hours to represent CDT
    localtime_r(&localEpoch, &timeinfo);

    struct timeval tv = {
        .tv_sec = localEpoch,
        .tv_usec = 0};
    settimeofday(&tv, NULL);

    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
    return String(timeStr);
}
void drawHomeScreen() {
    bool sd_ok = init_sd_card();
    if (!sd_ok) {
        Serial.println("SD card init failed, continuing without SD features.");
        // Optionally, show a warning on the UI here
    }
    Serial.println("Initializing WiFi...");
    
    // Clear screen once
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // Create tabview
    lv_obj_t *tabview = lv_tabview_create(scr, LV_DIR_TOP, 50);
    lv_obj_set_size(tabview, 240, 270);
    lv_obj_align(tabview, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(tabview, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 2), 0);

    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS | LV_STATE_CHECKED);

    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Info");
    lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "Buttons");

    // Info Tab Content
    lv_obj_t *wifi_label = lv_label_create(tab1);
    String wifiInfo = "WiFi: Connected";
    lv_label_set_text(wifi_label, wifiInfo.c_str());
    lv_label_set_long_mode(wifi_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_color(wifi_label, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_align(wifi_label, LV_ALIGN_TOP_RIGHT, -5, 5);

    lv_obj_t *table = lv_table_create(tab1);
    lv_table_set_col_width(table, 0, 80);
    lv_table_set_col_width(table, 1, 140);
    lv_obj_set_style_text_font(table, &lv_font_montserrat_10, LV_PART_MAIN);
    
    // Fill table with device info
    lv_table_set_cell_value(table, 0, 0, "Property");
    lv_table_set_cell_value(table, 0, 1, "Value");
    lv_table_set_cell_value(table, 1, 0, "Location");
    lv_table_set_cell_value(table, 1, 1, g_config.location.c_str());
    lv_table_set_cell_value(table, 2, 0, "Station");
    lv_table_set_cell_value(table, 2, 1, g_config.stationId.c_str());
    lv_table_set_cell_value(table, 3, 0, "Time");
    lv_table_set_cell_value(table, 3, 1, getCurrentTimeString().c_str());
    lv_table_set_cell_value(table, 4, 0, "Status");
    lv_table_set_cell_value(table, 4, 1, "Active");
    lv_table_set_cell_value(table, 5, 0, "IP Address");
    lv_table_set_cell_value(table, 5, 1, WiFi.localIP().toString().c_str());
    
    lv_obj_set_size(table, 240, 250);
    lv_obj_align(table, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(table, draw_part_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
  
    // Buttons Tab Content
    static const char *btnm_map[] = {"Call QA Inspection", "\n", "Call Supervisor", "\n", "Maintenance Ticket", NULL};
    lv_obj_t *btnm = lv_btnmatrix_create(tab4);
    lv_btnmatrix_set_map(btnm, btnm_map);
    lv_obj_set_size(btnm, 200, 200);
    lv_obj_align(btnm, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(btnm, [](lv_event_t *e) {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t *obj = lv_event_get_target(e);

        if (code == LV_EVENT_CLICKED) {
            uint16_t btn_id = lv_btnmatrix_get_selected_btn(obj);
            lv_obj_t *parent = lv_scr_act();

            // Create popup box
            lv_obj_t *box = lv_obj_create(parent);
            lv_obj_set_size(box, 120, 120);
            lv_obj_align(box, LV_ALIGN_CENTER, 0, 0);
            lv_obj_set_style_bg_color(box, lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_border_width(box, 1, 0);
            lv_obj_set_style_radius(box, 10, 0);

            // Spinner inside box
            lv_obj_t *spinner = lv_spinner_create(box, 1000, 20);
            lv_obj_set_size(spinner, 50, 50);
            lv_obj_align(spinner, LV_ALIGN_CENTER, 0, -10);

            // Status label inside box
            lv_obj_t *status_label = lv_label_create(box);
            lv_label_set_text(status_label, "Sending event...");
            lv_obj_set_style_text_color(status_label, lv_color_hex(0x0000FF), 0);
            lv_obj_align(status_label, LV_ALIGN_CENTER, 0, 30);

            g_status_box = box;
            g_status_label = status_label;

            bool success = false;
            switch (btn_id) {
                case 0: // QA
                    success = publishEvent(String("QA Inspection"), String("inspection"), String("welding"), 1, std::map<String, String>{});
                    break;
                case 1: // Supervisor
                    success = publishEvent(String("Supervisor Call"), String("supervisor_call"), String("welding"), 1, std::map<String, String>{});
                    break;
                case 2: // Ticket
                    success = publishEvent(String("Create Ticket"), String("ticket"), String("welding"), 1, std::map<String, String>{{"priority", "medium"}, {"note", "Equipment requires maintenance"}});
                    break;
                default:
                    lv_obj_del(box);
                    g_status_box = NULL;
                    g_status_label = NULL;
                    return;
            }

            // Update status based on success
            if (success) {
                lv_label_set_text(status_label, "Success!");
                lv_obj_set_style_text_color(status_label, lv_palette_main(LV_PALETTE_GREEN), 0);
            } else {
                lv_label_set_text(status_label, "Failed!");
                lv_obj_set_style_text_color(status_label, lv_palette_main(LV_PALETTE_RED), 0);
            }

            // Remove spinner
            lv_obj_del(spinner);

            // Cleanup after 2 seconds
            lv_timer_t *timer = lv_timer_create([](lv_timer_t *timer) {
                if (g_status_box) {
                    lv_obj_del(g_status_box);
                    g_status_box = NULL;
                    g_status_label = NULL;
                }
                lv_timer_del(timer);
            }, 2000, NULL);
        }
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(btnm, fourth_tab_btnm_event_cb, LV_EVENT_ALL, NULL);

    // Sync time with NTP server
    syncTimeWithNTP();
    lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);
    drawNavBar();

    ensureButtonQueueAndTask();
}

void showHomeScreenWithNtpSync() {
    syncTimeWithNTP();
    drawHomeScreen();
}
