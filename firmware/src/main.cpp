#include <Arduino.h>

#include "event_bus.h"
#include "bluetooth_service.h"

BluetoothService bluetoothService;

void setup() {
    Serial.begin(115200);

    bluetoothService.begin();

    EventBus::subscribe<Temp>([](Temp const& t) {
        Serial.print("fast: ");
        Serial.println(t.serialize());
    });
    EventBus::subscribe<Temp>(1000, [](Temp const& t) {
        Serial.print("throttled: ");
        Serial.println(t.serialize());
    });

    EventBus::subscribe<Command>([](Command const& c) {
        Serial.print("main - command: ");
        Serial.println(c.serialize());
    });
}

void loop() {
    static float v = 0;
    Temp t {.value = v++};
    EventBus::publish<Temp>(t);
    delay(200);
    if (v > 10) {
        while (true) {
            delay(1000);
        }
    }
}