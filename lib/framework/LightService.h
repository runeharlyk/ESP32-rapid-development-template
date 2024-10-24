#pragma once

#include <stateful_endpoint.h>
#include <FSPersistence.h>
#include <EventEndpoint.h>

#include <domain/light_settings.h>

#define EVENT_LIGHT "light"

class LightService : public StatefulService<LightState> {
  public:
    LightService()
        : endpoint(LightState::read, LightState::update, this),
          _fsPersistence(LightState::read, LightState::update, this, LIGHT_FILE),
          _eventEndpoint(LightState::read, LightState::update, this, EVENT_LIGHT) {
        addUpdateHandler([&](const String &originId) { stateChange(); }, false);
        pinMode(LED_BUILTIN, OUTPUT);
        stateChange();
    };

    void stateChange() {
        ESP_LOGI("LightState", "brightness: %d", _state.brightness);
        analogWrite(LED_BUILTIN, _state.brightness);
    }

    HttpEndpoint<LightState> endpoint;

  protected:
    FSPersistence<LightState> _fsPersistence;
    EventEndpoint<LightState> _eventEndpoint;
};
