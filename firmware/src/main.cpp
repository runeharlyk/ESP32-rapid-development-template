#include <Arduino.h>

#include "event_bus.h"
#include "bluetooth_service.h"

BluetoothService bluetoothService;

void setup() {
    Serial.begin(115200);

    bluetoothService.begin();

    EventBus::subscribe<Temp>([](Temp const& t) {
        Serial.print("fast: ");
        Serial.println(t.value);
    });
    EventBus::subscribe<Temp>(1000, [](Temp const& t) {
        Serial.print("throttled: ");
        Serial.println(t.value);
    });

    EventBus::subscribe<Command>([](Command const& c) {
        Serial.print("main - command: ");
        Serial.println(c.lx);
    });
}

void loop() {
    static float v = 0;
    Command t {
        .lx = v++,
        .ly = v,

    };
    EventBus::publish<Command>(t);
    delay(20);
}