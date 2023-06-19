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
