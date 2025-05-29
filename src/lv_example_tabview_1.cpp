#include "lv_example_widgets.h"
#if LV_USE_TABVIEW && LV_BUILD_EXAMPLES

// Custom event callback for Fourth tab button matrix
static void fourth_tab_btnm_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
        if(dsc->class_p == &lv_btnmatrix_class && dsc->type == LV_BTNMATRIX_DRAW_PART_BTN) {
            if(dsc->id == 0) { // Call QA
                dsc->rect_dsc->radius = 0;
                if(lv_btnmatrix_get_selected_btn(obj) == dsc->id)  dsc->rect_dsc->bg_color = lv_palette_darken(LV_PALETTE_BLUE, 3);
                else dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_BLUE);
                dsc->rect_dsc->shadow_width = 6;
                dsc->rect_dsc->shadow_ofs_x = 3;
                dsc->rect_dsc->shadow_ofs_y = 3;
                dsc->label_dsc->color = lv_color_white();
            } else if(dsc->id == 1) { // Call Tech
                dsc->rect_dsc->radius = LV_RADIUS_CIRCLE;
                if(lv_btnmatrix_get_selected_btn(obj) == dsc->id)  dsc->rect_dsc->bg_color = lv_palette_darken(LV_PALETTE_RED, 3);
                else dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_RED);
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

/*A vertical tab view with disabled scrolling and some styling*/
void lv_example_tabview_2(void)
{
    /*Create a Tab view object*/
    lv_obj_t * tabview;
    uint32_t tab_count = 0;
    uint32_t i = 0;

    tabview = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
    lv_tabview_set_tab_bar_size(tabview, 80);

    lv_obj_set_style_bg_color(tabview, lv_palette_lighten(LV_PALETTE_RED, 2), 0);

    lv_obj_t * tab_buttons = lv_tabview_get_tab_bar(tabview);
    lv_obj_set_style_bg_color(tab_buttons, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_text_color(tab_buttons, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);

    /*Add 5 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Tab 3");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "Tab 4");
    lv_obj_t * tab5 = lv_tabview_add_tab(tabview, "Tab 5");

    tab_count = lv_tabview_get_tab_count(tabview);
    for(i = 0; i < tab_count; i++) {
        lv_obj_t * button = lv_obj_get_child(tab_buttons, i);
        lv_obj_set_style_border_side(button, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_CHECKED);
    }
    lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_AMBER, 3), 0);
    lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);

    /*Add content to the tabs*/
    lv_obj_t * label = lv_label_create(tab1);
    lv_label_set_text(label, "First tab");

    label = lv_label_create(tab2);
    lv_label_set_text(label, "Second tab");

    label = lv_label_create(tab3);
    lv_label_set_text(label, "Third tab");

    // Add button matrix to Tab 4
    static const char * btnm_map[] = {"Call QA", "Call Tech", "Create Support Ticket", "", NULL};
    lv_obj_t *btnm = lv_btnmatrix_create(tab4);
    lv_btnmatrix_set_map(btnm, btnm_map);
    lv_obj_set_size(btnm, 160, 320);
    lv_obj_align(btnm, LV_ALIGN_RIGHT_MID, -100, 0); // Moved 10 pixels closer to the right edge
    0lv_obj_add_event_cb(btnm, fourth_tab_btnm_event_cb, LV_EVENT_ALL, NULL);

    label = lv_label_create(tab5);
    lv_label_set_text(label, "Fifth tab");

    lv_obj_remove_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);
}
#endif