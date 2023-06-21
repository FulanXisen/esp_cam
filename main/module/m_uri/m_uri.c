#include <esp_timer.h>
#include <esp_log.h>
#include <esp_http_server.h>
#include <esp_camera.h>

#include "m_uri.h"

/** BEGIN internal Type Defs **/
typedef struct{
    size_t size;  //number of values used for filtering
    size_t index; //current value index
    size_t count; //value count
    int sum;
    int *values; //array to be filled with values
} ra_filter_t;
/** END internal Type Defs **/

/* BEGIN internal Func Decls BEGIN */
static esp_err_t uri_stream_handler(httpd_req_t *req);
static esp_err_t uri_capture_handler(httpd_req_t *req);
/* END internal Func Decls END */

/** BEGIN internal Var Defs **/
static const char * DEFAULT_TAG = "mURI";
static ra_filter_t ra_filter;
httpd_uri_t URI_HANDLE_TBL[URI_KIND_SIZE] = {
    [URI_KIND_CAPTURE] = {
        .uri = "/capture",
        .method = HTTP_GET,
        .handler = uri_capture_handler,
        .user_ctx = NULL,
    },
    [URI_KIND_STREAM] = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = uri_stream_handler,
        .user_ctx = NULL,
    },
};
/** END intertnal Var Defs **/


/** BEGIN internal Func Defs **/
static ra_filter_t *ra_filter_init(ra_filter_t *filter, size_t sample_size)
{
    memset(filter, 0, sizeof(ra_filter_t));

    filter->values = (int *)malloc(sample_size * sizeof(int));
    if (!filter->values)
    {
        return NULL;
    }
    memset(filter->values, 0, sample_size * sizeof(int));

    filter->size = sample_size;
    return filter;
}

static int ra_filter_run(ra_filter_t *filter, int value)
{
    if (!filter->values)
    {
        return value;
    }
    filter->sum -= filter->values[filter->index];
    filter->values[filter->index] = value;
    filter->sum += filter->values[filter->index];
    filter->index++;
    filter->index = filter->index % filter->size;
    if (filter->count < filter->size)
    {
        filter->count++;
    }
    return filter->sum / filter->count;
}

static esp_err_t uri_capture_handler(httpd_req_t *req){
    // extern const unsigned char index_ov2640_html_gz_start[] asm("_binary_index_ov2640_html_gz_start");
    // extern const unsigned char index_ov2640_html_gz_end[] asm("_binary_index_ov2640_html_gz_end");
    // size_t index_ov2640_html_gz_len = index_ov2640_html_gz_end - index_ov2640_html_gz_start;

  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  size_t fb_len = 0;
  int64_t fr_start = esp_timer_get_time();
  fb = esp_camera_fb_get();
  if (!fb){
    ESP_LOGE(DEFAULT_TAG, "Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  res = httpd_resp_set_type(req, "image/jpeg");
  if (res == ESP_OK){
    res = httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
  }

  if (res == ESP_OK){
    fb_len = fb->len;
    res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  }
  esp_camera_fb_return(fb);
  int64_t fr_end = esp_timer_get_time();
  ESP_LOGI(DEFAULT_TAG, "JPG: %luKB %lums", (uint32_t)(fb_len / 1024), (uint32_t)((fr_end - fr_start) / 1000));
  return res;
    // httpd_resp_set_type(req, "text/html");
    // httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    // sensor_t *s = esp_camera_sensor_get();
    // assert(s != NULL);
    // assert(s->id.PID == OV2640_PID);
    //return httpd_resp_send(req, (const char *)index_ov2640_html_gz_start, index_ov2640_html_gz_len);
}

static esp_err_t uri_stream_handler(httpd_req_t *req){
#define PART_BOUNDARY "123456789000000000000987654321"

    static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
    static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
    static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";
    camera_fb_t *fb = NULL;
    struct timeval _timestamp;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[128];

    static int64_t last_frame = 0;
    if (!last_frame)
    {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
    {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "60");

    while (true)
    {
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(DEFAULT_TAG, "Camera capture failed");
            res = ESP_FAIL;
        }
        else
        {
            _timestamp.tv_sec = fb->timestamp.tv_sec;
            _timestamp.tv_usec = fb->timestamp.tv_usec;
            if (fb->format != PIXFORMAT_JPEG)
            {
                bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                esp_camera_fb_return(fb);
                fb = NULL;
                if (!jpeg_converted)
                {
                    ESP_LOGE(DEFAULT_TAG, "JPEG compression failed");
                    res = ESP_FAIL;
                }
            }
            else
            {
                _jpg_buf_len = fb->len;
                _jpg_buf = fb->buf;
            }
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if (res == ESP_OK)
        {
            size_t hlen = snprintf((char *)part_buf, 128, _STREAM_PART, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if (fb)
        {
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        }
        else if (_jpg_buf)
        {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if (res != ESP_OK)
        {
            break;
        }
        int64_t fr_end = esp_timer_get_time();
        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        uint32_t avg_frame_time = ra_filter_run(&ra_filter, frame_time);

        ESP_LOGI(DEFAULT_TAG, "MJPG: %luB %lums (%.1ffps), AVG: %lums (%.1ffps)",
                 (uint32_t)(_jpg_buf_len),
                 (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time,
                 avg_frame_time, 1000.0 / avg_frame_time);
        // ESP_LOGI(TAG, "Free heap: %u", xPortGetFreeHeapSize());//Memory monitoring
    }

    last_frame = 0;
    return res;
}
/*** END Private Func Defs ***/


/* BEGIN Public Function Definition */
void m_uri_init_default(void){
    ra_filter_init(&ra_filter, 20);
}

httpd_uri_t *m_uri_get_uri_handler(m_uri_kind_t kind ) {
    return &URI_HANDLE_TBL[kind];
}
/* END Public Function Definition */
