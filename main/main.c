#include <stdio.h>
#include <string.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include "esp_err.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#include <esp_system.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "img_converters.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_attr.h"

#include "m_camera.h"
#include "m_wifi.h"
#include "m_uri.h"
static const char *DEFAULT_TAG="APP_MAIN";

void app_main()
{   
    ESP_LOGI(DEFAULT_TAG, "HELLO XUEJIAO");

    m_camera_init_default();
    //ESP_ERROR_CHECK(nvs_flash_init());
    //m_wifi_sta_init_default();

    //m_uri_init_default();
    uint32_t i = 0;
    for (;;) {
        vTaskDelay(2000);
        ESP_LOGI(DEFAULT_TAG, "CNT %lu", i++);
    }
}
