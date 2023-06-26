#pragma once

#include <Client.h>
#include <ArduinoJson.h>

#define SPOTIFY_HOST "api.spotify.com"

class ApiClientBase {
protected:
    Client* client;
    char* server;
    char* url;
    int port;
    int timeout;
    size_t capacity {JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60};
    DynamicJsonDocument doc;

public:
    ApiClientBase(Client &Client)
    : doc(capacity)
    {
        log_i("Calling api base contructer");
        client = &Client;
    }

    String httpGETRequest(const char* serverName) {
        WiFiClient client;
        HTTPClient http;
            
        // Your Domain name with URL path or IP address with path
        http.begin(client, serverName);
        
        // Send HTTP POST request
        int httpResponseCode = http.GET();
        
        String payload = "{}"; 
        
        if (httpResponseCode>0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            payload = http.getString();
        }
        else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        // Free resources
        http.end();

        return payload;
    }

    // Generic Request Methods
    int makeGetRequest(const char *command, const char *authorization, const char *accept = "application/json", const char *host = SPOTIFY_HOST)
    {
        log_i("Making get request to server: %s:%u getting: %s, timeout:%u", server, port, url, timeout);
        client->flush();
        client->setTimeout(timeout);
        if (!client->connect(host, port)) 
        {
            log_w("Connection failed");
            return -1;
        }

        // give the esp a breather
        yield();

        // Send HTTP request
        client->print(F("GET "));
        client->print(command);
        client->println(F(" HTTP/1.0"));

        //Headers
        client->print(F("Host: "));
        client->println(host);

        if (accept != NULL)
        {
            client->print(F("Accept: "));
            client->println(accept);
        }

        if (authorization != NULL)
        {
            client->print(F("Authorization: "));
            client->println(authorization);
        }

        client->println(F("Cache-Control: no-cache"));

        if (client->println() == 0) 
        {
            log_w("Failed to send request");
            return -2;
        }

        int statusCode = getHttpStatusCode();

        return statusCode;
    }

    int makeRequestWithBody(const char *type, const char *command, const char *authorization, const char *body = "", const char *contentType = "application/json", const char *host = SPOTIFY_HOST)
    {
        client->flush();
        client->setTimeout(timeout);
        if (!client->connect(host, port)) 
        {
            log_w("Connection failed");
            return -1;
        }

        // give the esp a breather
        yield();

        // Send HTTP request
        client->print(type);
        client->print(command);
        client->println(F(" HTTP/1.0"));

        //Headers
        client->print(F("Host: "));
        client->println(host);

        client->println(F("Accept: application/json"));
        client->print(F("Content-Type: "));
        client->println(contentType);

        if (authorization != NULL) 
        {
            client->print(F("Authorization: "));
            client->println(authorization);
        }

        client->println(F("Cache-Control: no-cache"));

        client->print(F("Content-Length: "));
        client->println(strlen(body));

        client->println();

        client->print(body);

        if (client->println() == 0)
        {
            log_w("Failed to send request");
            return -2;
        }

        int statusCode = getHttpStatusCode();
        return statusCode;
    }

    int makePostRequest(const char *command, const char *authorization, const char *body = "", const char *contentType = "application/json", const char *host = SPOTIFY_HOST)
    {
        return makeRequestWithBody("PUT ", command, authorization, body, contentType);
    }

    int makePutRequest(const char *command, const char *authorization, const char *body = "", const char *contentType = "application/json", const char *host = SPOTIFY_HOST)
    {
        return makeRequestWithBody("POST ", command, authorization, body, contentType, host);
    }
protected:

    // Request helper functions
    int getContentLength()
    {
        if (client->find("Content-Length:"))
        {
            int contentLength = client->parseInt();
            log_i("Content-Length: %u", contentLength);
            return contentLength;
        }

        return -1;
    }

    int getHttpStatusCode()
    {
        char status[32] = {0};
        client->readBytesUntil('\r', status, sizeof(status));
        Serial.print(F("Status: "));
        Serial.println(status);

        char *token;
        token = strtok(status, " "); // https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm

        Serial.print(F("HTTP Version: "));
        Serial.println(token);

        if (token != NULL && (strcmp(token, "HTTP/1.0") == 0 || strcmp(token, "HTTP/1.1") == 0))
        {
            token = strtok(NULL, " ");
            if (token != NULL)
            {
                Serial.print(F("Status Code: "));
                Serial.println(token);
                return atoi(token);
            }
        }

        return -1;
    }

    void skipHeaders(bool tossUnexpectedForJSON = true)
    {
        if (!client->find("\r\n\r\n"))
        {
            log_i("Invalid response");
            return;
        }

        if (tossUnexpectedForJSON)
        {
            // Was getting stray characters between the headers and the body
            // This should toss them away
            while (client->available() && client->peek() != '{')
            {
                char c = 0;
                client->readBytes(&c, 1);
            }
        }
    }

    void closeClient()
    {
        if (client->connected())
        {
            log_i("Closing client");
            client->stop();
        }
    }
};
