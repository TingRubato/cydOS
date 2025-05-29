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
// #include <WiFiClientSecure.h>

// Global variables for NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
struct tm timeinfo;

static void fourth_tab_btnm_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
        if(dsc->class_p == &lv_btnmatrix_class && dsc->type == LV_BTNMATRIX_DRAW_PART_BTN) {
            if(dsc->id == 0) { // Call QA
                dsc->rect_dsc->radius = 0;
                dsc->rect_dsc->bg_color = (lv_btnmatrix_get_selected_btn(obj) == dsc->id) ? 
                    lv_palette_darken(LV_PALETTE_BLUE, 3) : lv_palette_main(LV_PALETTE_BLUE);
                dsc->rect_dsc->shadow_width = 6;
                dsc->rect_dsc->shadow_ofs_x = 3;
                dsc->rect_dsc->shadow_ofs_y = 3;
                dsc->label_dsc->color = lv_color_white();
            } else if(dsc->id == 1) { // Call Tech
                dsc->rect_dsc->radius = LV_RADIUS_CIRCLE;
                dsc->rect_dsc->bg_color = (lv_btnmatrix_get_selected_btn(obj) == dsc->id) ? 
                    lv_palette_darken(LV_PALETTE_RED, 3) : lv_palette_main(LV_PALETTE_RED);
                dsc->label_dsc->color = lv_color_white();
            } else if(dsc->id == 2) { // Create Support Ticket
                dsc->rect_dsc->radius = 8;
                dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_GREEN);
                dsc->label_dsc->color = lv_color_white();
            }
        }
    }
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint16_t btn_id = lv_btnmatrix_get_selected_btn(obj);
        const char *txt = lv_btnmatrix_get_btn_text(obj, btn_id);
        if(txt) {
            LV_LOG_USER("Button pressed: %s", txt);
        }
    }
}

void syncTimeWithNTP() {
    Serial.println("Syncing time with NTP server...");

    timeClient.begin();
    timeClient.update();
    
    time_t localEpoch = timeClient.getEpochTime() - 5*3600; // -5 hours to represent CDT
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
   Serial.println("Syncing time with NTP server...");

    timeClient.begin();
    timeClient.update();
    
    time_t localEpoch = timeClient.getEpochTime() - 5*3600; // -5 hours to represent CDT
    localtime_r(&localEpoch, &timeinfo);
    
    struct timeval tv = {
        .tv_sec = localEpoch,
        .tv_usec = 0
    };
    settimeofday(&tv, NULL);

    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
    return String(timeStr);
}

void drawHomeScreen() {
    init_sd_card();
    Serial.println("Initializing WiFi...");
        const char* ssid = "SpectrumSetup-21";
        const char* password = "vasttooth431";
        Serial.printf("Loading saved password for SSID: %s\n", ssid);
        Serial.printf("connecting using password: %s\n", password);
        if (connectToNetwork(ssid, password)) {
                Serial.println("Connection successful");
                lv_obj_t *scr = lv_scr_act();
                lv_obj_clean(scr);
                lv_obj_t *label = lv_label_create(scr);
                lv_label_set_text(label, "Connected successfully!");
                lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
            } 
        else {
                Serial.println("Connection failed");
                lv_obj_t *label = lv_label_create(lv_scr_act());
                lv_label_set_text(label, "Failed to connect to WiFi");
                lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
            }
    
    // Create the tabview
    lv_obj_t * tabview = lv_tabview_create(lv_scr_act(), LV_DIR_LEFT, 70);
    lv_obj_set_size(tabview, 240, 250);
    lv_obj_align(tabview, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(tabview, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 2), 0);

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_RIGHT, LV_PART_ITEMS | LV_STATE_CHECKED);

    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Info");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Settings");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Explorer");
    lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "Buttons");

    // Info Tab Content
    lv_obj_t * label = lv_label_create(tab1);
    lv_label_set_text(label, getCurrentTimeString().c_str());

    lv_obj_t * meter = lv_meter_create(tab1);
    lv_obj_set_size(meter, 150, 150);
    lv_obj_center(meter);

    lv_meter_scale_t * scale_min = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 8, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 270);

    lv_meter_scale_t * scale_hour = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 2, 15, lv_color_black(), 5);
    lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300);

    lv_meter_indicator_t * indic_min = lv_meter_add_needle_line(meter, scale_min, 4, lv_palette_main(LV_PALETTE_GREY), -8);
    lv_meter_indicator_t * indic_hour = lv_meter_add_needle_line(meter, scale_min, 6, lv_palette_main(LV_PALETTE_GREY), -8);
   
    time_t localEpoch = timeClient.getEpochTime() - 5*3600; // -5 hours to represent CDT
    localtime_r(&localEpoch, &timeinfo);
    int hours = timeinfo.tm_hour;
    int minutes = timeinfo.tm_min;
    lv_meter_set_indicator_value(meter, indic_min, minutes);  
    lv_meter_set_indicator_value(meter, indic_hour, hours);

    LV_IMG_DECLARE(bhs);
    lv_obj_t * img1 = lv_img_create(tab1);
    lv_img_set_src(img1, &bhs);
    lv_obj_align(img1, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(tab1, info_event_handler, LV_EVENT_CLICKED, NULL);

    // Settings Tab Content
    label = lv_label_create(tab2);
    lv_label_set_text(label, "Settings Tab");
    lv_obj_add_event_cb(tab2, settings_event_handler, LV_EVENT_CLICKED, NULL);

    // Explorer Tab Content
    label = lv_label_create(tab3);
    lv_label_set_text(label, "File Explorer Tab");
    lv_obj_add_event_cb(tab3, explorer_event_handler, LV_EVENT_CLICKED, NULL);

    // Buttons Tab Content
    // begin();
    static const char * btnm_map[] = {"QA", "\n", "Tech", "\n", "Ticket", "\n", "Health", NULL};
    lv_obj_t *btnm = lv_btnmatrix_create(tab4);
    lv_btnmatrix_set_map(btnm, btnm_map);
    lv_obj_set_width(btnm, 160);
    lv_obj_set_height(btnm, 260);
    lv_obj_align(btnm, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(btnm, [](lv_event_t * e) {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t * obj = lv_event_get_target(e);
        if(code == LV_EVENT_CLICKED) {
            uint16_t btn_id = lv_btnmatrix_get_selected_btn(obj);
            if(btn_id == 0) { // QA button
                postButtonEvent("QA", "Zone1");
            } else if(btn_id == 3) { // Health button

                postHealthStatus(100);  // send health at startup with full battery
            }
        }
    }, LV_EVENT_CLICKED, NULL);

    // label = lv_label_create(tab4);
    // lv_label_set_text(label, "Magic Buttons");
    lv_obj_align(btnm, LV_ALIGN_CENTER, 0, 0);  // Center align the button matrix
    lv_obj_add_event_cb(btnm, fourth_tab_btnm_event_cb, LV_EVENT_ALL, NULL);

    // Sync time with NTP server
    syncTimeWithNTP();
    
    lv_obj_clear_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);
    drawNavBar();
}

// Add a function to sync NTP and then show the home screen
void showHomeScreenWithNtpSync() {
    syncTimeWithNTP();
    drawHomeScreen();
}