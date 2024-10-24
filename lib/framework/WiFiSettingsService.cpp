#include <WiFiSettingsService.h>

static const char *TAG = "Wifi Service";

WiFiSettingsService::WiFiSettingsService()
    : endpoint(WiFiSettings::read, WiFiSettings::update, this),
      _fsPersistence(WiFiSettings::read, WiFiSettings::update, this, WIFI_SETTINGS_FILE),
      _lastConnectionAttempt(0) {
    addUpdateHandler([&](const String &originId) { reconfigureWiFiConnection(); }, false);
}

void WiFiSettingsService::initWiFi() {
    WiFi.mode(WIFI_MODE_STA);

    // Disable WiFi config persistance and auto reconnect
    WiFi.persistent(false);
    WiFi.setAutoReconnect(false);

    WiFi.onEvent(
        std::bind(&WiFiSettingsService::onStationModeDisconnected, this, std::placeholders::_1, std::placeholders::_2),
        WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.onEvent(std::bind(&WiFiSettingsService::onStationModeStop, this, std::placeholders::_1, std::placeholders::_2),
                 WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_STOP);
    WiFi.onEvent(onStationModeGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

    if (!_state.wifiSettings.empty()) {
        configureNetwork(_state.wifiSettings.front());
    }
}

void WiFiSettingsService::begin() {}

void WiFiSettingsService::reconfigureWiFiConnection() {
    // reset last connection attempt to force loop to reconnect immediately
    _lastConnectionAttempt = 0;

    // disconnect and de-configure wifi
    if (WiFi.disconnect(true)) {
        _stopping = true;
    }
}

void WiFiSettingsService::loop() {
    unsigned long currentMillis = millis();
    if (!_lastConnectionAttempt || (unsigned long)(currentMillis - _lastConnectionAttempt) >= WIFI_RECONNECTION_DELAY) {
        _lastConnectionAttempt = currentMillis;
        manageSTA();
    }

    if (!_lastRssiUpdate || (unsigned long)(currentMillis - _lastRssiUpdate) >= RSSI_EVENT_DELAY) {
        _lastRssiUpdate = currentMillis;
        updateRSSI();
    }
}

String WiFiSettingsService::getHostname() { return _state.hostname; }

void WiFiSettingsService::manageSTA() {
    // Abort if already connected, or if we have no SSID
    if (WiFi.isConnected() || _state.wifiSettings.empty()) {
        return;
    }

    // Connect or reconnect as required
    if ((WiFi.getMode() & WIFI_STA) == 0) {
        ESP_LOGI(TAG, "Connecting to WiFi...");
        connectToWiFi();
    }
}

void WiFiSettingsService::connectToWiFi() {
    // reset availability flag for all stored networks
    for (auto &network : _state.wifiSettings) {
        network.available = false;
    }

    // scanning for available networks
    int scanResult = WiFi.scanNetworks();
    if (scanResult == WIFI_SCAN_FAILED) {
        ESP_LOGE("WiFiSettingsService", "WiFi scan failed.");
    } else if (scanResult == 0) {
        ESP_LOGW("WiFiSettingsService", "No networks found.");
    } else {
        ESP_LOGI("WiFiSettingsService", "%d networks found.", scanResult);

        // find the best network to connect
        wifi_settings_t *bestNetwork = NULL;
        int bestNetworkDb = FACTORY_WIFI_RSSI_THRESHOLD;

        for (int i = 0; i < scanResult; ++i) {
            String ssid_scan;
            int32_t rssi_scan;
            uint8_t sec_scan;
            uint8_t *BSSID_scan;
            int32_t chan_scan;

            WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan);
            ESP_LOGV("WiFiSettingsService", "SSID: %s, BSSID: " MACSTR ", RSSI: %d dbm, Channel: %d", ssid_scan.c_str(),
                     MAC2STR(BSSID_scan), rssi_scan, chan_scan);

            for (auto &network : _state.wifiSettings) {
                if (ssid_scan == network.ssid) {     // SSID match
                    if (rssi_scan > bestNetworkDb) { // best network
                        bestNetworkDb = rssi_scan;
                        ESP_LOGV("WiFiSettingsService", "--> New best network SSID: %s, BSSID: " MACSTR "",
                                 ssid_scan.c_str(), MAC2STR(BSSID_scan));
                        network.available = true;
                        network.channel = chan_scan;
                        memcpy(network.bssid, BSSID_scan, 6);
                        bestNetwork = &network;
                    } else if (rssi_scan >= FACTORY_WIFI_RSSI_THRESHOLD &&
                               network.available == false) { // available network
                        network.available = true;
                        network.channel = chan_scan;
                        memcpy(network.bssid, BSSID_scan, 6);
                    }
                }
                break;
            }
        }

        // if configured to prioritize signal strength, use the best network else
        // use the first available network
        if (_state.priorityBySignalStrength == false) {
            for (auto &network : _state.wifiSettings) {
                if (network.available == true) {
                    ESP_LOGI("WiFiSettingsService", "Connecting to first available network: %s", network.ssid.c_str());
                    configureNetwork(network);
                    break;
                }
            }
        } else if (_state.priorityBySignalStrength == true && bestNetwork) {
            ESP_LOGI("WiFiSettingsService", "Connecting to strongest known network: %s", bestNetwork->ssid.c_str());
            configureNetwork(*bestNetwork);
        } else // no suitable network to connect
        {
            ESP_LOGI("WiFiSettingsService", "No known networks found.");
        }

        // delete scan results
        WiFi.scanDelete();
    }
}

void WiFiSettingsService::configureNetwork(wifi_settings_t &network) {
    if (network.staticIPConfig) {
        // configure for static IP
        WiFi.config(network.localIP, network.gatewayIP, network.subnetMask, network.dnsIP1, network.dnsIP2);
    } else {
        // configure for DHCP
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
    }
    WiFi.setHostname(_state.hostname.c_str());

    // attempt to connect to the network
    ESP_LOGI(TAG, "Connection to wifi: %s", network.ssid.c_str());
    WiFi.begin(network.ssid.c_str(), network.password.c_str());

    delay(750);

#if CONFIG_IDF_TARGET_ESP32C3
    WiFi.setTxPower(WIFI_POWER_8_5dBm); // https://www.wemos.cc/en/latest/c3/c3_mini_1_0_0.html#about-wifi
#endif
}

void WiFiSettingsService::updateRSSI() {
    if (!socket.hasSubscribers("rssi")) return;
    char buffer[8];
    snprintf(buffer, sizeof(buffer), "%d", WiFi.RSSI());
    socket.emit("rssi", buffer);
}

void WiFiSettingsService::onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) { WiFi.disconnect(true); }
void WiFiSettingsService::onStationModeStop(WiFiEvent_t event, WiFiEventInfo_t info) {
    if (_stopping) {
        _lastConnectionAttempt = 0;
        _stopping = false;
    }
}

