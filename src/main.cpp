#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Bitbang.h>
#include "home_screen.h"
#include "utils.h" // Include il file di intestazione
#include "main.h"
#include <time.h>
#include "WIFI_utils.h"

// Initialize the SPI class
SPIClass mySpi = SPIClass(VSPI);
XPT2046_Bitbang ts = XPT2046_Bitbang(XPT2046_MOSI, XPT2046_MISO, XPT2046_CLK, XPT2046_CS);

uint16_t touchScreenMinimumY = 200, touchScreenMaximumY = 3700, touchScreenMinimumX = 240, touchScreenMaximumX = 3800;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[240 * 320 / 10];

TFT_eSPI tft = TFT_eSPI(240, 320);
static bool touchPressed = false;
unsigned long getSystemTime() { return millis(); } 

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp_drv);
}

void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    TouchPoint p = ts.getTouch();
    if (p.x != 0 && p.y != 0)
    {
        data->point.x = 240 - p.y;
        data->point.y = p.x;
        data->state = LV_INDEV_STATE_PR;
        if (!touchPressed)
        {
            touchPressed = true;
        }
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
        if (touchPressed)
        {
            touchPressed = false;
        }
    }

    digitalWrite(XPT2046_CS, HIGH);
}

void flushDisplay()
{
    tft.fillScreen(TFT_BLACK);
    lv_obj_clean(lv_scr_act());
}

void setup()
{
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(0);
    
    mySpi.begin(25, 39, 32, 33);
    ts.begin();

    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);

    lv_init();

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, 240 * 320 / 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 320;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // Initialize WiFi
    initWiFi();
    
    // Start UI
    drawHomeScreen();
}

void loop()
{
    lv_timer_handler();
    lv_task_handler();
    delay(5);
}