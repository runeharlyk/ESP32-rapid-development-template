#pragma once

#include <memory>
#include <vector>
#include <tuple>
#include <types.h>
#include <jsonserializer.h>

#define DEVICE_CONFIG_FILE "/device.cfg"
#define DEFAULT_NTP_SERVER "0.pool.ntp.org"

class DeviceConfig : public IJSONSerializable
{
    // Add variables for additional settings to this list
    String location;
    bool locationIsZip;
    String countryCode;
    String timeZone;
    String openWeatherApiKey;
    bool use24HourClock;
    bool useCelsius;
    String ntpServer;

    std::vector<SettingSpec> settingSpecs;
    size_t writerIndex;
/*
    void WriteToNVS(const String& name, const String& value);
    void WriteToNVS(const String& name, bool value);
*/
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
    static constexpr const char * LocationTag = NAME_OF(location);
    static constexpr const char * LocationIsZipTag = NAME_OF(locationIsZip);
    static constexpr const char * CountryCodeTag = NAME_OF(countryCode);
    static constexpr const char * OpenWeatherApiKeyTag = NAME_OF(openWeatherApiKey);
    static constexpr const char * TimeZoneTag = NAME_OF(timeZone);
    static constexpr const char * Use24HourClockTag = NAME_OF(use24HourClock);
    static constexpr const char * UseCelsiusTag = NAME_OF(useCelsius);
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
        jsonDoc[LocationTag] = location;
        jsonDoc[LocationIsZipTag] = locationIsZip;
        jsonDoc[CountryCodeTag] = countryCode;
        jsonDoc[TimeZoneTag] = timeZone;
        jsonDoc[Use24HourClockTag] = use24HourClock;
        jsonDoc[UseCelsiusTag] = useCelsius;
        jsonDoc[NTPServerTag] = ntpServer;

        if (includeSensitive)
            jsonDoc[OpenWeatherApiKeyTag] = openWeatherApiKey;

        return jsonObject.set(jsonDoc.as<JsonObjectConst>());
    }

    virtual bool DeserializeFromJSON(const JsonObjectConst& jsonObject) override
    {
        return DeserializeFromJSON(jsonObject, false);
    }

    bool DeserializeFromJSON(const JsonObjectConst& jsonObject, bool skipWrite)
    {
        // Add deserialization logic for additional settings to this code
        SetIfPresentIn(jsonObject, location, LocationTag);
        SetIfPresentIn(jsonObject, locationIsZip, LocationIsZipTag);
        SetIfPresentIn(jsonObject, countryCode, CountryCodeTag);
        SetIfPresentIn(jsonObject, openWeatherApiKey, OpenWeatherApiKeyTag);
        SetIfPresentIn(jsonObject, use24HourClock, Use24HourClockTag);
        SetIfPresentIn(jsonObject, useCelsius, UseCelsiusTag);
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

    const String &GetTimeZone() const
    {
        return timeZone;
    }

    bool Use24HourClock() const
    {
        return use24HourClock;
    }

    void Set24HourClock(bool new24HourClock)
    {
        SetAndSave(use24HourClock, new24HourClock);
    }

    const String &GetLocation() const
    {
        return location;
    }

    void SetLocation(const String &newLocation)
    {
        SetAndSave(location, newLocation);
    }

    const String &GetCountryCode() const
    {
        return countryCode;
    }

    void SetCountryCode(const String &newCountryCode)
    {
        SetAndSave(countryCode, newCountryCode);
    }

    bool IsLocationZip() const
    {
        return locationIsZip;
    }

    void SetLocationIsZip(bool newLocationIsZip)
    {
        SetAndSave(locationIsZip, newLocationIsZip);
    }

    const String &GetOpenWeatherAPIKey() const
    {
        return openWeatherApiKey;
    }

    ValidateResponse ValidateOpenWeatherAPIKey(const String &newOpenWeatherAPIKey);

    void SetOpenWeatherAPIKey(const String &newOpenWeatherAPIKey)
    {
        SetAndSave(openWeatherApiKey, newOpenWeatherAPIKey);
    }

    bool UseCelsius() const
    {
        return useCelsius;
    }

    void SetUseCelsius(bool newUseCelsius)
    {
        SetAndSave(useCelsius, newUseCelsius);
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