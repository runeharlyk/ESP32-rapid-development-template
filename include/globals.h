#pragma once

#include <inttypes.h>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <exception>
#include <mutex>
#include <vector>
#include <errno.h>
#include <math.h>
#include <deque>
#include <algorithm>

#include <SPIFFS.h>

/*
 * Macros
 */
#define NAME_OF(x)          #x

/*
 * Feature flags
 */

#include <featureflags.h>

#if USE_WIFI
    #include <WiFi.h>
#endif

#if USE_WIFI && USE_WEBSERVER
    #include <ESPAsyncWebServer.h>
#endif

#if USE_OAT
    #include <ArduinoOTA.h>
#endif

#if USE_DNS_SERVER
    #include <ArduinoOTA.h>
#endif

#if USE_MDNS
    #include <ESPmDNS.h>
#endif


#define STACK_SIZE (ESP_TASK_MAIN_STACK) // Stack size for each new thread

/*
 * Thread priority
 */
#define NET_PRIORITY            tskIDLE_PRIORITY+5
#define JSONWRITER_PRIORITY     tskIDLE_PRIORITY+2



/*
 * Thread core
 */
#define NET_CORE                0
#define JSONWRITER_CORE         0


/*
 * Main include
 */
#include <taskmanager.h>
#include <secrets.h>

#if USE_WIFI && USE_WEBSERVER
    #include <webserver.h>
#endif

#if USE_WIFI
    #include <network.h>
#endif
