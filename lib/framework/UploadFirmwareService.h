#ifndef UploadFirmwareService_h
#define UploadFirmwareService_h

#include <Arduino.h>

#include <Update.h>
#include <WiFi.h>

#include <PsychicHttp.h>
#include <EventSocket.h>
#include <system_service.h>

enum FileType { ft_none = 0, ft_firmware = 1, ft_md5 = 2 };

class UploadFirmwareService {
  public:
    UploadFirmwareService();

    void begin();

    PsychicUploadHandler *getHandler() { return &uploadHandler; }

  private:
    PsychicUploadHandler uploadHandler;

    esp_err_t handleUpload(PsychicRequest *request, const String &filename, uint64_t index, uint8_t *data, size_t len,
                           bool final);
    esp_err_t uploadComplete(PsychicRequest *request);
    esp_err_t handleError(PsychicRequest *request, int code);
    esp_err_t handleEarlyDisconnect();
};

#endif // end UploadFirmwareService_h
