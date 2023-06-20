#ifndef __M_URI_H__
#define __M_URI_H__
#include "esp_http_server.h"
typedef enum {
    URI_KIND_NONE,
    URI_KIND_INDEX,
    URI_KIND_STREAM,
    URI_KIND_SIZE,
} m_uri_kind_t;
void m_uri_init_default(void);
httpd_uri_t *m_uri_get_uri_handler(m_uri_kind_t);
#endif 