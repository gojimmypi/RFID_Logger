// 
// htmlHelper.cpp
// 

#include "GlobalDefine.h"
#include "htmlHelper.h"


#undef FOUND_BOARD
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#define FOUND_BOARD ESP8266
#pragma message(Reminder "Target hardware is ESP8266 for htmlHelper")
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <HTTPClient.h>
#include <WiFi.h>
#define FOUND_BOARD ESP32
#pragma message(Reminder "Target hardware is ESP32 for htmlHelper")
#endif

#ifndef FOUND_BOARD
#pragma message(Reminder "Error Target hardware not defined !")
#endif // ! FOUND_BOARD

String HTML_RequestText(String url) {
    return String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + SECRET_APP_HOST + "\r\n" +
        "Accept-Language: en-US\r\n" +
        "User-Agent: HA_RFID\r\n" +
        "Connection: Keep-Alive\r\n\r\n";
}

void HTML_SendRequest(WiFiClientSecure *thisClient, String TheRequest, String& MovedToURL) {
    Serial.println("HTML_SendRequest");
    MovedToURL = "";
    (*thisClient).print(TheRequest);
    Serial.println("moved request sent");
    while ((*thisClient).connected()) {
        String line = (*thisClient).readStringUntil('\n');
        if (line.startsWith("Location:")) {
            MovedToURL = line.substring(9);
        }
        Serial.println(line);
        if (line == "\r") {
            Serial.println("headers received");
            break;
        }
    }
    String line = (*thisClient).readStringUntil('\n');
    Serial.println(line);

    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(line);
    Serial.println("==========");

}
