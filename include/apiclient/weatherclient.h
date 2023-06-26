#include <apiclient/apiclient.h>

#include <HTTPClient.h>

class WeatherApiClient : public ApiClientBase {
public:
    WeatherApiClient(Client &client) : ApiClientBase(client){ 
        log_i("Calling weather contructer");
        server = (char*)"api.openweathermap.org";
        url = (char*)"/data/2.5/forecast?id=524901&appid=APIKEY";
        port = 80;
        timeout = 10000;
        capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
    }

    void getWeatherData() {
        log_i("getting weather data");

        String serverPath = "http://api.openweathermap.org/data/2.5/forecast?id=524901&appid=APIKEY";

        String jsonBuffer = httpGETRequest(serverPath.c_str());

        StaticJsonDocument<200> filter;
        filter["list"][0]["dt"] = true;
        filter["list"][0]["main"]["temp"] = true;

        // Deserialize the document
        StaticJsonDocument<400> doc;
        deserializeJson(doc, jsonBuffer, DeserializationOption::Filter(filter));

        serializeJsonPretty(doc, Serial);
    }
};

extern DRAM_ATTR std::unique_ptr<WeatherApiClient> g_ptrWeatherApiClient;