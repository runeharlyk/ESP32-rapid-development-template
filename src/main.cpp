#include <globals.h>

ESPTaskManager g_TaskManager;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  SPIFFS.begin();
  g_TaskManager.begin();
  g_TaskManager.StartThreads();
}

void loop() {
  delay(10);
}
