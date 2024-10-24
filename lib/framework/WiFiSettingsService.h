#ifndef WiFiSettingsService_h
#define WiFiSettingsService_h

#include <ESPFS.h>
#include <EventSocket.h>
#include <FSPersistence.h>
#include <JsonUtils.h>
#include <PsychicHttp.h>
#include <SettingValue.h>
#include <StatefulService.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <stateful_endpoint.h>
#include <vector>
#include <domain/wifi_settings.h>

#define WIFI_RECONNECTION_DELAY 1000 * 10
#define RSSI_EVENT_DELAY 500

#define EVENT_RSSI "rssi"

namespace wifi_sta {
void networks(JsonObject &root);
void networkStatus(JsonObject &root);

esp_err_t handleScan(PsychicRequest *request);
esp_err_t getNetworks(PsychicRequest *request);
esp_err_t getNetworkStatus(PsychicRequest *request);
} // namespace wifi_sta

class WiFiSettingsService : public StatefulService<WiFiSettings> {
  public:
    WiFiSettingsService();

    void initWiFi();
    void begin();
    void loop();
    String getHostname();

    HttpEndpoint<WiFiSettings> endpoint;

  private:
    FSPersistence<WiFiSettings> _fsPersistence;
    unsigned long _lastConnectionAttempt {0};
    unsigned long _lastRssiUpdate;

    bool _stopping {false};
    void onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
    void onStationModeStop(WiFiEvent_t event, WiFiEventInfo_t info);
    static void onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info);

    void reconfigureWiFiConnection();
    void manageSTA();
    void connectToWiFi();
    void configureNetwork(wifi_settings_t &network);
    void updateRSSI();
};

#endif // end WiFiSettingsService_h
