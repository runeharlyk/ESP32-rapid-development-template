#include "globals.h"
#include "webserver.h"

// Maps settings for which a validator is available to the invocation thereof
const std::map<String, CWebServer::ValueValidator> CWebServer::settingValidators
{};

std::vector<SettingSpec> CWebServer::deviceSettingSpecs{};

// Member function template specialzations

// Push param that represents a bool. Values considered true are text "true" and any whole number not equal to 0
template<>
bool CWebServer::PushPostParamIfPresent<bool>(AsyncWebServerRequest * pRequest, const String &paramName, ValueSetter<bool> setter)
{
    return PushPostParamIfPresent<bool>(pRequest, paramName, setter, [](AsyncWebParameter * param) constexpr
    {
        const String& value = param->value();
        return value == "true" || strtol(value.c_str(), NULL, 10);
    });
}

// Push param that represents a size_t
template<>
bool CWebServer::PushPostParamIfPresent<size_t>(AsyncWebServerRequest * pRequest, const String &paramName, ValueSetter<size_t> setter)
{
    return PushPostParamIfPresent<size_t>(pRequest, paramName, setter, [](AsyncWebParameter * param) constexpr
    {
        return strtoul(param->value().c_str(), NULL, 10);
    });
}

// Add CORS header to and send JSON response
template<>
void CWebServer::AddCORSHeaderAndSendResponse<AsyncJsonResponse>(AsyncWebServerRequest * pRequest, AsyncJsonResponse * pResponse)
{
    pResponse->setLength();
    AddCORSHeaderAndSendResponse<AsyncWebServerResponse>(pRequest, pResponse);
}

// Member function implementations

bool CWebServer::IsPostParamTrue(AsyncWebServerRequest * pRequest, const String & paramName)
{
    bool returnValue = false;

    PushPostParamIfPresent<bool>(pRequest, paramName, [&returnValue](auto value) { returnValue = value; return true; });

    return returnValue;
}

void CWebServer::GetStatistics(AsyncWebServerRequest * pRequest)
{
    log_v("GetStatistics");

    auto response = new AsyncJsonResponse(false, JSON_BUFFER_BASE_SIZE);
    auto j = response->getRoot();

    j["HEAP_SIZE"]             = _staticStats.HeapSize;
    j["HEAP_FREE"]             = ESP.getFreeHeap();
    j["HEAP_MIN"]              = ESP.getMinFreeHeap();

    j["DMA_SIZE"]              = _staticStats.DmaHeapSize;
    j["DMA_FREE"]              = heap_caps_get_free_size(MALLOC_CAP_DMA);
    j["DMA_MIN"]               = heap_caps_get_largest_free_block(MALLOC_CAP_DMA);

    j["PSRAM_SIZE"]            = _staticStats.PsramSize;
    j["PSRAM_FREE"]            = ESP.getFreePsram();
    j["PSRAM_MIN"]             = ESP.getMinFreePsram();

    j["CHIP_MODEL"]            = _staticStats.ChipModel;
    j["CHIP_CORES"]            = _staticStats.ChipCores;
    j["CHIP_SPEED"]            = _staticStats.CpuFreqMHz;
    j["PROG_SIZE"]             = _staticStats.SketchSize;

    j["CODE_SIZE"]             = _staticStats.SketchSize;
    j["CODE_FREE"]             = _staticStats.FreeSketchSpace;
    j["FLASH_SIZE"]            = _staticStats.FlashChipSize;

    j["CPU_USED"]              = g_TaskManager.GetCPUUsagePercent();
    j["CPU_USED_CORE0"]        = g_TaskManager.GetCPUUsagePercent(0);
    j["CPU_USED_CORE1"]        = g_TaskManager.GetCPUUsagePercent(1);

    AddCORSHeaderAndSendResponse(pRequest, response);
}

void CWebServer::SendSettingSpecsResponse(AsyncWebServerRequest * pRequest, const std::vector<SettingSpec> & settingSpecs)
{
    static size_t jsonBufferSize = JSON_BUFFER_BASE_SIZE;
    bool bufferOverflow;

    do
    {
        bufferOverflow = false;
        auto response = std::make_unique<AsyncJsonResponse>(false, jsonBufferSize);
        auto jsonArray = response->getRoot().to<JsonArray>();

        for (auto& spec : settingSpecs)
        {
            auto specObject = jsonArray.createNestedObject();

            StaticJsonDocument<384> jsonDoc;

            jsonDoc["name"] = spec.Name;
            jsonDoc["friendlyName"] = spec.FriendlyName;
            jsonDoc["description"] = spec.Description;
            jsonDoc["type"] = to_value(spec.Type);
            jsonDoc["typeName"] = spec.ToName(spec.Type);

            if (!specObject.set(jsonDoc.as<JsonObjectConst>()))
            {
                bufferOverflow = true;
                jsonBufferSize += JSON_BUFFER_INCREMENT;
                log_v("JSON reponse buffer overflow! Increased buffer to %zu bytes", jsonBufferSize);
                break;
            }
        }

        if (!bufferOverflow)
            AddCORSHeaderAndSendResponse(pRequest, response.release());

    } while (bufferOverflow);
}

