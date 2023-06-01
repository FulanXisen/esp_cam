#pragma once 

#include "esp_http_server.h"

httpd_handle_t app_http_start_webserver(void);

void app_http_stop_webserver(httpd_handle_t server);


