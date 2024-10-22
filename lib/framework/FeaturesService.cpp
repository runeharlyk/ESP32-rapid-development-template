#include <FeaturesService.h>

namespace feature_service {

void features(JsonObject &root) {
#if FT_ENABLED(USE_MQTT)
    root["mqtt"] = true;
#else
    root["mqtt"] = false;
#endif
#if FT_ENABLED(USE_NTP)
    root["ntp"] = true;
#else
    root["ntp"] = false;
#endif
#if FT_ENABLED(USE_UPLOAD_FIRMWARE)
    root["upload_firmware"] = true;
#else
    root["upload_firmware"] = false;
#endif
#if FT_ENABLED(USE_DOWNLOAD_FIRMWARE)
    root["download_firmware"] = true;
#else
    root["download_firmware"] = false;
#endif
#if FT_ENABLED(USE_SLEEP)
    root["sleep"] = true;
#else
    root["sleep"] = false;
#endif
#if FT_ENABLED(USE_ANALYTICS)
    root["analytics"] = true;
#else
    root["analytics"] = false;
#endif

    root["firmware_version"] = APP_VERSION;
    root["firmware_name"] = APP_NAME;
    root["firmware_built_target"] = BUILD_TARGET;
}

esp_err_t getFeatures(PsychicRequest *request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, false);
    JsonObject root = response.getRoot();
    features(root);
    return response.send();
}

} // namespace feature_service
