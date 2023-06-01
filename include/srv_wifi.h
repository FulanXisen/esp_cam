#pragma once 

typedef enum {
    EX_WIFI_MODE_BEGIN = 0,
    EX_WIFI_MODE_NULL,      /**< null mode */
    EX_WIFI_MODE_STA,       /**< WiFi station mode */
    EX_WIFI_MODE_AP,        /**< WiFi soft-AP mode */
    EX_WIFI_MODE_APSTA,     /**< WiFi station + soft-AP mode */
    EX_WIFI_MODE_MAX,
} WIFI_MODE_t;

typedef enum {
    EX_WIFI_AUTH_BEGIN = 0,
    EX_WIFI_AUTH_OPEN,         /**< authenticate mode : open */
    EX_WIFI_AUTH_WEP,              /**< authenticate mode : WEP */
    EX_WIFI_AUTH_WPA_PSK,          /**< authenticate mode : WPA_PSK */
    EX_WIFI_AUTH_WPA2_PSK,         /**< authenticate mode : WPA2_PSK */
    EX_WIFI_AUTH_WPA_WPA2_PSK,     /**< authenticate mode : WPA_WPA2_PSK */
    EX_WIFI_AUTH_WPA2_ENTERPRISE,  /**< authenticate mode : WPA2_ENTERPRISE */
    EX_WIFI_AUTH_WPA3_PSK,         /**< authenticate mode : WPA3_PSK */
    EX_WIFI_AUTH_WPA2_WPA3_PSK,    /**< authenticate mode : WPA2_WPA3_PSK */
    EX_WIFI_AUTH_WAPI_PSK,         /**< authenticate mode : WAPI_PSK */
    EX_WIFI_AUTH_OWE,              /**< authenticate mode : OWE */
    EX_WIFI_AUTH_MAX
} WIFI_AUTH_t;

typedef struct {
    uint8_t ssid[32];      /**< SSID of target AP. */
    uint8_t password[64];  /**< Password of target AP. */
} WIFI_MODE_STA_CONFIG_t;

typedef struct {
    uint8_t ssid[32];           /**< SSID of ESP32 soft-AP. If ssid_len field is 0, this must be a Null terminated string. Otherwise, length is set according to ssid_len. */
    uint8_t password[64];       /**< Password of ESP32 soft-AP. */
} WIFI_MODE_AP_CONFIG_t;

typedef union {
    WIFI_MODE_STA_CONFIG_t sta;
    WIFI_MODE_AP_CONFIG_t ap;
} WIFI_MODE_CONFIG_t;

typedef struct 
{
   uint8_t id[32];
   uint8_t username[32];
   uint8_t password[64]; /* data */
} WIFI_AUTH_WPA2_ENTERPRISE_CONFIG_t ;

typedef union {
    WIFI_AUTH_WPA2_ENTERPRISE_CONFIG_t wpa2ent;
} WIFI_AUTH_CONFIG_t;

typedef struct {
    WIFI_MODE_CONFIG_t mode_cfg;
    WIFI_AUTH_CONFIG_t auth_cfg;
}  WIFI_CONFIG_t;

void srv_wifi_register_mode_config(WIFI_MODE_t mode, WIFI_MODE_CONFIG_t cfg);
void srv_wifi_register_auth_config(WIFI_AUTH_t auth, WIFI_AUTH_CONFIG_t cfg);

void srv_wifi_setup_and_start(WIFI_MODE_t mode, WIFI_AUTH_t auth);