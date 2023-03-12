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

static const char *TAG="main";
// typedef struct {
//         httpd_req_t *req;
//         size_t len;
// } jpg_chunking_t;

// static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len){
//     jpg_chunking_t *j = (jpg_chunking_t *)arg;
//     if(!index){
//         j->len = 0;
//     }
//     if(httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK){
//         return 0;
//     }
//     j->len += len;
//     return len;
// }

// esp_err_t jpg_httpd_handler(httpd_req_t *req){
//     camera_fb_t * fb = NULL;
//     esp_err_t res = ESP_OK;
//     size_t fb_len = 0;
//     int64_t fr_start = esp_timer_get_time();

//     fb = esp_camera_fb_get();
//     if (!fb) {
//         ESP_LOGE(TAG, "Camera capture failed");
//         httpd_resp_send_500(req);
//         return ESP_FAIL;
//     }
//     res = httpd_resp_set_type(req, "image/jpeg");
//     if(res == ESP_OK){
//         res = httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
//     }

//     if(res == ESP_OK){
//         if(fb->format == PIXFORMAT_JPEG){
//             fb_len = fb->len;
//             res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
//         } else {
//             jpg_chunking_t jchunk = {req, 0};
//             res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk)?ESP_OK:ESP_FAIL;
//             httpd_resp_send_chunk(req, NULL, 0);
//             fb_len = jchunk.len;
//         }
//     }
//     esp_camera_fb_return(fb);
//     int64_t fr_end = esp_timer_get_time();
//     ESP_LOGI(TAG, "JPG: %luKB %lums", (uint32_t)(fb_len/1024), (uint32_t)((fr_end - fr_start)/1000));
//     return res;
// }

void app_main()
{
    if(ESP_OK != init_camera()) {
        return;
    }
    ESP_ERROR_CHECK(nvs_flash_init());
    app_wifi_init();
    while (1)
    {
        ESP_LOGI(TAG, "Taking picture...");
        camera_fb_t *pic = esp_camera_fb_get();

        // use pic->buf to access the image
        ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);
        esp_camera_fb_return(pic);

        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}
