#include "globals.h"
#include "webserver.h"

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