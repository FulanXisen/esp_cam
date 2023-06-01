#include <string.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include "esp_netif.h"
#include "esp_wpa2.h"

#include "freertos/event_groups.h"
#include "srv_wifi.h"
#include "srv_http.h"
#include "linked_list.h"

static const char *TAG = "app_wifi";

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* esp netif object representing the WIFI station */
static esp_netif_t *netif_sta = NULL;

static WIFI_MODE_CONFIG_t WIFI_MODE_CONFIG_TABLE[WIFI_MODE_MAX];

static WIFI_AUTH_CONFIG_t WIFI_AUTH_CONFIG_TABLE[WIFI_AUTH_MAX];

const int CONNECTED_BIT = BIT0;

void presetting(){
    //LINKED_LIST_t *cc = LINKED_LIST_New();
    //LINKED_LIST_Add(cc, (void *)1, sizeof(uint8_t));
    //cc->Add(cc, (void *)(uint8_t)1, sizeof(uint8_t));
    ESP_ERROR_CHECK(esp_netif_init());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    netif_sta = esp_netif_create_default_wifi_sta();
    assert(netif_sta);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg));
}

void setup_sta_wpa2_ent(){
    presetting();
    // ESP_ERROR_CHECK(esp_netif_init());
    // wifi_event_group = xEventGroupCreate();
    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    // netif_sta = esp_netif_create_default_wifi_sta();
    // assert(netif_sta);

    // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // ESP_ERROR_CHECK( esp_wifi_init(&cfg));
    //ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    //ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config;
    memccpy(wifi_config.sta.ssid, 
        WIFI_MODE_CONFIG_TABLE[EX_WIFI_MODE_STA].sta.ssid, 
        strlen((char *)WIFI_MODE_CONFIG_TABLE[EX_WIFI_MODE_STA].sta.ssid), 
        sizeof(uint8_t));
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    WIFI_AUTH_WPA2_ENTERPRISE_CONFIG_t cfg = WIFI_AUTH_CONFIG_TABLE[EX_WIFI_AUTH_WPA2_ENTERPRISE].wpa2ent;
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_identity(cfg.id, strlen((char *)cfg.id)) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_username(cfg.username, strlen((char *)cfg.username)));
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_password(cfg.password, strlen((char *)cfg.password)));
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_enable() );
    ESP_ERROR_CHECK( esp_wifi_start() );
    esp_wifi_connect();
}

static void (*WIFI_SETUP_TABLE[WIFI_MODE_MAX][WIFI_AUTH_MAX])(void) = {
    [EX_WIFI_MODE_STA][EX_WIFI_AUTH_WPA2_ENTERPRISE] = setup_sta_wpa2_ent,
};



void srv_wifi_register_mode_config(WIFI_MODE_t mode, WIFI_MODE_CONFIG_t cfg){
    WIFI_MODE_CONFIG_TABLE[mode] = cfg;
}

void srv_wifi_register_auth_config(WIFI_AUTH_t auth, WIFI_AUTH_CONFIG_t cfg){
    WIFI_AUTH_CONFIG_TABLE[auth] = cfg;
}

void srv_wifi_setup_and_start(WIFI_MODE_t mode, WIFI_AUTH_t auth){
    WIFI_SETUP_TABLE[mode][auth]();
}


#define CONFIG_WIFI_WPA2_ENT_EAP_ID "fanyx@shanghaitech.edu.cn"
#define CONFIG_WIFI_WPA2_ENT_EAP_METHOD_PEAP_USERNAME "2020233216"
#define CONFIG_WIFI_WPA2_ENT_EAP_METHOD_PEAP_PASSWORD "Fanfan227571"
#define CONFIG_WIFI_SSID "ShanghaiTech"



static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data){
    static httpd_handle_t server = NULL;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        if ( server == NULL ){

        }else if (server != NULL){
            app_http_stop_webserver(server);
            server = NULL;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        if (server == NULL){
            server = app_http_start_webserver();
        }
    }
}

static void _wifi_mode_sta_wpa2_ent_method_peap_init(){

    ESP_ERROR_CHECK(esp_netif_init());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    netif_sta = esp_netif_create_default_wifi_sta();
    assert(netif_sta);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg));
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)CONFIG_WIFI_WPA2_ENT_EAP_ID, strlen(CONFIG_WIFI_WPA2_ENT_EAP_ID)) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_username((uint8_t *)CONFIG_WIFI_WPA2_ENT_EAP_METHOD_PEAP_USERNAME, strlen(CONFIG_WIFI_WPA2_ENT_EAP_METHOD_PEAP_USERNAME)) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_set_password((uint8_t *)CONFIG_WIFI_WPA2_ENT_EAP_METHOD_PEAP_PASSWORD, strlen(CONFIG_WIFI_WPA2_ENT_EAP_METHOD_PEAP_PASSWORD)) );
    ESP_ERROR_CHECK( esp_wifi_sta_wpa2_ent_enable() );
    ESP_ERROR_CHECK( esp_wifi_start() );
    esp_wifi_connect();
}

static void wpa2_enterprise_example_task(void *pvParameters)
{
    esp_netif_ip_info_t ip;
    memset(&ip, 0, sizeof(esp_netif_ip_info_t));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (1) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        if (esp_netif_get_ip_info(netif_sta, &ip) == 0) {
            ESP_LOGI(TAG, "~~~~~~~~~~~");
            ESP_LOGI(TAG, "IP:"IPSTR, IP2STR(&ip.ip));
            ESP_LOGI(TAG, "MASK:"IPSTR, IP2STR(&ip.netmask));
            ESP_LOGI(TAG, "GW:"IPSTR, IP2STR(&ip.gw));
            ESP_LOGI(TAG, "~~~~~~~~~~~");
        }
    }
}

// void srv_wifi_setup_and_start(void){
//     _wifi_mode_sta_wpa2_ent_method_peap_init();
//     xTaskCreate(&wpa2_enterprise_example_task, "wpa2_enterprise_example_task", 4096, NULL, 5, NULL);
// }