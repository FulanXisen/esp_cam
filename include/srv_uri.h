#pragma once 

typedef enum{
    URT_SRV_FIRST,
    URI_SRV_INDEX,
    URI_SRV_STREAM,
    URI_SRV_LAST,
} uri_srv_t;

void app_srv_uri_init(void);
httpd_uri_t *app_srv_uri_srv_get(uri_srv_t);

