#pragma once
/*
Per hour
- Time spend in wheel
- Max speed
- Average speed
- Distance traveled


Top stats:
- Total time tracking
- Max overall speed
- Total distance
- Time spend in wheel
*/
#include <ArduinoJson.h>
#include <ESPFS.h>
#include <EventSocket.h>
#include <FSPersistence.h>
#include <WiFi.h>
#include <stateful_endpoint.h>
#include <timing.h>
#include <vector>

#define STEPS_SETTINGS_SERVICE_PATH "/api/v1/steps"

#define EVENT_STEP "step"
#define STEP_INTERVAL 150

#define HALL_SENSOR_PIN 32
#define DEBOUNCE_DELAY 150
#define SESSION_INACTIVITY_DELAY 10000

struct SessionSlot {
    long start;
    long end;
    int steps;
    std::vector<float> times;
};

class PedoMeterData {
    std::vector<SessionSlot> sessions;
    // float diameterOfHamsterWheel = 0.19;
    // float numOfMagnets = 1;
    // float circumference = PI * diameterOfHamsterWheel / numOfMagnets;

  public:
    static void read(PedoMeterData &settings, JsonObject &root) {
        JsonArray sessionsArray = root["sessions"].to<JsonArray>();

        for (auto &session : settings.sessions) {
            JsonObject newSession = sessionsArray.add<JsonObject>();

            newSession["start"] = session.start;
            newSession["end"] = session.end;
            newSession["steps"] = session.steps;

            JsonArray timesArray = newSession["times"].to<JsonArray>();

            for (auto &time : session.times) {
                timesArray.add(time);
            }
        }
    }

    static StateUpdateResult update(JsonObject &root, PedoMeterData &settings) {
        settings.sessions.clear();

        if (root["sessions"].is<JsonArray>()) {
            JsonArray sessionsArray = root["sessions"];

            for (auto jsonSession : sessionsArray) {
                JsonObject session = jsonSession.as<JsonObject>();

                SessionSlot newSession;

                newSession.start = session["start"];
                newSession.end = session["end"];
                newSession.steps = session["steps"];

                if (session["times"].is<JsonArray>()) {
                    JsonArray timesArray = session["times"];
                    for (JsonVariant time : timesArray) {
                        newSession.times.push_back(time.as<float>());
                    }
                }
                settings.sessions.push_back(newSession);
            }
        } else {
            ESP_LOGE("PEDO", "sessions was not an array");
        }

        return StateUpdateResult::CHANGED;
    }
    void updateSession(float timeElapsed) {
        SessionSlot &lastSession = sessions.back();
        lastSession.steps += 1;
        lastSession.times.push_back(timeElapsed);
    }

    void startSession() { sessions.push_back(SessionSlot {.start = time(nullptr)}); }

    void endSession() {
        if (sessions.empty()) return;
        SessionSlot &lastSession = sessions.back();
        lastSession.end = time(nullptr);
    }

    void reset() { sessions.clear(); }
};

class PedoMeter : public StatefulService<PedoMeterData> {
  public:
    PedoMeter(PsychicHttpServer *server, EventSocket *socket)
        : _socket(socket),
          endpoint(PedoMeterData::read, PedoMeterData::update, this),
          _fsPersistence(PedoMeterData::read, PedoMeterData::update, this, STEPS_FILE) {};

    void begin();

    HttpEndpoint<PedoMeterData> endpoint;

  protected:
    FSPersistence<PedoMeterData> _fsPersistence;
    EventSocket *_socket;

    static void _loopImpl(void *_this) { static_cast<PedoMeter *>(_this)->_loop(); }
    void _loop();

    float totalDistance = 0.0;
    const float diameterOfHamsterWheel = 0.19; // cm
    const float pi = 3.14159;
    const float numOfMagnets = 1;
    const float circumference = pi * diameterOfHamsterWheel / numOfMagnets;
};
