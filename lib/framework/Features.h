#ifndef Features_h
#define Features_h

#define FT_ENABLED(feature) feature

// mqtt feature on by default
#ifndef USE_MQTT
#define USE_MQTT 1
#endif

// ntp feature on by default
#ifndef USE_NTP
#define USE_NTP 1
#endif

// upload firmware feature off by default
#ifndef USE_UPLOAD_FIRMWARE
#define USE_UPLOAD_FIRMWARE 0
#endif

// download firmware feature off by default
#ifndef USE_DOWNLOAD_FIRMWARE
#define USE_DOWNLOAD_FIRMWARE 0
#endif

// ESP32 sleep states off by default
#ifndef USE_SLEEP
#define USE_SLEEP 0
#endif

// ESP32 analytics on by default
#ifndef USE_ANALYTICS
#define USE_ANALYTICS 1
#endif

#endif
