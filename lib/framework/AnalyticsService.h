#pragma once

#include <ArduinoJson.h>
#include <ESPFS.h>
#include <EventSocket.h>
#include <WiFi.h>
#include <timing.h>

#define MAX_ESP_ANALYTICS_SIZE 1024
#define EVENT_ANALYTICS "analytics"
#define ANALYTICS_INTERVAL 2000

class AnalyticsService {
  public:
    AnalyticsService(EventSocket *socket) : _socket(socket) {};

    void loop() { EXECUTE_EVERY_N_MS(ANALYTICS_INTERVAL, updateAnalytics()); };

  private:
    EventSocket *_socket;
    JsonDocument doc;
    char message[MAX_ESP_ANALYTICS_SIZE];

    void updateAnalytics() {
        if (!_socket->hasSubscribers(EVENT_ANALYTICS)) return;
        doc.clear();
        doc["uptime"] = millis() / 1000;
        doc["free_heap"] = ESP.getFreeHeap();
        doc["total_heap"] = ESP.getHeapSize();
        doc["min_free_heap"] = ESP.getMinFreeHeap();
        doc["max_alloc_heap"] = ESP.getMaxAllocHeap();
        doc["fs_used"] = ESPFS.usedBytes();
        doc["fs_total"] = ESPFS.totalBytes();
        doc["core_temp"] = temperatureRead();

        serializeJson(doc, message);
        _socket->emit(EVENT_ANALYTICS, message);
    }
};
