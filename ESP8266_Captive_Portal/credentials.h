#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#include <IPAddress.h>

// WiFi Settings
extern const char* WIFI_SSID;
extern const char* WIFI_SUBTITLE;

// Deauthentication Settings
extern const bool DEAUTH_ENABLED;
extern const int DEAUTH_INTERVAL;
extern const char* DEAUTH_REASON;

// Web Server Settings  
extern const uint8_t WEB_PORT;
extern const uint8_t DNS_PORT;

// IP Settings
extern IPAddress AP_IP;
extern IPAddress AP_NETMASK;

// LED Settings
extern const uint8_t LED_PIN;
extern const uint8_t BLINK_COUNT;
extern const uint16_t BLINK_DELAY;

// File System Settings
extern const char* DATA_FILE;
extern const char* ADMIN_FILE;
extern const char* DEFAULT_ADMIN_PASSWORD;

// Web Page Titles
extern const char* CREDS_TITLE;
extern const char* POST_TITLE;
extern const char* CLEAR_TITLE;
extern const char* POST_BODY;

#endif