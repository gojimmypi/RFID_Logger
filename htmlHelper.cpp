// 
// htmlHelper.cpp
//
// define HTTP_DEBUG in GlobalDefine to turn on Serial debug diagnostics
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


// ensure there are no unprintable characters (including space) in the url 
String HTML_CleanURL(String url) {
    String res = "";
    String thisChar = "";
    for (size_t i = 0; i < url.length(); i++)
    {
        thisChar = url.substring(i, i + 1);
        if ((thisChar > " ") && (thisChar <= "~") ) {
            res = res + thisChar;
        }
    }
    return res;
}

// generate a request block from the given url
String HTML_RequestText(String url) {
    if (url == "") {
        HTTP_DEBUG_PRINTLN("HTML_RequestText empty URL!");
    }
    url = HTML_CleanURL(url);
    return String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + SECRET_APP_HOST + "\r\n" +
        "Accept-Language: en-US\r\n" +
        "User-Agent: ESP32_RFID\r\n" +
        "Connection: keep-alive\r\n\r\n"; // we may need to follow a 302; see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Connection
}

// send html request TheRequest to thisClient, return 302 move url in MovedToURL as needed
void HTML_SendRequest(WiFiClientSecure *thisClient, String TheRequest, String& MovedToURL) {
    HTTP_DEBUG_PRINTLN("HTML_SendRequest");
    HTTP_DEBUG_PRINTLN("");
    HTTP_DEBUG_PRINTLN(TheRequest);
    HTTP_DEBUG_PRINTLN("");
    MovedToURL = "";
    (*thisClient).flush();
    yield();
    (*thisClient).print(TheRequest);
    HTTP_DEBUG_PRINTLN("Request sent");
    while ((*thisClient).connected()) {
        String line = (*thisClient).readStringUntil('\n');
        if (line.startsWith("Location:")) {
            MovedToURL = line.substring(10); // note that this will include improper trailing \n\r chars, cleaned later
        }
        HTTP_DEBUG_PRINTLN(line);
        if (line == "\r") {
            HTTP_DEBUG_PRINTLN("headers received");
            break;
        }
    }
    String line;

    HTTP_DEBUG_PRINTLN("reply:");
    HTTP_DEBUG_PRINTLN("==========");
    HTTP_DEBUG_PRINTLN(line);

    while ((*thisClient).available()) {
        line = (*thisClient).readStringUntil('\n');
        HTTP_DEBUG_PRINTLN(line);
        yield();
    }

    HTTP_DEBUG_PRINTLN("==========");
    // (*thisClient).flush();
    yield();
    HTTP_DEBUG_PRINTLN("");
    HTTP_DEBUG_PRINTLN("End HTML_SendRequest");
    HTTP_DEBUG_PRINTLN("");
}

// the non-public HTML_SendRequestFollowMove that keeps track of MoveDepth
void HTML_SendRequestFollowMove(WiFiClientSecure* thisClient, String TheRequest, String& MovedToURL, int MoveDepth ) {
    HTTP_DEBUG_PRINTLN("HTML_SendRequestFollowMove=");

    HTML_SendRequest(thisClient, TheRequest, MovedToURL);

    HTTP_DEBUG_PRINT("thisMovedRequestURL=");
    HTTP_DEBUG_PRINTLN(MovedToURL);
    if (MovedToURL == "") {
        HTTP_DEBUG_PRINTLN("No move!");
    }
    else {
        HTTP_DEBUG_PRINT("Doing move! Depth = ");
        HTTP_DEBUG_PRINTLN(MoveDepth);

        //url = "/RFID/default?UID=00000000"; // thisMovedRequestURL; // typically the default.aspx with the ".aspx" removed (weird) 
        String newurl = MovedToURL; // our new url is the one that returned as a new address
        MovedToURL = ""; // we'll reset the moved address as a parameter
        String thisMovedRequest = HTML_RequestText(newurl); // generate a new regest from this newurl

        // we'll only recursively follow up to 10 moves. (typically there's only one, such as default.aspx moved to default    
        if (MoveDepth < 10) {
            HTML_SendRequestFollowMove(thisClient, thisMovedRequest, MovedToURL, MoveDepth + 1);
        }
    }

}

// Send HTML TheRequest to thisClient, following 302 moves up to 10 deep; we'll start at 0
void HTML_SendRequestFollowMove(WiFiClientSecure* thisClient, String TheRequest, String& MovedToURL) {
    HTML_SendRequestFollowMove(thisClient, TheRequest, MovedToURL, 0);
}

