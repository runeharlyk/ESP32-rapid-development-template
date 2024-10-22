#ifndef NTPSettingsService_h
#define NTPSettingsService_h

#include <ESPFS.h>
#include <FSPersistence.h>
#include <WiFi.h>
#include <stateful_endpoint.h>

#include <lwip/apps/sntp.h>
#include <time.h>

#ifndef FACTORY_NTP_ENABLED
#define FACTORY_NTP_ENABLED true
#endif

#ifndef FACTORY_NTP_TIME_ZONE_LABEL
#define FACTORY_NTP_TIME_ZONE_LABEL "Europe/London"
#endif

#ifndef FACTORY_NTP_TIME_ZONE_FORMAT
#define FACTORY_NTP_TIME_ZONE_FORMAT "GMT0BST,M3.5.0/1,M10.5.0"
#endif

#ifndef FACTORY_NTP_SERVER
#define FACTORY_NTP_SERVER "time.google.com"
#endif

class NTPSettings {
  public:
    bool enabled;
    String tzLabel;
    String tzFormat;
    String server;

    static void read(NTPSettings &settings, JsonObject &root) {
        root["enabled"] = settings.enabled;
        root["server"] = settings.server;
        root["tz_label"] = settings.tzLabel;
        root["tz_format"] = settings.tzFormat;
    }

    static StateUpdateResult update(JsonObject &root, NTPSettings &settings) {
        settings.enabled = root["enabled"] | FACTORY_NTP_ENABLED;
        settings.server = root["server"] | FACTORY_NTP_SERVER;
        settings.tzLabel = root["tz_label"] | FACTORY_NTP_TIME_ZONE_LABEL;
        settings.tzFormat = root["tz_format"] | FACTORY_NTP_TIME_ZONE_FORMAT;
        return StateUpdateResult::CHANGED;
    }
};

class NTPSettingsService : public StatefulService<NTPSettings> {
  public:
    NTPSettingsService(PsychicHttpServer *server);

    void begin();
    static esp_err_t getStatus(PsychicRequest *request);
    static esp_err_t handleTime(PsychicRequest *request, JsonVariant &json);

    HttpEndpoint<NTPSettings> endpoint;

  private:
    PsychicHttpServer *_server;
    FSPersistence<NTPSettings> _fsPersistence;

    void onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
    void onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
    void configureNTP();
};

#endif // end NTPSettingsService_h