void WiFiSettingsService::onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    ESP_LOGI("WiFiStatus", "WiFi Got IP. localIP=%s, hostName=%s", WiFi.localIP().toString().c_str(),
             WiFi.getHostname());
}

namespace wifi_sta {
void networks(JsonObject &root) {
    JsonArray networks = root["networks"].to<JsonArray>();
    int numNetworks = WiFi.scanComplete();
    for (int i = 0; i < numNetworks; i++) {
        JsonObject network = networks.add<JsonObject>();
        network["rssi"] = WiFi.RSSI(i);
        network["ssid"] = WiFi.SSID(i);
        network["bssid"] = WiFi.BSSIDstr(i);
        network["channel"] = WiFi.channel(i);
        network["encryption_type"] = (uint8_t)WiFi.encryptionType(i);
    }
}

void networkStatus(JsonObject &root) {
    wl_status_t status = WiFi.status();
    root["status"] = (uint8_t)status;
    if (status == WL_CONNECTED) {
        root["local_ip"] = WiFi.localIP().toString();
        root["mac_address"] = WiFi.macAddress();
        root["rssi"] = WiFi.RSSI();
        root["ssid"] = WiFi.SSID();
        root["bssid"] = WiFi.BSSIDstr();
        root["channel"] = WiFi.channel();
        root["subnet_mask"] = WiFi.subnetMask().toString();
        root["gateway_ip"] = WiFi.gatewayIP().toString();
        IPAddress dnsIP1 = WiFi.dnsIP(0);
        IPAddress dnsIP2 = WiFi.dnsIP(1);
        if (dnsIP1 != INADDR_NONE) {
            root["dns_ip_1"] = dnsIP1.toString();
        }
        if (dnsIP2 != INADDR_NONE) {
            root["dns_ip_2"] = dnsIP2.toString();
        }
    }
}

esp_err_t handleScan(PsychicRequest *request) {
    if (WiFi.scanComplete() != -1) {
        WiFi.scanDelete();
        WiFi.scanNetworks(true);
    }
    return request->reply(202);
}

esp_err_t getNetworks(PsychicRequest *request) {
    int numNetworks = WiFi.scanComplete();
    if (numNetworks == -1)
        return request->reply(202);
    else if (numNetworks < -1)
        return handleScan(request);

    PsychicJsonResponse response = PsychicJsonResponse(request, false);
    JsonObject root = response.getRoot();
    networks(root);
    return response.send();
}

esp_err_t getNetworkStatus(PsychicRequest *request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, false);
    JsonObject root = response.getRoot();
    networkStatus(root);
    return response.send();
}

} // namespace wifi_sta
