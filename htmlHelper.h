// htmlHelper.h

#ifndef _HTMLHELPER_h
#define _HTMLHELPER_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "arduino.h"
#else
    #include "WProgram.h"
#endif

#undef FOUND_BOARD
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#define FOUND_BOARD ESP8266
#pragma message(Reminder "Target hardware: ESP8266")
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <HTTPClient.h>
#include <WiFi.h>
#define FOUND_BOARD ESP32
#pragma message(Reminder "Target hardware: ESP32")
#endif

#ifdef ARDUINO_SAMD_MKRWIFI1010
#include <WiFiNINA.h>
#define FOUND_BOARD ARDUINO_SAMD_MKRWIFI1010
#endif

#ifndef FOUND_BOARD
#pragma message(Reminder "Error Target hardware not defined !")
#endif // ! FOUND_BOARD

String HTML_RequestText(String url);
int HTML_SendRequest(WIFI_CLIENT_CLASS* thisClient, String TheRequest, String& MovedToURL);
int HTML_SendRequestFollowMove(WIFI_CLIENT_CLASS* thisClient, String TheRequest, String& MovedToURL); // will follow 302 moves up to 10 times

#endif // _HTMLHELPER_h
