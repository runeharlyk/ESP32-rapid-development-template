#pragma once

#include <stateful_result.h>
#include <ArduinoJson.h>

#define DEFAULT_BRIGHTNESS 128

class LightState {
  public:
    uint8_t brightness = DEFAULT_BRIGHTNESS;

    static void read(LightState &settings, JsonObject &root) { root["brightness"] = settings.brightness; }

    static StateUpdateResult update(JsonObject &root, LightState &lightState) {
        uint8_t newBrightness = root["brightness"] | DEFAULT_BRIGHTNESS;
        if (lightState.brightness != newBrightness) {
            lightState.brightness = newBrightness;
            return StateUpdateResult::CHANGED;
        }
        return StateUpdateResult::UNCHANGED;
    }
};