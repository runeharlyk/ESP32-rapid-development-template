#include <ESP32SvelteKit.h>
#include <PsychicHttpServer.h>

DRAM_ATTR PsychicHttpServer server;

DRAM_ATTR ESP32SvelteKit esp32sveltekit(&server);

void setup() {
    Serial.begin(115200);

    esp32sveltekit.begin();
}

void loop() { vTaskDelete(nullptr); }
