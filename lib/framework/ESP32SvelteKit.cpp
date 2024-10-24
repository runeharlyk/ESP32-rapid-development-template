#include <ESP32SvelteKit.h>

static const char *TAG = "ESP32SvelteKit";

ESP32SvelteKit::ESP32SvelteKit(PsychicHttpServer *server) : _server(server) {}

void ESP32SvelteKit::begin() {
    ESP_LOGV("ESP32SvelteKit", "Loading settings from files system");
    ESP_LOGI(TAG, "Running Firmware Version: %s\n", APP_VERSION);

    _wifiSettingsService.initWiFi();

    setupMDNS();

    startServices();

    setupServer();
}

void ESP32SvelteKit::setupServer() {
    _server->config.max_uri_handlers = _numberEndpoints;
    _server->maxUploadSize = _maxFileUpload;
    _server->listen(_port);

    // Serve static resources from PROGMEM
    ESP_LOGV(TAG, "Registering routes from PROGMEM static resources");
    WWWData::registerRoutes([&](const String &uri, const String &contentType, const uint8_t *content, size_t len) {
        PsychicHttpRequestCallback requestHandler = [contentType, content, len](PsychicRequest *request) {
            PsychicResponse response(request);
            response.setCode(200);
            response.setContentType(contentType.c_str());
            response.addHeader("Content-Encoding", "gzip");
            response.addHeader("Cache-Control", "public, immutable, max-age=31536000");
            response.setContent(content, len);
            return response.send();
        };
        PsychicWebHandler *handler = new PsychicWebHandler();
        handler->onRequest(requestHandler);
        _server->on(uri.c_str(), HTTP_GET, handler);

        // Set default end-point for all non matching requests
        // this is easier than using webServer.onNotFound()
        if (uri.equals("/index.html")) {
            _server->defaultEndpoint->setHandler(handler);
        }
    });

    // SYSTEM
    _server->on("/api/v1/system/reset", HTTP_POST, system_service::handleReset);
    _server->on("/api/v1/system/restart", HTTP_POST, system_service::handleRestart);
    _server->on("/api/v1/system/sleep", HTTP_POST, system_service::handleSleep);
    _server->on("/api/v1/system/status", HTTP_GET, system_service::getStatus);
    _server->on("/api/v1/system/metrics", HTTP_GET, system_service::getMetrics);

    // WIFI
    _server->on("/api/v1/wifi/scan", HTTP_POST, wifi_sta::handleScan);
    _server->on("/api/v1/wifi/networks", HTTP_GET, wifi_sta::getNetworks);
    _server->on("/api/v1/wifi/sta/status", HTTP_GET, wifi_sta::getNetworkStatus);
    _server->on("/api/v1/wifi/sta/settings", HTTP_GET,
                [this](PsychicRequest *request) { return _wifiSettingsService.endpoint.getState(request); });
    _server->on("/api/v1/wifi/sta/settings", HTTP_POST, [this](PsychicRequest *request, JsonVariant &json) {
        return _wifiSettingsService.endpoint.handleStateUpdate(request, json);
    });

    // AP
    _server->on("/api/v1/wifi/ap/status", HTTP_GET,
                [this](PsychicRequest *r) { return _apSettingsService.getStatus(r); });
    _server->on("/api/v1/wifi/ap/settings", HTTP_GET,
                [this](PsychicRequest *request) { return _apSettingsService.endpoint.getState(request); });
    _server->on("/api/v1/wifi/ap/settings", HTTP_POST, [this](PsychicRequest *request, JsonVariant &json) {
        return _apSettingsService.endpoint.handleStateUpdate(request, json);
    });

// NTP
#if FT_ENABLED(USE_NTP)
    _server->on("/api/v1/ntp/status", HTTP_GET, [this](PsychicRequest *r) { return _ntpSettingsService.getStatus(r); });
    _server->on("/api/v1/ntp/time", HTTP_POST,
                [this](PsychicRequest *r, JsonVariant &json) { return _ntpSettingsService.handleTime(r, json); });
    _server->on("/api/v1/ntp/settings", HTTP_GET,
                [this](PsychicRequest *request) { return _ntpSettingsService.endpoint.getState(request); });
    _server->on("/api/v1/ntp/settings", HTTP_POST, [this](PsychicRequest *request, JsonVariant &json) {
        return _ntpSettingsService.endpoint.handleStateUpdate(request, json);
    });
#endif

    // FILESYSTEM
    _server->on("/api/files", HTTP_GET, FileSystem::getFiles);
    _server->on("/api/files/delete", HTTP_POST, FileSystem::handleDelete);
    _server->on("/api/files/upload/*", HTTP_POST, FileSystem::uploadHandler);
    _server->on("/api/files/edit", HTTP_POST, FileSystem::handleEdit);

    // MISC
    _server->on("/api/v1/features", HTTP_GET, feature_service::getFeatures);
    _server->on("/api/v1/ws/events", socket.getHandler());
    _server->on("/api/v1/firmware", HTTP_POST, _uploadFirmwareService.getHandler());

    // FIRMWARE
#if FT_ENABLED(USE_DOWNLOAD_FIRMWARE)
    _server->on("/api/v1/firmware/download", HTTP_POST, [this](PsychicRequest *r, JsonVariant &json) {
        return _downloadFirmwareService.handleDownloadUpdate(r, json);
    });
#endif

    // MQTT
#if FT_ENABLED(USE_MQTT)
    _server->on("/api/v1/mqtt/status", HTTP_GET,
                [this](PsychicRequest *r) { return _mqttSettingsService.getStatus(r); });
    _server->on("/api/v1/mqtt/settings", HTTP_GET,
                [this](PsychicRequest *r) { return _mqttSettingsService.endpoint.getState(r); });
    _server->on("/api/v1/mqtt/settings", HTTP_POST, [this](PsychicRequest *r, JsonVariant &json) {
        return _mqttSettingsService.endpoint.handleStateUpdate(r, json);
    });
#endif


    // STATIC CONFIG
#if SERVE_CONFIG_FILES
    _server->serveStatic("/api/config/", ESPFS, "/config/");
#endif

#if defined(ENABLE_CORS)
    ESP_LOGV(TAG, "Enabling CORS headers");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", CORS_ORIGIN);
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, Authorization");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials", "true");
#endif
    DefaultHeaders::Instance().addHeader("Server", APP_NAME);
}

