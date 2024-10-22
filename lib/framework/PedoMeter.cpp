#include <PedoMeter.h>

volatile bool eventTriggered = false;
volatile int count = 0;
volatile float timeElapsed = 0;
unsigned long lastDebounceTime = 0;

void IRAM_ATTR hallSensorInterrupt() {
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime > DEBOUNCE_DELAY) {
        eventTriggered = true;
        count += 1;
        timeElapsed = (currentTime - lastDebounceTime) / 1000.0; // time in seconds
        lastDebounceTime = currentTime;
    }
}

void PedoMeter::begin() {
    _socket->onEvent("reset_pedometer", [&](JsonObject &root, int originId) {
        _state.reset();
        _fsPersistence.writeToFS();
    });

    _fsPersistence.readFromFS();

    pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(HALL_SENSOR_PIN), hallSensorInterrupt, FALLING);

    xTaskCreatePinnedToCore(this->_loopImpl,    // Function that should be called
                            "Pedometer",        // Name of the task (for debugging)
                            5120,               // Stack size (bytes)
                            this,               // Pass reference to this class instance
                            (tskIDLE_PRIORITY), // task priority
                            NULL,               // Task handle
                            1                   // Pin to application core
    );
}

void PedoMeter::_loop() {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    JsonDocument doc;
    bool isInSession = false;

    while (1) {
        unsigned long currentTime = millis();
        if (eventTriggered) {
            eventTriggered = false;

            if (!isInSession) {
                isInSession = true;
                _state.startSession();
            } else {
                _state.updateSession(timeElapsed);
            }

            doc["time_elapsed"] = timeElapsed;

            String output;
            serializeJson(doc, output);
            _socket->emit(EVENT_STEP, output.c_str());
        }

        EXECUTE_EVERY_N_MS(30000,
                           _fsPersistence.writeToFS();); // Save every 30 seconds

        if (currentTime - lastDebounceTime > SESSION_INACTIVITY_DELAY) {
            _state.endSession();
            isInSession = false;
        }

        vTaskDelayUntil(&xLastWakeTime, STEP_INTERVAL / portTICK_PERIOD_MS);
    }
}
