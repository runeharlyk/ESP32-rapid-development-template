#pragma once
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "event_bus.h"
#include "message_types.h"

#include <map>
#include <list>
#include <vector>

#define SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"

#define CALLS_PER_SECOND(name)                                            \
    static unsigned long name##_count = 0;                                \
    static unsigned long last_time = 0;                                   \
    name##_count++;                                                       \
    if (millis() - last_time >= 1000) {                                   \
        Serial.printf("%s: %lu calls per second\n", #name, name##_count); \
        name##_count = 0;                                                 \
        last_time = millis();                                             \
    }

/////////////////////

enum message_type_t { CONNECT = 0, DISCONNECT = 1, EVENT = 2, PING = 3, PONG = 4 };

class CommAdapterBase {
  public:
    using EventCallback = std::function<void(const JsonObject&, int)>;
    using SubscribeCallback = std::function<void(const String&, bool)>;

    CommAdapterBase() { mutex_ = xSemaphoreCreateMutex(); }
    ~CommAdapterBase() { vSemaphoreDelete(mutex_); }

    void subscribe(message_topic_t t, int cid) {
        xSemaphoreTake(mutex_, portMAX_DELAY);
        subs_[t].push_back(cid);
        xSemaphoreGive(mutex_);
        for (auto& cb : subCbs_[t]) cb(String(cid), true);
    }

    void unsubscribe(message_topic_t t, int cid) {
        xSemaphoreTake(mutex_, portMAX_DELAY);
        subs_[t].remove(cid);
        xSemaphoreGive(mutex_);
        for (auto& cb : subCbs_[t]) cb(String(cid), false);
    }

    bool hasSubscribers(message_topic_t t) {
        xSemaphoreTake(mutex_, portMAX_DELAY);
        bool r = !subs_[t].empty();
        xSemaphoreGive(mutex_);
        return r;
    }

    void onEvent(message_topic_t t, EventCallback cb) { eventCbs_[t].push_back(std::move(cb)); }
    void onSubscribe(message_topic_t t, SubscribeCallback cb) { subCbs_[t].push_back(std::move(cb)); }

    template <typename T>
    void emit(message_topic_t topic, T const& payload) {
        if (!hasSubscribers(topic)) return;

        JsonDocument doc;
        JsonArray array = doc.to<JsonArray>();
        array.add((int)EVENT);
        array.add((int)topic);
        JsonObject obj = array.add<JsonObject>();
        toJson(obj, payload);

        String out;
#if USE_MSGPACK
        serializeMsgPack(doc, out);
#else
        serializeJson(doc, out);
#endif

        send(out.c_str());
    }

  protected:
    virtual void send(const char* data) = 0;

  private:
    SemaphoreHandle_t mutex_;
    std::map<message_topic_t, std::list<int>> subs_;
    std::map<message_topic_t, std::vector<EventCallback>> eventCbs_;
    std::map<message_topic_t, std::vector<SubscribeCallback>> subCbs_;
};

/////////////////////

class BluetoothService : public CommAdapterBase {
    BLEServer* _server;
    BLECharacteristic* _txCharacteristic;
    BLECharacteristic* _rxCharacteristic;
    bool _deviceConnected;
    void* _cmdSubHandle;
    void* _tempSubHandle;

    class ServerCallbacks : public BLEServerCallbacks {
        BluetoothService* _service;

      public:
        ServerCallbacks(BluetoothService* service) : _service(service) {}
        void onConnect(BLEServer* pServer) override;
        void onDisconnect(BLEServer* pServer) override;
    };

    class RXCallbacks : public BLECharacteristicCallbacks {
        BluetoothService* _service;

      public:
        RXCallbacks(BluetoothService* service) : _service(service) {}
        void onWrite(BLECharacteristic* characteristic) override;
    };

  public:
    BluetoothService()
        : _server(nullptr), _txCharacteristic(nullptr), _rxCharacteristic(nullptr), _deviceConnected(false) {}
    ~BluetoothService() {
        EventBus::unsubscribe<Command>(_cmdSubHandle);
        EventBus::unsubscribe<Temp>(_tempSubHandle);
    }

    void begin() {
        _cmdSubHandle = EventBus::subscribe<Command>([this](Command const& c) {
            if (_deviceConnected) emit(COMMAND, c);
        });

        _tempSubHandle = EventBus::subscribe<Temp>([this](Temp const& t) {
            if (_deviceConnected) emit(TEMP, t);
        });

        setup();
    }

    void handleReceivedData(const std::string& data) {
        CALLS_PER_SECOND(new_message);

        JsonDocument doc;
#if USE_MSGPACK
        DeserializationError error = deserializeMsgPack(doc, data);
#else
        DeserializationError error = deserializeJson(doc, s);
#endif
        if (error) {
            throw std::runtime_error(error.c_str());
        }
        JsonArray obj = doc.as<JsonArray>();

        message_type_t type = obj[0].as<message_type_t>();

        int cid = 0;

        switch (type) {
            case CONNECT: {
                message_topic_t topic = obj[1].as<message_topic_t>();
                ESP_LOGI("BluetoothService", "Connecting to topic: %d", topic);
                subscribe(topic, cid);
                break;
            }
            case DISCONNECT: {
                message_topic_t topic = obj[1].as<message_topic_t>();
                ESP_LOGI("BluetoothService", "Disconnecting to topic: %d", topic);
                unsubscribe(topic, cid);
                break;
            }

            case EVENT: {
                message_topic_t topic = obj[1].as<message_topic_t>();
                if (topic == TEMP) {
                    Temp payload;
                    payload.fromJson(obj[2]);
                    EventBus::publish<Temp>(payload, _tempSubHandle);
                } else if (topic == COMMAND) {
                    Command payload;
                    payload.fromJson(obj[2]);
                    EventBus::publish<Command>(payload, _tempSubHandle);
                };
                ESP_LOGD("BluetoothService", "Got payload for topic: %d", msgTopic);
                break;
            }

            default: ESP_LOGW("BluetoothService", "Uknown message type: %d", type); break;
        }
    }

    void restart();
    void setup();

    void send(const char* data) override {
        if (_deviceConnected) {
            _txCharacteristic->setValue((uint8_t*)data, strlen(data));
            _txCharacteristic->notify();
        }
    }
};

void BluetoothService::ServerCallbacks::onConnect(BLEServer* pServer) {
    _service->_deviceConnected = true;
    ESP_LOGI("BluetoothService", "Client connected");
}

void BluetoothService::ServerCallbacks::onDisconnect(BLEServer* pServer) {
    _service->_deviceConnected = false;
    ESP_LOGI("BluetoothService", "Client disconnected");
    pServer->startAdvertising();
    ESP_LOGI("BluetoothService", "Restarting advertising");
}

void BluetoothService::RXCallbacks::onWrite(BLECharacteristic* characteristic) {
    std::string value = characteristic->getValue();
    if (!value.empty()) {
        _service->handleReceivedData(value);
    }
}

void BluetoothService::restart() {
    if (_server) {
        BLEDevice::deinit(true);
        _server = nullptr;
        _txCharacteristic = nullptr;
        _rxCharacteristic = nullptr;
    }
    setup();
}

void BluetoothService::setup() {
    char deviceName[32] = "Hexapod";
    ESP_LOGI("BluetoothService", "Initializing BLE with device name: %s", deviceName);

    BLEDevice::init(deviceName);
    _server = BLEDevice::createServer();
    _server->setCallbacks(new ServerCallbacks(this));

    BLEService* service = _server->createService(SERVICE_UUID);

    _txCharacteristic = service->createCharacteristic(CHARACTERISTIC_TX, BLECharacteristic::PROPERTY_NOTIFY);
    _txCharacteristic->addDescriptor(new BLE2902());

    _rxCharacteristic = service->createCharacteristic(CHARACTERISTIC_RX, BLECharacteristic::PROPERTY_WRITE);
    _rxCharacteristic->setCallbacks(new RXCallbacks(this));

    service->start();
    _server->getAdvertising()->start();

    ESP_LOGI("BluetoothService", "BLE UART service started, advertising as %s", deviceName);
}