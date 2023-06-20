#pragma once

#include <globals.h>
#include <deviceconfig.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

#define HTTP_CODE_OK 200
#define HTTP_CODE_NOT_FOUND 404

class CWebServer {
  private:
    struct StaticStatistics {
        uint32_t HeapSize;
        size_t DmaHeapSize;
        uint32_t PsramSize;
        const char *ChipModel;
        uint8_t ChipCores;
        uint32_t CpuFreqMHz;
        uint32_t SketchSize;
        uint32_t FreeSketchSpace;
        uint32_t FlashChipSize;
    };

    AsyncWebServer _server;
    StaticStatistics _staticStats;

    // AddCORSHeaderAndSend(OK)Response
    //
    // Sends a response with CORS headers added
    template<typename Tr>
    static void AddCORSHeaderAndSendResponse(AsyncWebServerRequest * pRequest, Tr * pResponse) {
        pResponse->addHeader("Server","NightDriverStrip");
        pResponse->addHeader("Access-Control-Allow-Origin", "*");
        pRequest->send(pResponse);
    }

    // Version for empty response, normally used to finish up things that don't return anything, like "NextEffect"
    static void AddCORSHeaderAndSendOKResponse(AsyncWebServerRequest * pRequest) {
        AddCORSHeaderAndSendResponse(pRequest, pRequest->beginResponse(HTTP_CODE_OK));
    }

    // Endpoint member functions
    //static void GetEffectListText(AsyncWebServerRequest * pRequest);

    // Not static because it uses member _staticStats
    void GetStatistics(AsyncWebServerRequest * pRequest);

  public:

    CWebServer()
        : _server(HTTP_PORT)
    {}

    void begin() {
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

        _staticStats.HeapSize = ESP.getHeapSize();
        _staticStats.DmaHeapSize = heap_caps_get_total_size(MALLOC_CAP_DMA);
        _staticStats.PsramSize = ESP.getPsramSize();
        _staticStats.ChipModel = ESP.getChipModel();
        _staticStats.ChipCores = ESP.getChipCores();
        _staticStats.CpuFreqMHz = ESP.getCpuFreqMHz();
        _staticStats.SketchSize = ESP.getSketchSize();
        _staticStats.FreeSketchSpace = ESP.getFreeSketchSpace();
        _staticStats.FlashChipSize = ESP.getFlashChipSize();

        //_server.on("/effects",               HTTP_GET,  [](AsyncWebServerRequest * pRequest)        { GetEffectListText(pRequest); });

        //_server.on("/reset",                 HTTP_POST, [](AsyncWebServerRequest * pRequest)        { Reset(pRequest); });

        _server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

        _server.onNotFound([](AsyncWebServerRequest *request) {
            if (request->method() == HTTP_OPTIONS) {
                request->send(HTTP_CODE_OK);                                     // Apparently needed for CORS: https://github.com/me-no-dev/ESPAsyncWebServer
            } else {
                log_w("Failed GET for %s\n", request->url().c_str() );
                request->send(HTTP_CODE_NOT_FOUND);
            }
        });

        _server.begin();

        log_i("HTTP server started");
    }
};