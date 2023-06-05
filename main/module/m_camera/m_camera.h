#ifndef __M_CAMERA_H__
#define __M_CAMERA_H__

#include "error.h"
#include "esp_camera.h"
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif


#define BOARD_ESP32CAM_AITHINKER 1

/* WROVER-KIT PIN Map */
#ifdef BOARD_WROVER_KIT

#define CAM_PIN_PWDN -1  //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 21
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 19
#define CAM_PIN_D2 18
#define CAM_PIN_D1 5
#define CAM_PIN_D0 4
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

/*  ESP32Cam (AiThinker) PIN Map */
#ifdef BOARD_ESP32CAM_AITHINKER
#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 /* software reset will be performed */
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22
#endif

typedef error_t (*camera_fb_cb_t)(camera_fb_t *);

error_t m_camera_default_init(void);
error_t m_camera_init(const camera_config_t *config);
error_t m_camera_capture_once(camera_fb_cb_t cb);

#endif 