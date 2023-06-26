#include <globals.h>
#include <deviceconfig.h>
#include <apiclient/weatherclient.h>

ESPTaskManager g_TaskManager;
#if USE_WIFI && USE_WEBSERVER
    DRAM_ATTR CWebServer g_WebServer;
#endif

DRAM_ATTR std::unique_ptr<WeatherApiClient> g_ptrWeatherApiClient;

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  SPIFFS.begin();
  g_TaskManager.begin();
  g_TaskManager.StartThreads();

  g_ptrJSONWriter = std::make_unique<JSONWriter>();
  g_ptrDeviceConfig = std::make_unique<DeviceConfig>();
  g_ptrWeatherApiClient = std::make_unique<WeatherApiClient>(client);
}

void loop() {
  g_ptrWeatherApiClient->getWeatherData();
  delay(10000);
}
