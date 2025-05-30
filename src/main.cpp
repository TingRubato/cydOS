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

static lv_color_t *dma_buf = NULL;
static bool dma_initialized = false;

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

void flushDisplay()
{
    tft.fillScreen(TFT_BLACK);
    lv_obj_clean(lv_scr_act());
}

// Removed INIT_CHECK macro due to incompatibility with void-returning functions and C++11

// Task handles for core tasks
static TaskHandle_t lvglTaskHandle = NULL;
static TaskHandle_t wifiTaskHandle = NULL;

void lvglTask(void *pvParameters) {
    for(;;) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void wifiTask(void *pvParameters) {
    for(;;) {
        if(WiFi.status() != WL_CONNECTED) {
            // Handle reconnection logic
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Check every 10 seconds
    }
}

void setup() {
    esp_err_t err = ESP_OK;
    
    // Core 0 tasks (high priority)
    xTaskCreatePinnedToCore(
        lvglTask,       // Task function
        "LVGL",         // Task name
        8192,          // Stack size
        NULL,           // Parameters
        3,              // Priority (higher than wifi)
        &lvglTaskHandle,// Task handle
        0               // Core 0
    );

    // Core 1 tasks (lower priority)
    xTaskCreatePinnedToCore(
        wifiTask,       // Task function
        "WiFi",         // Task name
        4096,           // Stack size
        NULL,           // Parameters
        2,              // Priority
        &wifiTaskHandle,// Task handle
        1               // Core 1
    );

    Serial.begin(115200);
    if(!Serial) {
        ESP_LOGI(ESP_FAIL, "Serial init failed");
        return;
    }

    tft.begin();
    // tft.begin() returns void, so just call and optionally check display with tft.readID() or similar if needed

    tft.setRotation(0);

    mySpi.begin(25, 39, 32, 33);
    // 若 XPT2046_Bitbang 支持 begin()，则调用
    ts.begin();

    lv_init();

    // pinMode(LED_RED, OUTPUT);
    // pinMode(LED_GREEN, OUTPUT);
    // pinMode(LED_BLUE, OUTPUT);
    // digitalWrite(LED_RED, HIGH);
    // digitalWrite(LED_GREEN, HIGH);
    // digitalWrite(LED_BLUE, HIGH);

    ESP_LOGI(ESP_FAIL, "LVGL init failed");

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, 240 * 320 / 10);
    // lv_init() already called above

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

#include "esp_task_wdt.h"

void loop() {
    // Empty since tasks handle everything
    vTaskDelay(pdMS_TO_TICKS(1000)); // Minimal delay
}
