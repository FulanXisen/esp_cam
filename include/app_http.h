#pragma once 

#include "esp_http_server.h"

esp_err_t jpg_httpd_handler(httpd_req_t *req);

httpd_handle_t app_http_start_webserver(void);

void app_http_stop_webserver(httpd_handle_t server);


