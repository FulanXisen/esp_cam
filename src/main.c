#include <stdio.h>
#include <string.h>
#include <nvs_flash.h>
#include <sys/param.h>

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

#include "app_camera.h"
#include "app_wifi.h"
#include "app_uri.h"
//static const char *TAG="main";

void app_main()
{
    if(ESP_OK != init_camera()) {
        return;
    }
    ESP_ERROR_CHECK(nvs_flash_init());
    app_uri_srv_init();
    app_wifi_init();
}
