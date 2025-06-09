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

// Function to update popup status from background task
void update_popup_status(void *success_ptr) {
    bool success = (bool)(intptr_t)success_ptr;
    if (g_status_label && g_status_box) {
        if (success) {
            lv_label_set_text(g_status_label, "Success!");
            lv_obj_set_style_text_color(g_status_label, lv_color_hex(0x00FF00), 0); // Green
        } else {
            lv_label_set_text(g_status_label, "Failed!");
            lv_obj_set_style_text_color(g_status_label, lv_color_hex(0xFF0000), 0); // Red
        }
        // Cleanup after 1 sec
        lv_timer_t *cleanup_timer = lv_timer_create([](lv_timer_t *ctimer) {
            lv_obj_del((lv_obj_t *)ctimer->user_data);
            lv_timer_del(ctimer);
            g_status_box = NULL;
            g_status_label = NULL;
        }, 1000, g_status_box);
    }
}

// Task handle for button event processing
static TaskHandle_t buttonEventTaskHandle = NULL;

// Button event processing task
void buttonEventTask(void *pvParameters) {
    int buttonEvent;
    while (1) {
        if (xQueueReceive(buttonQueue, &buttonEvent, portMAX_DELAY) == pdTRUE) {
            bool success = false;
            if (buttonEvent == 0) { // QA
                success = postButtonEvent("QA", "Zone1");
            } else if (buttonEvent == 1) { // Tech
                success = postButtonEvent("Tech", "Zone1");
            } else if (buttonEvent == 3) { // Health
                success = postHealthStatus(100);
            }
            lv_async_call(update_popup_status, (void*)(intptr_t)success);
        }
    }
}

// In a function that runs at startup (e.g. after LVGL init, or in drawHomeScreen if needed):
// Ensure the queue is created and the task is started if not already
void ensureButtonQueueAndTask() {
    if (!buttonQueue) {
        buttonQueue = xQueueCreate(10, sizeof(int));
    }
    if (!buttonEventTaskHandle) {
        xTaskCreatePinnedToCore(buttonEventTask, "ButtonEventTask", 12288, NULL, 2, &buttonEventTaskHandle, 1);
    }
}

static void fourth_tab_btnm_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_DRAW_PART_BEGIN)
    {
        lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
        if (dsc->class_p == &lv_btnmatrix_class && dsc->type == LV_BTNMATRIX_DRAW_PART_BTN)
        {
            if (dsc->id == 0)
            { // Call QA
                dsc->rect_dsc->radius = 0;
                dsc->rect_dsc->bg_color = (lv_btnmatrix_get_selected_btn(obj) == dsc->id) ? lv_palette_darken(LV_PALETTE_BLUE, 3) : lv_palette_main(LV_PALETTE_BLUE);
                dsc->rect_dsc->shadow_width = 6;
                dsc->rect_dsc->shadow_ofs_x = 3;
                dsc->rect_dsc->shadow_ofs_y = 3;
                dsc->label_dsc->color = lv_color_white();
            }
            else if (dsc->id == 1)
            { // Call Tech
                dsc->rect_dsc->radius = LV_RADIUS_CIRCLE;
                dsc->rect_dsc->bg_color = (lv_btnmatrix_get_selected_btn(obj) == dsc->id) ? lv_palette_darken(LV_PALETTE_RED, 3) : lv_palette_main(LV_PALETTE_RED);
                dsc->label_dsc->color = lv_color_white();
            }
            else if (dsc->id == 2)
            { // Create Support Ticket
                dsc->rect_dsc->radius = 8;
                dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_GREEN);
                dsc->label_dsc->color = lv_color_white();
            }
        }
    }
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t btn_id = lv_btnmatrix_get_selected_btn(obj);
        const char *txt = lv_btnmatrix_get_btn_text(obj, btn_id);
        if (txt)
        {
            LV_LOG_USER("Button pressed: %s", txt);
        }
    }
}

static void draw_part_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    /*If the cells are drawn...*/
    if(dsc->part == LV_PART_ITEMS) {
        uint32_t row = dsc->id /  lv_table_get_col_cnt(obj);
        uint32_t col = dsc->id - row * lv_table_get_col_cnt(obj);

        /*Make the texts in the first cell center aligned*/
        if(row == 0) {
            dsc->label_dsc->align = LV_TEXT_ALIGN_CENTER;
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), dsc->rect_dsc->bg_color, LV_OPA_20);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
        /*In the first column align the texts to the right*/
        else if(col == 0) {
            dsc->label_dsc->align = LV_TEXT_ALIGN_RIGHT;
        }

        /*MAke every 2nd row grayish*/
        if((row != 0 && row % 2) == 0) {
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), dsc->rect_dsc->bg_color, LV_OPA_10);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
    }
}


