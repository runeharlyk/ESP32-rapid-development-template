#ifndef MqttSettingsService_h
#define MqttSettingsService_h

/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32
 *platforms with responsive Sveltekit front-end built with TailwindCSS and
 *DaisyUI. https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <ESPFS.h>
#include <FSPersistence.h>
#include <PsychicMqttClient.h>
#include <StatefulService.h>
#include <WiFi.h>
#include <stateful_endpoint.h>
#include <domain/mqtt_settings.h>

class MqttSettingsService : public StatefulService<MqttSettings> {
  public:
    MqttSettingsService();
    ~MqttSettingsService();

    void begin();
    void loop();
    bool isEnabled();
    bool isConnected();
    const char *getClientId();
    String getLastError();
    PsychicMqttClient *getMqttClient();

    HttpEndpoint<MqttSettings> endpoint;

    esp_err_t getStatus(PsychicRequest *request) {
        PsychicJsonResponse response = PsychicJsonResponse(request, false);
        JsonObject root = response.getRoot();

        root["enabled"] = isEnabled();
        root["connected"] = isConnected();
        root["client_id"] = getClientId();
        root["last_error"] = getLastError();

        return response.send();
    }

  protected:
    void onConfigUpdated();

  private:
    PsychicHttpServer *_server;
    FSPersistence<MqttSettings> _fsPersistence;

    // Pointers to hold retained copies of the mqtt client connection strings.
    // This is required as AsyncMqttClient holds references to the supplied
    // connection strings.
    char *_retainedHost;
    char *_retainedClientId;
    char *_retainedUsername;
    char *_retainedPassword;

    // variable to help manage connection
    bool _reconfigureMqtt;
    String _lastError;

    // the MQTT client instance
    PsychicMqttClient _mqttClient;

    void onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
    void onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(bool sessionPresent);
    void onMqttError(esp_mqtt_error_codes_t error);
    void configureMqtt();
};

#endif // end MqttSettingsService_h
