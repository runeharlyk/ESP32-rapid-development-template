#pragma once
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "event_bus.h"
#include "message_types.h"

#define SERVICE_UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"

class BluetoothService {
    BLEServer* _server;
    BLECharacteristic* _txCharacteristic;
    BLECharacteristic* _rxCharacteristic;
    bool _deviceConnected;
    void* _cmdSubHandle;

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
    ~BluetoothService() { EventBus::unsubscribe<Command>(_cmdSubHandle); }

    void begin() {
        _cmdSubHandle = EventBus::subscribe<Command>(0, [this](Command const& cmd) { sendToWebApp(cmd); });
        setup();
    }

    void onWebAppCommand(Command const& cmd) { EventBus::publish<Command>(cmd, _cmdSubHandle); }

    void sendToWebApp(Command const& cmd) {}

    void restart();
    void setup();
    void handleReceivedData(const std::string& data);

    void sendData(const std::string& data);
    void sendData(uint8_t* data, size_t length);
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

void BluetoothService::handleReceivedData(const std::string& data) {
    ESP_LOGI("BluetoothService", "Received: %s", data.c_str());

    std::string response = "ACK: " + data;
    sendData(response);
}

void BluetoothService::sendData(const std::string& data) {
    if (_deviceConnected && _txCharacteristic) {
        _txCharacteristic->setValue(data);
        _txCharacteristic->notify();
        ESP_LOGI("BluetoothService", "Sent: %s", data.c_str());
    } else {
        ESP_LOGW("BluetoothService", "Cannot send data, no device connected or TX characteristic invalid.");
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

void BluetoothService::sendData(uint8_t* data, size_t length) {
    if (_deviceConnected && _txCharacteristic) {
        _txCharacteristic->setValue(data, length);
        _txCharacteristic->notify();
        ESP_LOGI("BluetoothService", "Sent %d bytes", length);
    } else {
        ESP_LOGW("BluetoothService", "Cannot send data, no device connected or TX characteristic invalid.");
    }
}