void syncTimeWithNTP() // Because even devices need to know what time their bedtime is
{
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
        .tv_usec = 0};
    settimeofday(&tv, NULL);

    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
    Serial.println("Current time: " + String(timeStr));
}

String getCurrentTimeString()
{
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

void drawHomeScreen()
{
    if (!init_sd_card()) {
        Serial.println("SD card init failed, aborting home screen draw.");
        return;
    }
    Serial.println("Initializing WiFi...");
    // const char *ssid = "SpectrumSetup-21";
    // const char *password = "vasttooth431";
    // Serial.printf("Loading saved password for SSID: %s\n", ssid);
    // Serial.printf("connecting using password: %s\n", password);

    // 只清理一次 screen
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // WiFi连接失败直接return，避免后续UI操作
    // if (!connectToNetwork(ssid, password)) {
    //     Serial.println("Connection failed");
    //     lv_obj_t *label = lv_label_create(scr);
    //     lv_label_set_text(label, "Failed to connect to WiFi");
    //     lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    //     return;
    // }
    // Serial.println("Connection successful");

    // 创建 tabview
    lv_obj_t *tabview = lv_tabview_create(scr, LV_DIR_TOP, 50);
    lv_obj_set_size(tabview, 240, 270);
    lv_obj_align(tabview, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(tabview, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 2), 0);

    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS | LV_STATE_CHECKED);

    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Info");
    // lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Settings");
    // lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Mistery");
    lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "Buttons");
    // Info Tab Content
    // Add WiFi status as a small label on the top right corner
    lv_obj_t *wifi_label = lv_label_create(tab1);
    String wifiInfo = "WiFi: Connected";
    lv_label_set_text(wifi_label, wifiInfo.c_str());
    lv_label_set_long_mode(wifi_label, LV_LABEL_LONG_SCROLL_CIRCULAR); // Enable rolling (scrolling) text
    lv_obj_set_style_text_color(wifi_label, lv_palette_main(LV_PALETTE_GREEN), 0); // Set text color to green
    lv_obj_align(wifi_label, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_t *table = lv_table_create(tab1);
    
    // Set column widths
    lv_table_set_col_width(table, 0, 80);
    lv_table_set_col_width(table, 1, 140);
    
    // Fill the table with device info
    // Set header style to smaller font
    lv_obj_set_style_text_font(table, &lv_font_montserrat_10, LV_PART_MAIN);
    
    lv_table_set_cell_value(table, 0, 0, "Property");
    lv_table_set_cell_value(table, 0, 1, "Value");
    
    lv_table_set_cell_value(table, 1, 0, "Location");
    lv_table_set_cell_value(table, 1, 1, g_config.location.c_str());
    
    lv_table_set_cell_value(table, 2, 0, "Station");
    lv_table_set_cell_value(table, 2, 1, g_config.station.c_str());
    
    lv_table_set_cell_value(table, 3, 0, "Time");
    lv_table_set_cell_value(table, 3, 1, getCurrentTimeString().c_str());
    
    lv_table_set_cell_value(table, 4, 0, "Status");
    lv_table_set_cell_value(table, 4, 1, "Active");
    
    lv_table_set_cell_value(table, 5, 0, "IP Address");
    lv_table_set_cell_value(table, 5, 1, WiFi.localIP().toString().c_str());
    
    // Set table size and position
    lv_obj_set_size(table, 240, 250);
    lv_obj_align(table, LV_ALIGN_BOTTOM_MID, 0, 0);
    
    // Add custom drawing callback
    lv_obj_add_event_cb(table, draw_part_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    // lv_obj_t *meter = lv_meter_create(tab1);
    // lv_obj_set_size(meter, 150, 150);
    // lv_obj_center(meter);

    // lv_meter_scale_t *scale_min = lv_meter_add_scale(meter);
    // lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 8, lv_palette_main(LV_PALETTE_GREY));
    // lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 270);

    // lv_meter_scale_t *scale_hour = lv_meter_add_scale(meter);
    // lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));
    // lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 2, 15, lv_color_black(), 5);
    // lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300);

    // lv_meter_indicator_t *indic_min = lv_meter_add_needle_line(meter, scale_min, 4, lv_palette_main(LV_PALETTE_GREY), -8);
    // lv_meter_indicator_t *indic_hour = lv_meter_add_needle_line(meter, scale_min, 6, lv_palette_main(LV_PALETTE_GREY), -8);

    // int hours = getCurrentTimeString().substring(0, 2).toInt();
    // int minutes = getCurrentTimeString().substring(3, 5).toInt();
    // lv_meter_set_indicator_value(meter, indic_min, minutes);
    // lv_meter_set_indicator_value(meter, indic_hour, hours);

    // LV_IMG_DECLARE(bhs); // Ensure the image resource is defined in the linked header file or source file.
    // lv_obj_t *img1 = lv_img_create(tab1);
    // lv_img_set_src(img1, &bhs);
    // lv_obj_align(img1, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    // lv_obj_add_event_cb(tab1, info_event_handler, LV_EVENT_CLICKED, NULL);

    // // Settings Tab Content
    // lv_obj_t *settings_label = lv_label_create(tab2);
    // lv_label_set_text(settings_label, "Settings Tab");
    // lv_obj_add_event_cb(settings_label, settings_event_handler, LV_EVENT_CLICKED, NULL);

    // // Explorer Tab Content
    // lv_obj_t *explorer_label = lv_label_create(tab3);
    // lv_label_set_text(explorer_label, "Click to Figure Out");
    // lv_obj_add_event_cb(explorer_label, explorer_event_handler, LV_EVENT_CLICKED, NULL);

    // Buttons Tab Content
    // begin();
    static const char *btnm_map[] = {"Call QA Inspection", "\n", "Call Supervisor", "\n", "Maintenance Ticket", NULL};
    lv_obj_t *btnm = lv_btnmatrix_create(tab4);
    lv_btnmatrix_set_map(btnm, btnm_map);
    lv_obj_set_size(btnm, 200, 200);
    lv_obj_align(btnm, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(btnm, [](lv_event_t *e)
                        {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        uint16_t btn_id = lv_btnmatrix_get_selected_btn(obj);
        lv_obj_t * parent = lv_scr_act();

        // Create popup box
        lv_obj_t * box = lv_obj_create(parent);
        lv_obj_set_size(box, 120, 120);
        lv_obj_align(box, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_bg_color(box, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_border_width(box, 1, 0);
        lv_obj_set_style_radius(box, 10, 0);

        // Spinner inside box
        lv_obj_t * spinner = lv_spinner_create(box, 1000, 20);
        lv_obj_set_size(spinner, 50, 50);
        lv_obj_align(spinner, LV_ALIGN_CENTER, 0, -10);

        // Status label inside box
        lv_obj_t * status_label = lv_label_create(box);
        lv_label_set_text(status_label, "Sending event...");
        lv_obj_align(status_label, LV_ALIGN_CENTER, 0, 30);

        g_status_box = box;
        g_status_label = status_label;

        bool skipProcessing = false;
        int event_id = -1;
        switch (btn_id) {
            case 0: // QA
                postButtonEvent("ting", "welding-station-1");
                break;
            case 1: // Supervisor
                postCallSupervisor("supervisor-01", "welding-station-1", "quality_check");
                break;
            case 2: // Ticket
                postCreateTicket("maintenance", "medium", "Equipment needs maintenance");
                break;
            default:
                skipProcessing = true;
                break;
        }

        if (skipProcessing) {
            lv_obj_del(box);
            g_status_box = NULL;
            g_status_label = NULL;
            return;
        }

        // Update status and remove popup after 2 seconds
        lv_timer_t *timer = lv_timer_create([](lv_timer_t * timer) {
            if (g_status_box) {
                lv_obj_del(g_status_box);
                g_status_box = NULL;
                g_status_label = NULL;
            }
            lv_timer_del(timer);
        }, 2000, NULL);
    } }, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btnm, fourth_tab_btnm_event_cb, LV_EVENT_ALL, NULL);

    // Sync time with NTP server
    syncTimeWithNTP();
    lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);
    drawNavBar();

    ensureButtonQueueAndTask();
}

// Add a function to sync NTP and then show the home screen
void showHomeScreenWithNtpSync()
{
    syncTimeWithNTP();
    drawHomeScreen();
}
