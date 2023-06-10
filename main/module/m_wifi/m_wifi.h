#ifndef __M_WIFI_H__
#define __M_WIFI_H__

#include <stdint.h>

#define WIFI_SSID "??"
#define WIFI_IDENTITY "??"
#define WIFI_USERNAME "??"
#define WIFI_PASSWORD "??"

/*
  typedecl of wifi mode: wifi_mode_t
  STA
  AP
  APSTA
*/

/* 
  typedecl of wifi authenticate mode: 
  OPEN
  WEP
  WPA_PSK
  WPA2_PSK
  WPA_WPA2_PSK
  WPA2_ENTERPRISE
  WPA3_PSK
  WPA2_WPA3_PSK
  WAPI_PSK
  OWE
*/

void m_wifi_ap_init(void);
void m_wifi_sta_init(void);
void m_wifi_apsta_init(void);

#endif /* __M_WIFI_H__ */