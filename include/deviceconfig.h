#pragma once

#include <memory>
#include <types.h>
#include <jsonserializer.h>

#define DEVICE_CONFIG_FILE "/device.cfg"
#define DEFAULT_NTP_SERVER "0.pool.ntp.org"

class DeviceConfig : public IJSONSerializable
{
    // Add variables for additional settings to this list
    String ntpServer;

    std::vector<SettingSpec> settingSpecs;
    size_t writerIndex;

    void SaveToJSON();

    template <typename T>
    void SetAndSave(T& target, const T& source)
    {
        if (target == source)
            return;

        target = source;

        SaveToJSON();
    }

    template <typename T>
    void SetIfPresentIn(const JsonObjectConst& jsonObject, T& target, const char *tag)
    {
        if (jsonObject.containsKey(tag))
            target = jsonObject[tag].as<T>();
    }

  public:

    using ValidateResponse = std::pair<bool, String>;

    // Add additional setting Tags to this list
    static constexpr const char * NTPServerTag = NAME_OF(ntpServer);

    DeviceConfig();

    virtual bool SerializeToJSON(JsonObject& jsonObject) override
    {
        return SerializeToJSON(jsonObject, true);
    }

    bool SerializeToJSON(JsonObject& jsonObject, bool includeSensitive)
    {
        AllocatedJsonDocument jsonDoc(1024);

        // Add serialization logic for additionl settings to this code
        jsonDoc[NTPServerTag] = ntpServer;

        return jsonObject.set(jsonDoc.as<JsonObjectConst>());
    }

    virtual bool DeserializeFromJSON(const JsonObjectConst& jsonObject) override
    {
        return DeserializeFromJSON(jsonObject, false);
    }

    bool DeserializeFromJSON(const JsonObjectConst& jsonObject, bool skipWrite)
    {
        // Add deserialization logic for additional settings to this code
        SetIfPresentIn(jsonObject, ntpServer, NTPServerTag);

        if (ntpServer.isEmpty())
            ntpServer = DEFAULT_NTP_SERVER;

        if (!skipWrite)
            SaveToJSON();

        return true;
    }

    void RemovePersisted()
    {
        RemoveJSONFile(DEVICE_CONFIG_FILE);
    }

    virtual const std::vector<SettingSpec>& GetSettingSpecs() const
    {
        return settingSpecs;
    }

    const String &GetNTPServer() const
    {
        return ntpServer;
    }

    void SetNTPServer(const String &newNTPServer)
    {
        SetAndSave(ntpServer, newNTPServer);
    }

};


extern DRAM_ATTR std::unique_ptr<DeviceConfig> g_ptrDeviceConfig;