#ifndef FeaturesService_h
#define FeaturesService_h

#include <Features.h>

#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <WiFi.h>

namespace feature_service {

void features(JsonObject &root);

esp_err_t getFeatures(PsychicRequest *request);

} // namespace feature_service

#endif
