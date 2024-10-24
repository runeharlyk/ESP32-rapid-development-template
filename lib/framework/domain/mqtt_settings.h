#include <Arduino.h>
#include <ArduinoJson.h>
#include <stateful_result.h>
#include <SettingValue.h>

#ifndef FACTORY_MQTT_ENABLED
#define FACTORY_MQTT_ENABLED false
#endif

#ifndef FACTORY_MQTT_HOST
#define FACTORY_MQTT_HOST "test.mosquitto.org"
#endif

#ifndef FACTORY_MQTT_PORT
#define FACTORY_MQTT_PORT 1883
#endif

#ifndef FACTORY_MQTT_USERNAME
#define FACTORY_MQTT_USERNAME ""
#endif

#ifndef FACTORY_MQTT_PASSWORD
#define FACTORY_MQTT_PASSWORD ""
#endif

#ifndef FACTORY_MQTT_CLIENT_ID
#define FACTORY_MQTT_CLIENT_ID "#{platform}-#{unique_id}"
#endif

#ifndef FACTORY_MQTT_KEEP_ALIVE
#define FACTORY_MQTT_KEEP_ALIVE 16
#endif

#ifndef FACTORY_MQTT_CLEAN_SESSION
#define FACTORY_MQTT_CLEAN_SESSION true
#endif

#ifndef FACTORY_MQTT_MAX_TOPIC_LENGTH
#define FACTORY_MQTT_MAX_TOPIC_LENGTH 128
#endif

#define MQTT_RECONNECTION_DELAY 5000

class MqttSettings {
  public:
    // host and port - if enabled
    bool enabled;
    String uri;

    // username and password
    String username;
    String password;

    // client id settings
    String clientId;

    // connection settings
    uint16_t keepAlive;
    bool cleanSession;

    static void read(MqttSettings &settings, JsonObject &root) {
        root["enabled"] = settings.enabled;
        root["uri"] = settings.uri;
        root["username"] = settings.username;
        root["password"] = settings.password;
        root["client_id"] = settings.clientId;
        root["keep_alive"] = settings.keepAlive;
        root["clean_session"] = settings.cleanSession;
    }

    static StateUpdateResult update(JsonObject &root, MqttSettings &settings) {
        settings.enabled = root["enabled"] | FACTORY_MQTT_ENABLED;
        settings.uri = root["uri"] | FACTORY_MQTT_URI;
        settings.username = root["username"] | SettingValue::format(FACTORY_MQTT_USERNAME);
        settings.password = root["password"] | FACTORY_MQTT_PASSWORD;
        settings.clientId = root["client_id"] | SettingValue::format(FACTORY_MQTT_CLIENT_ID);
        settings.keepAlive = root["keep_alive"] | FACTORY_MQTT_KEEP_ALIVE;
        settings.cleanSession = root["clean_session"] | FACTORY_MQTT_CLEAN_SESSION;
        return StateUpdateResult::CHANGED;
    }
};