void ESP32SvelteKit::setupMDNS() {
    ESP_LOGV("ESP32SvelteKit", "Starting MDNS");
    MDNS.begin(_wifiSettingsService.getHostname().c_str());
    MDNS.setInstanceName(_appName);
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "Firmware Version", APP_VERSION);
}

void ESP32SvelteKit::startServices() {
    _apSettingsService.begin();
    _wifiSettingsService.begin();

#if FT_ENABLED(USE_UPLOAD_FIRMWARE)
    _uploadFirmwareService.begin();
#endif
#if FT_ENABLED(USE_NTP)
    _ntpSettingsService.begin();
#endif
#if FT_ENABLED(USE_MQTT)
    _mqttSettingsService.begin();
#endif

    xTaskCreatePinnedToCore(this->_loopImpl, "ESP32 SvelteKit Loop", 4096, this, (tskIDLE_PRIORITY + 1), NULL, 0);
}

void ESP32SvelteKit::_loop() {
    while (1) {
        _wifiSettingsService.loop(); // 30 seconds
        _apSettingsService.loop();   // 10 seconds
#if FT_ENABLED(USE_MQTT)
        _mqttSettingsService.loop(); // 5 seconds
#endif
#if FT_ENABLED(USE_ANALYTICS)
        _analyticsService.loop();
#endif
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
