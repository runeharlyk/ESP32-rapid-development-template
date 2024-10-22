#ifndef ESPFS_H_
#define ESPFS_H_

#include <FS.h>
#include <LittleFS.h>

#define ESPFS LittleFS
#define FS_CONFIG_DIRECTORY "/config"

#define WIFI_SETTINGS_FILE "/config/wifiSettings.json"
#define NTP_SETTINGS_FILE "/config/ntpSettings.json"
#define STEPS_FILE "/config/steps.json"
#define MQTT_SETTINGS_FILE "/config/mqttSettings.json"
#define AP_SETTINGS_FILE "/config/apSettings.json"

#endif