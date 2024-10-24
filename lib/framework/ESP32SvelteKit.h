#ifndef ESP32SvelteKit_h
#define ESP32SvelteKit_h

#include <APSettingsService.h>
#include <AnalyticsService.h>
#include <Arduino.h>
#include <DownloadFirmwareService.h>
#include <ESPFS.h>
#include <ESPmDNS.h>
#include <EventSocket.h>
#include <features_service.h>
#include <MqttSettingsService.h>
#include <NTPSettingsService.h>
#include <PsychicHttp.h>
#include <UploadFirmwareService.h>
#include <WiFi.h>
#include <WiFiSettingsService.h>
#include <system_service.h>
#include <filesystem_service.h>
#include <LightService.h>

#include <WWWData.h>

#ifndef CORS_ORIGIN
#define CORS_ORIGIN "*"
#endif

#ifndef APP_VERSION
#define APP_VERSION "demo"
#endif

#ifndef APP_NAME
#define APP_NAME "ESP32 SvelteKit Demo"
#endif

class ESP32SvelteKit {
  public:
    ESP32SvelteKit(PsychicHttpServer *server);

    void begin();

    PsychicHttpServer *getServer() { return _server; }

    StatefulService<WiFiSettings> *getWiFiSettingsService() { return &_wifiSettingsService; }

    StatefulService<APSettings> *getAPSettingsService() { return &_apSettingsService; }

#if FT_ENABLED(USE_NTP)
    StatefulService<NTPSettings> *getNTPSettingsService() { return &_ntpSettingsService; }
#endif

#if FT_ENABLED(USE_MQTT)
    StatefulService<MqttSettings> *getMqttSettingsService() { return &_mqttSettingsService; }

    PsychicMqttClient *getMqttClient() { return _mqttSettingsService.getMqttClient(); }
#endif

    void setMDNSAppName(String name) { _appName = name; }

    void recoveryMode() { _apSettingsService.recoveryMode(); }

  private:
    PsychicHttpServer *_server;
    WiFiSettingsService _wifiSettingsService;
    APSettingsService _apSettingsService;
#if FT_ENABLED(USE_NTP)
    NTPSettingsService _ntpSettingsService;
#endif
#if FT_ENABLED(USE_UPLOAD_FIRMWARE)
    UploadFirmwareService _uploadFirmwareService;
#endif
#if FT_ENABLED(USE_DOWNLOAD_FIRMWARE)
    DownloadFirmwareService _downloadFirmwareService;
#endif
#if FT_ENABLED(USE_MQTT)
    MqttSettingsService _mqttSettingsService;
#endif
#if FT_ENABLED(USE_ANALYTICS)
    AnalyticsService _analyticsService;
#endif

    LightService lightService;

    String _appName = APP_NAME;

    const u_int16_t _numberEndpoints = 115;
    const u_int32_t _maxFileUpload = 2300000; // 2.3 MB
    const uint16_t _port = 80;

  protected:
    static void _loopImpl(void *_this) { static_cast<ESP32SvelteKit *>(_this)->_loop(); }
    void _loop();
    void setupServer();
    void setupMDNS();
    void startServices();
};

#endif
