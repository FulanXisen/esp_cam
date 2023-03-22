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

#include "srv_camera.h"
#include "srv_wifi.h"
#include "srv_uri.h"
//static const char *TAG="main";

void app_main()
{
    if(ESP_OK != init_camera()) {
        return;
    }
    ESP_ERROR_CHECK(nvs_flash_init());
    app_srv_uri_init();

    WIFI_MODE_CONFIG_t wifi_mode_cfg = {
        .sta = {
            .ssid = "ShanghaiTech",
        },
    };
    WIFI_AUTH_CONFIG_t wifi_auth_cfg = {
        .wpa2ent = {
            .id = "fanyx@shanghaitech.edu.cn",
            .username = "2020233216",
            .password = "Fanfan227571",
        },
    };
    srv_wifi_register_mode_config(EX_WIFI_MODE_STA, wifi_mode_cfg);
    srv_wifi_register_auth_config(EX_WIFI_AUTH_WPA2_ENTERPRISE, wifi_auth_cfg);
    srv_wifi_setup_and_start(EX_WIFI_MODE_STA, EX_WIFI_AUTH_WPA2_ENTERPRISE);
    
}
