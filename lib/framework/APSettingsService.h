#ifndef APSettingsConfig_h
#define APSettingsConfig_h

#include <ESPFS.h>
#include <FSPersistence.h>
#include <JsonUtils.h>
#include <SettingValue.h>
#include <WiFi.h>
#include <stateful_endpoint.h>

#include <DNSServer.h>
#include <IPAddress.h>

#include <domain/ap_settings.h>

class APSettingsService : public StatefulService<APSettings> {
  public:
    APSettingsService();

    void begin();
    void loop();
    APNetworkStatus getAPNetworkStatus();
    void recoveryMode();

    esp_err_t getStatus(PsychicRequest *request);

    HttpEndpoint<APSettings> endpoint;

  private:
    PsychicHttpServer *_server;
    FSPersistence<APSettings> _fsPersistence;

    // for the captive portal
    DNSServer *_dnsServer;

    // for the mangement delay loop
    volatile unsigned long _lastManaged;
    volatile boolean _reconfigureAp;
    volatile boolean _recoveryMode = false;

    void reconfigureAP();
    void manageAP();
    void startAP();
    void stopAP();
    void handleDNS();
};

#endif // end APSettingsConfig_h
