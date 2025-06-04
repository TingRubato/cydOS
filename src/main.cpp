/**
 * @file main.cpp
 * @brief cydOS main application entry point for ESP32-2432S028R
 *
 * This file initializes the display, touchscreen, LVGL, WiFi, and core tasks for cydOS.
 * It manages the main event loop and hardware abstraction for the GUI OS.
 */
#include <TFT_eSPI.h>
#include <XPT2046_Bitbang.h>
#include "home_screen.h"
#include "utils.h" // Include il file di intestazione
#include "main.h"
#include <time.h>
#include "WIFI_utils.h"
#include <WiFi.h>
#include "config.h"
#include "I2C_utils.h"

/**
 * @brief Callback to update the LVGL UI after WiFi connection is established.
 * @param param Unused parameter (for compatibility with lv_async_call)
 */
void update_lvgl_on_wifi_connect(void *param) {
    Serial.println("[LVGL] Updating UI after WiFi connected!");
    syncTimeWithNTP();

    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "WiFi + NTP Ready!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    drawHomeScreen();
}

// Initialize the SPI class
SPIClass mySpi = SPIClass(VSPI);
XPT2046_Bitbang ts = XPT2046_Bitbang(XPT2046_MOSI, XPT2046_MISO, XPT2046_CLK, XPT2046_CS);

uint16_t touchScreenMinimumY = 200, touchScreenMaximumY = 3700, touchScreenMinimumX = 240, touchScreenMaximumX = 3800;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[240 * 320 / 10];

TFT_eSPI tft = TFT_eSPI(240, 320);
static bool touchPressed = false;

/**
 * @brief Get the system uptime in milliseconds.
 * @return Milliseconds since boot.
 */
unsigned long getSystemTime() { return millis(); } 

static lv_color_t *dma_buf = NULL;
static bool dma_initialized = false;

// Declare and initialize `wifiConnected`
static bool wifiConnected = false;

/**
 * @brief LVGL display flush callback. Transfers a rendered area to the TFT display.
 * @param disp_drv LVGL display driver pointer
 * @param area Area to update
 * @param color_p Pointer to color buffer
 */
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    if(!dma_initialized) {
        dma_buf = (lv_color_t*)heap_caps_malloc(LV_HOR_RES * 10 * sizeof(lv_color_t), 
                                              MALLOC_CAP_DMA);
        if(dma_buf) dma_initialized = true;
    }

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    
    if(dma_initialized && w*h <= LV_HOR_RES*10) {
        memcpy(dma_buf, color_p, w*h*sizeof(lv_color_t));
        tft.pushPixelsDMA((uint16_t*)dma_buf, w*h);
    } else {
        tft.pushColors((uint16_t*)color_p, w*h, true);
    }
    
    tft.endWrite();
    lv_disp_flush_ready(disp_drv);
}

/**
 * @brief LVGL touchpad read callback. Reads the current touch state and position.
 * @param indev_drv LVGL input device driver pointer
 * @param data Pointer to input data structure to fill
 */
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) // Touchy-feely function
{
    TouchPoint p = ts.getTouch();
    if (p.x != 0 && p.y != 0)
    {
        // 恢复之前的坐标映射方式
        data->point.x = 240 - p.y;
        data->point.y = p.x;
        data->state = LV_INDEV_STATE_PR;
        touchPressed = true;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
        touchPressed = false;
    }
    // digitalWrite(XPT2046_CS, HIGH); // 保持注释
}

/**
 * @brief Clears the display and LVGL screen objects.
 */
void flushDisplay()
{
    tft.fillScreen(TFT_BLACK);
    lv_obj_clean(lv_scr_act());
}

// Removed INIT_CHECK macro due to incompatibility with void-returning functions and C++11

// Task handles for core tasks
static TaskHandle_t lvglTaskHandle = NULL;
static TaskHandle_t wifiTaskHandle = NULL;

/**
 * @brief Task for running the LVGL timer handler and UI updates.
 * @param pvParameters Unused parameter
 */
void lvglTask(void *pvParameters) {
    drawHomeScreen();  // Only draw initial screen; avoid running WiFi here

    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/**
 * @brief Task for managing WiFi connection and reconnection logic.
 * @param pvParameters Unused parameter
 */
void wifiTask(void *pvParameters) {
    const char *ssid = g_config.wifi_ssid.c_str();
    const char *password = g_config.wifi_password.c_str();

    if (connectToNetwork(ssid, password)) {
        wifiConnected = true;
        // Signal LVGL thread safely
        lv_async_call(update_lvgl_on_wifi_connect, NULL);
    } else {
        Serial.println("[WiFi] Failed to connect.");
    }

    while (1) {
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WiFi] Disconnected, trying to reconnect...");
            connectToNetwork(ssid, password);
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Check every 10 seconds
    }
}

/**
 * @brief Arduino setup function. Initializes hardware, LVGL, and creates core tasks.
 */
void setup() {
    Serial.begin(115200);
    if (!loadConfig()) {
        Serial.println("Using default config or failed to load config!");
    }

    // Initialize I2C
    if (initI2C()) {
        Serial.println("I2C initialized successfully");
        scanI2CDevices();  // Scan for connected I2C devices
    } else {
        Serial.println("Failed to initialize I2C");
    }

    // Initialize display
    tft.begin();
    tft.setRotation(0);
    mySpi.begin(25, 39, 32, 33);
    ts.begin();

    // Initialize LVGL
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

    // Create tasks
    xTaskCreatePinnedToCore(lvglTask, "LVGL", 8192, NULL, 3, &lvglTaskHandle, 0);
    xTaskCreatePinnedToCore(wifiTask, "WiFi", 4096, NULL, 2, &wifiTaskHandle, 1);
}

/**
 * @brief Arduino main loop. Idle loop; all work is done in FreeRTOS tasks.
 */
void loop() {
    // Main loop does nothing; all work is done in tasks
    vTaskDelay(pdMS_TO_TICKS(1000)); // Just to yield CPU
}