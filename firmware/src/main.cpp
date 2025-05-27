#include <Arduino.h>

#include "event_bus.h"
#include "bluetooth_service.h"

BluetoothService bluetoothService;

void* _tempSubHandle;

void setup() {
    Serial.begin(115200);

    bluetoothService.begin();

    _tempSubHandle = EventBus::subscribe<Temp>([](Temp const& t) {
        Serial.print("fast: ");
        Serial.println(t.value);
    });
    EventBus::subscribe<Temp>(1000, [](Temp const& t) {
        Serial.print("throttled: ");
        Serial.println(t.value);
    });

    EventBus::subscribe<Command>([](Command const& c) {
        Serial.print("main - command: ");
    });
}

void loop() {
    static float v = 0;
    Temp t {.value = v++};
    EventBus::publish<Temp>(t, _tempSubHandle);
    delay(200);
}