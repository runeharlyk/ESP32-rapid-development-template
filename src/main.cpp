#include <globals.h>
#include <deviceconfig.h>

ESPTaskManager g_TaskManager;
#if USE_WIFI && USE_WEBSERVER
    DRAM_ATTR CWebServer g_WebServer;
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  SPIFFS.begin();
  g_TaskManager.begin();
  g_TaskManager.StartThreads();

  g_ptrJSONWriter = std::make_unique<JSONWriter>();
  g_ptrDeviceConfig = std::make_unique<DeviceConfig>();
}

void loop() {
  delay(10);
}
