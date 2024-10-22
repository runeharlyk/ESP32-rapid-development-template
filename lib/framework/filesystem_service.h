#pragma once

#include <PsychicHttp.h>
#include <ESPFS.h>

namespace FileSystem {
extern PsychicUploadHandler *uploadHandler;

String listFiles(const String &directory, bool isRoot = true);
bool deleteFile(const char *filename);
bool editFile(const char *filename, const char *content);
esp_err_t uploadFile(PsychicRequest *request, const String &filename, uint64_t index, uint8_t *data, size_t len,
                     bool last);

esp_err_t getFiles(PsychicRequest *request);
esp_err_t handleDelete(PsychicRequest *request, JsonVariant &json);
esp_err_t handleEdit(PsychicRequest *request, JsonVariant &json);
} // namespace FileSystem