const std::vector<SettingSpec> & CWebServer::LoadDeviceSettingSpecs()
{
    if (deviceSettingSpecs.size() == 0)
    {
        auto deviceConfigSpecs = g_ptrDeviceConfig->GetSettingSpecs();
        deviceSettingSpecs.insert(deviceSettingSpecs.end(), deviceConfigSpecs.begin(), deviceConfigSpecs.end());
    }

    return deviceSettingSpecs;
}

void CWebServer::GetSettingSpecs(AsyncWebServerRequest * pRequest)
{
    SendSettingSpecsResponse(pRequest, LoadDeviceSettingSpecs());
}

// Responds with current config, excluding any sensitive values
void CWebServer::GetSettings(AsyncWebServerRequest * pRequest)
{
    log_v("GetSettings");

    auto response = new AsyncJsonResponse(false, JSON_BUFFER_BASE_SIZE);
    response->addHeader("Server","NightDriverStrip");
    auto root = response->getRoot();
    JsonObject jsonObject = root.to<JsonObject>();

    // We get the serialized JSON for the device config, without any sensitive values
    g_ptrDeviceConfig->SerializeToJSON(jsonObject, false);
   
    AddCORSHeaderAndSendResponse(pRequest, response);
}

// Support function that silently sets whatever settings are included in the request passed.
//   Composing a response is left to the invoker!
void CWebServer::SetSettingsIfPresent(AsyncWebServerRequest * pRequest)
{
    PushPostParamIfPresent<String>(pRequest, DeviceConfig::NTPServerTag, SET_VALUE(g_ptrDeviceConfig->SetNTPServer(value)));
}

// Set settings and return resulting config
void CWebServer::SetSettings(AsyncWebServerRequest * pRequest)
{
    log_v("SetSettings");

    SetSettingsIfPresent(pRequest);

    // We return the current config in response
    GetSettings(pRequest);
}

// Save the posted file to SPIFFS
void CWebServer::SaveFile(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final)
{
    log_v("SaveFile");
}

// Update the current firmware
void CWebServer::UpdateFileSystemImage(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    log_v("UpdateFileSystemImage");

    HandleUpdate(request, filename, index, data, len, final, (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000, U_FLASH);
}

// Update the current firmware
void CWebServer::UpdateFirmware(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    log_v("UpdateFirmware");

    HandleUpdate(request, filename, index, data, len, final, UPDATE_SIZE_UNKNOWN, U_SPIFFS);
}

// Handles updating the filesysten image or firmware 
void CWebServer::HandleUpdate(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final, size_t upload_size, uint8_t upload_index) {
    if (!index) {
        log_i("Update Start: %s\n", filename.c_str());
        log_i("Uploading: %d", upload_index);
        if (!Update.begin(upload_size, upload_index)) {
            Update.printError(Serial);
        }
    }
    if (!Update.hasError()) {
        if (Update.write(data, len) != len) {
            Update.printError(Serial);
        }
    }
    if (final) {
        if (Update.end(true)) {
            log_i("Update Success: %uB\n", index + len);
        }
        else {
            Update.printError(Serial);
        }
    }
}

// Ensures the request gets send to the client
void CWebServer::UpdateRequestHandler(AsyncWebServerRequest* request){
    bool success = !Update.hasError();
    AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", success ? "OK" : "FAIL");
    response->addHeader("Connection", "close");
    request->send(response);

    if (success) {
        delay(250);
        ESP.restart();
    }
}

// Reset effect config, device config and/or the board itself
void CWebServer::Reset(AsyncWebServerRequest * pRequest)
{
    if (IsPostParamTrue(pRequest, "deviceConfig"))
    {
        log_i("Removing DeviceConfig");
        g_ptrDeviceConfig->RemovePersisted();
    }

    bool boardResetRequested = IsPostParamTrue(pRequest, "board");

    AddCORSHeaderAndSendOKResponse(pRequest);

    if (boardResetRequested)
    {
        log_w("Resetting device at API request!");
        delay(1000);    // Give the response a second to be sent
        throw new std::runtime_error("Resetting device at API request");
    }
}
