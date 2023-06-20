#include <string.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>
#include <esp_log.h>
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_wpa2.h"
#include "esp_wpa.h"

#include "freertos/event_groups.h"
#include "m_wifi.h"
#include "srv_http.h"
#include "linked_list.h"

/* BEGIN Private Macros Definition */
#define CONFIG_WIFI_IDENTITY "6-301"
#define CONFIG_WIFI_USERNAME ""
#define CONFIG_WIFI_PASSWORD "15921244482"

// #define CONFIG_WIFI_WPA2_ENT_EAP_ID "fanyx@shanghaitech.edu.cn"
// #define CONFIG_WIFI_WPA2_ENT_EAP_METHOD_PEAP_USERNAME "2020233216"
// #define CONFIG_WIFI_WPA2_ENT_EAP_METHOD_PEAP_PASSWORD "Fanfan227571"
// #define CONFIG_WIFI_SSID "ShanghaiTech"
/* END Private Macros Definition */

/* BEGIN Private Variables Definition */
static const char *DEFAULT_TAG = "mWiFi"; /* log tag for this module */

extern esp_netif_t *netif_sta; /* esp netif object representing the WIFI station */

static EventGroupHandle_t wifi_event_group; /* FreeRTOS event group to signal when we are connected & ready to make a request */

const int CONNECTED_BIT = BIT0; /* control bit of freertos event group for wifi event */
/* END Private Variables Definition */

/* BEGIN Private Functions Definition */
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data){
    static httpd_handle_t server = NULL;
    if (event_base == WIFI_EVENT ) {
        if (event_id == WIFI_EVENT_STA_START) {
             ESP_LOGI(DEFAULT_TAG, "SYSTEM_EVENT_STA_START");
            esp_wifi_connect();
        }else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            ESP_LOGI(DEFAULT_TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
            if ( server == NULL ){

            }else if (server != NULL){
                app_http_stop_webserver(server);
                server = NULL;
            }
        }
    }
    if (event_base == IP_EVENT) {
        if (event_id == IP_EVENT_STA_GOT_IP) {
            ESP_LOGI(DEFAULT_TAG, "SYSTEM_EVENT_STA_GOT_IP");
            xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            if (server == NULL){
                server = app_http_start_webserver();
            }
        }
    }
}

__attribute__((unused))
static void wpa2_enterprise_example_task(void *pvParameters)
{
    esp_netif_ip_info_t ip;
    memset(&ip, 0, sizeof(esp_netif_ip_info_t));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (1) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        if (esp_netif_get_ip_info(netif_sta, &ip) == 0) {
            ESP_LOGI(DEFAULT_TAG, "~~~~~~~~~~~");
            ESP_LOGI(DEFAULT_TAG, "IP:"IPSTR, IP2STR(&ip.ip));
            ESP_LOGI(DEFAULT_TAG, "MASK:"IPSTR, IP2STR(&ip.netmask));
            ESP_LOGI(DEFAULT_TAG, "GW:"IPSTR, IP2STR(&ip.gw));
            ESP_LOGI(DEFAULT_TAG, "~~~~~~~~~~~");
        }
    }
}

/* END Private Functions Definition */

/* BEGIN Public Functions Definition */
void m_wifi_enable_wpa_default(void){
#ifdef CFG_WIFI_AUTH_METHOD_WPA
    /* Identity of the WiFi, eg. "TPLINK-12345", eg. "HOTEL-502" */
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_identity((unsigned char *)CONFIG_WIFI_IDENTITY, sizeof(CONFIG_WIFI_IDENTITY)) );
    ESP_LOGI(DEFAULT_TAG, "WiFi identity is %s...", WIFI_IDENTITY);

    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_username((unsigned char *)WIFI_USERNAME, sizeof(WIFI_USERNAME)) );
    ESP_LOGI(DEFAULT_TAG, "WiFi username is %s...", WIFI_IDENTITY);

    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_password((unsigned char *)CONFIG_WIFI_PASSWORD, sizeof(WIFI_PASSWORD)) );
    ESP_LOGI(DEFAULT_TAG, "WiFi password is %s...", WIFI_IDENTITY);

    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_enable() );
    ESP_LOGI(DEFAULT_TAG, "WiFi sta wpa2 ent is enable ");
#endif 
}
void m_wifi_sta_init_default(void) {
    /* initialize net if */
    ESP_ERROR_CHECK( esp_netif_init());

    /* create event group for handling wifi event */
    wifi_event_group = xEventGroupCreate();
        /* a default event loop */
    ESP_ERROR_CHECK( esp_event_loop_create_default());
        /* register wifi event handler */
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
        /* register ip event handler */
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );

    /* create default netif wifi sta */
    netif_sta = esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg));

    /* wifi config store only in MEM */
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_LOGI(DEFAULT_TAG, "WiFi mode is STA");

    wifi_config_t wifi_config;
    memcpy(wifi_config.sta.ssid, CONFIG_WIFI_IDENTITY, sizeof(CONFIG_WIFI_IDENTITY));
    memcpy(wifi_config.sta.password, CONFIG_WIFI_PASSWORD, sizeof(CONFIG_WIFI_PASSWORD));
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_LOGI(DEFAULT_TAG, "WiFi SSID is %s...", CONFIG_WIFI_IDENTITY);

    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_LOGI(DEFAULT_TAG, "WiFi start ");

    ESP_ERROR_CHECK( esp_wifi_connect() );
    ESP_LOGI(DEFAULT_TAG, "WiFi connect");
}

void m_wifi_sta_init(void) {
    /* initialize net if */
    ESP_ERROR_CHECK( esp_netif_init());

    /* create event group for handling wifi event */
    wifi_event_group = xEventGroupCreate();
        /* a default event loop */
    ESP_ERROR_CHECK( esp_event_loop_create_default());
        /* register wifi event handler */
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
        /* register ip event handler */
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );

    /* create default netif wifi sta */
    netif_sta = esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg));

    /* wifi config store only in MEM */
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_LOGI(DEFAULT_TAG, "WiFi mode is STA");

    wifi_config_t wifi_config;
    memcpy(wifi_config.sta.ssid, WIFI_SSID, sizeof(WIFI_SSID));
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_LOGI(DEFAULT_TAG, "WiFi SSID is %s...", WIFI_SSID);

    /* Identity of the WiFi, eg. "TPLINK-12345", eg. "HOTEL-502" */
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_identity((unsigned char *)WIFI_IDENTITY, sizeof(WIFI_IDENTITY)) );
    ESP_LOGI(DEFAULT_TAG, "WiFi identity is %s...", WIFI_IDENTITY);

    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_username((unsigned char *)WIFI_USERNAME, sizeof(WIFI_USERNAME)) );
    ESP_LOGI(DEFAULT_TAG, "WiFi username is %s...", WIFI_IDENTITY);

    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_password((unsigned char *)WIFI_PASSWORD, sizeof(WIFI_PASSWORD)) );
    ESP_LOGI(DEFAULT_TAG, "WiFi password is %s...", WIFI_IDENTITY);

    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_enable() );
    ESP_LOGI(DEFAULT_TAG, "WiFi sta wpa2 ent is enable ");

    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_LOGI(DEFAULT_TAG, "WiFi start ");

    ESP_ERROR_CHECK( esp_wifi_connect() );
    ESP_LOGI(DEFAULT_TAG, "WiFi connect");

}
/* END Public Functions Definition */