#include <ESP32SvelteKit.h>
#include <PsychicHttp.h>

DRAM_ATTR PsychicHttpServer server;

std::unique_ptr<ESP32SvelteKit> g_ptrApp;

void setup() {
    Serial.begin(115200);
    ESPFS.begin(true);

    g_ptrApp = std::make_unique<ESP32SvelteKit>(&server);
    g_ptrApp->begin();
}

void loop() { vTaskDelete(nullptr); }
