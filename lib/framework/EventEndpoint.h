#ifndef EventEndpoint_h
#define EventEndpoint_h

#include <EventSocket.h>
#include <PsychicHttp.h>
#include <StatefulService.h>

template <class T>
class EventEndpoint {
  public:
    EventEndpoint(JsonStateReader<T> stateReader, JsonStateUpdater<T> stateUpdater, StatefulService<T> *statefulService,
                  const char *event)
        : _stateReader(stateReader), _stateUpdater(stateUpdater), _statefulService(statefulService), _event(event) {
        _statefulService->addUpdateHandler([&](const String &originId) { syncState(originId); }, false);
        socket.onEvent(_event,
                       std::bind(&EventEndpoint::updateState, this, std::placeholders::_1, std::placeholders::_2));
        // socket.onSubscribe(_event, [this](const String &originId) { syncState(originId, true); });
    }

    void begin() {}

  private:
    JsonStateReader<T> _stateReader;
    JsonStateUpdater<T> _stateUpdater;
    StatefulService<T> *_statefulService;
    const char *_event;

    void updateState(JsonObject &root, int originId) {
        ESP_LOGI("EventEndpoint", "updating state");
        _statefulService->update(root, _stateUpdater, String(originId));
    }

    void syncState(const String &originId, bool sync = false) {
        JsonDocument doc;
        JsonObject root = doc.to<JsonObject>();
        _statefulService->read(root, _stateReader);
        String payload;
        serializeJson(doc, payload);
        socket.emit(_event, payload.c_str(), originId.c_str(), sync);
    }
};

#endif
