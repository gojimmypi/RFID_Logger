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

#ifdef ARDUINO_SAMD_MKRWIFI1010
#include <WiFiNINA.h>
#define FOUND_BOARD ARDUINO_SAMD_MKRWIFI1010
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
        "User-Agent: RFID-Logger\r\n" +
        "Connection: keep-alive\r\n\r\n"; // we may need to follow a 302; see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Connection
}

// send html request TheRequest to thisClient, return 302 move url in MovedToURL as needed
// we do NOT follow the 302 here! See HTML_SendRequestFollowMove()
// return 0 for success
int HTML_SendRequest(WIFI_CLIENT_CLASS *thisClient, String TheRequest, String& MovedToURL) {
    int result = 1;
    String line = "";
    MovedToURL = ""; // if there's a 302 move response location, we'll put it here. If not, it will be sthis empty strong

    if (!(*thisClient).connected()) {
        HTTP_DEBUG_PRINTLN("Warning: HTML_SendRequest called with thisClient NOT CONNECTED!");
    }

    HTTP_DEBUG_PRINTLN("Starting HTML_SendRequest. TheRequest:");
    HTTP_DEBUG_PRINTLN("==========");
    HTTP_DEBUG_PRINTLN(TheRequest);
    HTTP_DEBUG_PRINTLN("==========");

    (*thisClient).flush();
    yield();
    (*thisClient).print(TheRequest);
    HTTP_DEBUG_PRINTLN("Request sent.");
    HTTP_DEBUG_PRINTLN("");
    HTTP_DEBUG_PRINTLN("Response:");
    HTTP_DEBUG_PRINTLN("");

    // we'll only read while WiFi is connected. 
    // If we lose connection while reading data, we'll give up
    while ((*thisClient).connected()) {
        String line = (*thisClient).readStringUntil('\n');
        if (line.startsWith("Location:")) {
            MovedToURL = line.substring(10); // note that this will include improper trailing \n\r chars, cleaned later
            // TODO - is there ever non-link text included? (maybe)
        }
        HTTP_DEBUG_PRINTLN(line);
        if (line == "\r") {
            HTTP_DEBUG_PRINTLN("Headers receive complete!");
            break;
        }
        yield();
    }

    HTTP_DEBUG_PRINTLN("reply:");
    HTTP_DEBUG_PRINTLN("==========");

    // we'll continue to read everything i nthe response... but if using the supplied web app, 
    // the only thing returned should be a number: the primary key of the record added in the SQL table
    int ResponseLineCount = 0;
    while ((*thisClient).available()) {
        ResponseLineCount++;
        line = (*thisClient).readStringUntil('\n');
        if (ResponseLineCount == 1) {
            if (line != "") {
                result = 0; // TODO check for numeric value in our custom page, for now a non-blank response is considered success
            }

            HTTP_DEBUG_PRINTLN(line);
        }
        else {
            HTTP_RESPONSE_DEBUG_PRINT(line);
        }
        yield();
    }

    HTTP_DEBUG_PRINTLN("==========");
    HTTP_DEBUG_PRINTLN("");
    HTTP_DEBUG_PRINTLN("End HTML_SendRequest");
    HTTP_DEBUG_PRINTLN("");
    return result;
}

// the non-public HTML_SendRequestFollowMove that keeps track of MoveDepth
int HTML_SendRequestFollowMove(WIFI_CLIENT_CLASS* thisClient, String TheRequest, String& MovedToURL, int MoveDepth ) {
    int result = 1;
    HTTP_DEBUG_PRINTLN("HTML_SendRequestFollowMove=");

    result = HTML_SendRequest(thisClient, TheRequest, MovedToURL);

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
            result = HTML_SendRequestFollowMove(thisClient, thisMovedRequest, MovedToURL, MoveDepth + 1);
        }
    }
    return result;
}

// Send HTML TheRequest to thisClient, following 302 moves up to 10 deep; we'll start at 0
int HTML_SendRequestFollowMove(WIFI_CLIENT_CLASS* thisClient, String TheRequest, String& MovedToURL) {
    return HTML_SendRequestFollowMove(thisClient, TheRequest, MovedToURL, 0);
}

