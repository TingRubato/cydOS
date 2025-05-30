#include "ui.h"
#include "home_screen.h"
#include "settings.h"
#include "event_handlers.h"
#include "utils.h"
#include <lvgl.h>
#include "main.h"


// //basic ui elements like navbar anche status bar // call them when needed
// // Handler per il pulsante della torcia


// These objects stick around like that one ex who won't stop texting
static lv_obj_t *slide_menu = NULL;
static lv_obj_t *torch_btn = NULL;
static lv_obj_t *home_BTN = NULL;


void resetNavBarPointers() {
    slide_menu = NULL;
    torch_btn = NULL;
    home_BTN = NULL;
}

void drawNavBar() {
    // 检查 slide_menu 是否还有效
    if (!slide_menu || !lv_obj_is_valid(slide_menu)) {
        resetNavBarPointers();
        const short dockmargin = 55;
        const short dockiconsize = 50;
        lv_obj_t *parent = lv_scr_act();
        slide_menu = lv_obj_create(parent);
        lv_obj_set_size(slide_menu, 240, dockmargin);
        lv_obj_set_style_bg_color(slide_menu, lv_color_hex(0xD3D3D3), LV_PART_MAIN);
        lv_obj_align(slide_menu, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_clear_flag(slide_menu, LV_OBJ_FLAG_SCROLLABLE);

        // Create torch button if not exists
        if (!torch_btn) {
            torch_btn = lv_btn_create(slide_menu);
            lv_obj_set_size(torch_btn, dockiconsize, dockiconsize);
            lv_obj_add_event_cb(torch_btn, torch_event_handler, LV_EVENT_CLICKED, NULL);
            lv_obj_align(torch_btn, LV_ALIGN_LEFT_MID, 0, 0);
            lv_obj_set_style_bg_color(torch_btn, lv_color_hex(0xA9A9A9), LV_PART_MAIN);
            lv_obj_t *torch_label = lv_label_create(torch_btn);
            lv_label_set_text(torch_label, LV_SYMBOL_EYE_OPEN);
            lv_obj_center(torch_label);
        }
        // Create home button if not exists  
        if (!home_BTN) {
            home_BTN = lv_btn_create(slide_menu);
            lv_obj_set_size(home_BTN, dockiconsize, dockiconsize);
            lv_obj_align(home_BTN, LV_ALIGN_RIGHT_MID, 10, 0);
            lv_obj_set_style_bg_color(home_BTN, lv_color_hex(0xA9A9A9), LV_PART_MAIN);
            lv_obj_add_event_cb(home_BTN, home_button_event_handler, LV_EVENT_CLICKED, NULL);
            lv_obj_t *home_BTN_label = lv_label_create(home_BTN);
            lv_label_set_text(home_BTN_label, LV_SYMBOL_HOME);
            lv_obj_center(home_BTN_label);
        }
        // BHS Label (static as it doesn't change)
        lv_obj_t *bhs_label = lv_label_create(slide_menu);
        lv_label_set_text(bhs_label, "BHS Inc.");
        lv_obj_align(bhs_label, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_set_style_text_color(bhs_label, lv_color_hex(0x000000), LV_PART_MAIN);
    }
}
