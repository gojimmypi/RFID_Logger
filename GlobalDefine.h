// GlobalDefine.h

#ifndef _GLOBALDEFINE_h
#define _GLOBALDEFINE_h

// arduino.h allows some handy types, such as String
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

static const char* HEX_CHARS = "0123456789ABCDEF";


//*******************************************************************************************************************************************
//*************************************************************************************************************************************************************************************************
// Begin user config
//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
// Note .RFID_Logger.vsarduino.h is ignored at compile time! (always? we'll still wrap with #ifndef just in case)
//
//

//*************************************************************************************************************************************************************************************************
// Board Type
//*******************************************************************************************************************************************
// Choose ARDUINO_ARCH_ESP8266 or ARDUINO_ARCH_ESP32 or ARDUINO_SAMD_MKRWIFI1010 
// then ensure the Extensions - vMicro - Board is set to the matching hardware; ESP32 WRover Module (if using the ThingPulse)

//#ifndef ARDUINO_ARCH_ESP8266
//#define ARDUINO_ARCH_ESP8266
//#endif

#ifndef ARDUINO_ARCH_ESP32
#define ARDUINO_ARCH_ESP32
#endif

//#ifndef ARDUINO_SAMD_MKRWIFI1010
//#define ARDUINO_SAMD_MKRWIFI1010
//#endif


//*************************************************************************************************************************************************************************************************
// private secret settings
//*******************************************************************************************************************************************
// My config is stored in myPrivateSettings.h file 
// if you choose not to use such a file, set this to false:
#define USE_myPrivateSettings true

// Note the two possible file name string formats. 
#if USE_myPrivateSettings == true 

#include "/workspace-git/myPrivateSettings.h"

#else

#pragma message(Reminder "Settings needed !")
// create your own myPrivateSettings.h, or update the following lines in a SEPARATE file not included in your repo:
static const char* SECRET_WIFI_SSID = "my-wifi-SSID"
static const bool IS_EAP = true;											// set to true for enterprise access point authentication, otherwise false for regular (personal) wifi

// WPA2 Personal Settings 
static const char* SECRET_WIFI_PWD = "my-WiFi-PASSWORD";					// SSID password for regular Wi-Fi; user password for enterprise 

// WPA2 Enterprise Settings
static const char* SECRET_EAP_ID = "my-Enterprise-ID"						// EAP_ID (typically the same as EAP_USERNAME, e.g. domain\\my-Enterpeise-UserName) 
static const char* SECRET_EAP_USERNAME = "domain\\my-Enterprise-UserName";	// Username for authentification (typically the same as EAP_ID); consider trying also username@yourdomain.com
static const char* SECRET_EAP_PASSWORD = "my-Enterpeise-Password";			// Password for authentication

static const int APP_HTTPS_PORT = 443;										// the TLS/SSL port (typically 443)
static const char* SECRET_APP_HOST = "contoso.com";							// The primary host for this application
static const char* SECRET_APP_PATH = "/RFID/default.aspx";					// The application path and page name to send results

static const char CERTIFICATE_DETAILS_THUMBPRINT[] PROGMEM = "5F 3F 7A C2 56 9F 50 A4 66 76 47 C6 A1 8C A0 07 AA ED BB 8E"; // SHA1 fingerprint copy this from APP_HOST certificate thumbprint to use for fingerprint setting
#endif
//**************************************************************************************************************
//**************************************************************************************************************
// End user config
//**************************************************************************************************************
//**************************************************************************************************************

//**************************************************************************************************************
// debugging options
//**************************************************************************************************************
// #define SCREEN_DEBUG        // when defined, display low level screen debug info 
// #define SCREEN_DATA_DEBUG   // when defined, print screen data also to serial terminal
// #define JSON_DEBUG          // when defined, display JSON debug info 
#define WIFI_DEBUG          // when defined, display WiFi debug info 
#define SERIAL_SCREEN_DEBUG // when defined, display screen messages to serial port
#define HTTP_DEBUG          // when defined, display HTTP debug info 
// #define HTTP_HEADER_DEBUG   // when defined, print HTTP headers
// #define HTTP_RESPONSE_DEBUG // when defined, print HTTP response text (first line printed with HTTP_DEBUG) 
// #define TIMER_DEBUG         // when defined, display diagnostic timer info
#define HEAP_DEBUG          // when defined, display diagnostic heap info
#define HARDWARE_DEBUG
// #define SPIFFS_DEBUG
#define RFID_DEBUG // when defined, show RFID debug info
#define DEBUG_SEPARATOR F("***********************************")

//**************************************************************************************************************
// board-architecture-dependent WIFI_CLIENT_CLASS type selector 
//**************************************************************************************************************
#define USE_TLS_SSL // when defined, JSON data will use SSL
#define WIFI_CLIENT_CLASS WiFiClient  // the name of the WiFi class may vary depending on (1) architecture and (2) using TLS or not

#undef FOUND_BOARD
#ifdef ARDUINO_ARCH_ESP8266
#undef  WIFI_CLIENT_CLASS
#ifdef USE_TLS_SSL
#define WIFI_CLIENT_CLASS BearSSL::WiFiClientSecure // BearSSL :: WiFiClientSecure
#else
#define THE_CLIENT_TYPE WiFiClient // no TLS 
#endif // USE_TLS_SSL

//#define WIFI_CLIENT_CLASS  WiFiClientSecure //  WiFiClientSecure
#define FOUND_BOARD ESP8266
#endif

#ifdef ARDUINO_ARCH_ESP32
// #define WIFI_CLIENT_CLASS BearSSL::WiFiClientSecure // BearSSL :: WiFiClientSecure not supprted in ESP32 ?
  #undef  WIFI_CLIENT_CLASS
  #ifdef USE_TLS_SSL
  #define WIFI_CLIENT_CLASS  WiFiClientSecure //  WiFiClientSecure default
#else
  #define THE_CLIENT_TYPE WiFiClient // no TLS 
  #endif // USE_TLS_SSL
  #define FOUND_BOARD ESP32
#endif

#ifdef ARDUINO_SAMD_MKRWIFI1010
  // #define WIFI_CLIENT_CLASS BearSSL::WiFiClientSecure // BearSSL :: WiFiClientSecure not supprted in ESP32 ?
  #undef  WIFI_CLIENT_CLASS
  #ifdef USE_TLS_SSL
    #define WIFI_CLIENT_CLASS  WiFiSSLClient 
  #else
    #define THE_CLIENT_TYPE WiFiClient // no TLS 
  #endif // USE_TLS_SSL
  #define FOUND_BOARD ARDUINO_SAMD_MKRWIFI1010
#endif

#ifndef FOUND_BOARD
#pragma message(Reminder "Error Target hardware not defined! (currently only supporting ESP32 and ESP8266)")
#endif // ! FOUND_BOARD


//**************************************************************************************************************
//
//**************************************************************************************************************

// Statements like:
// #pragma message(Reminder "Fix this problem!")
// Which will cause messages like:
// C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
// to show up during compiles. Note that you can NOT use the
// words "error" or "warning" in your reminders, since it will
// make the IDE think it should abort execution. You can double
// click on these messages and jump to the line in question.
//
// see https://stackoverflow.com/questions/5966594/how-can-i-use-pragma-message-so-that-the-message-points-to-the-filelineno
//
#define Stringize( L )     #L 
#define MakeString( M, L ) M(L)
#define $Line MakeString( Stringize, __LINE__ )
#define Reminder __FILE__ "(" $Line ") : Reminder: "


//**************************************************************************************************************
// some optional Serial.print() statements...
//**************************************************************************************************************
#ifdef WIFI_DEBUG
#define WIFI_DEBUG_PRINT(string)           (Serial.print(string))
#define WIFI_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef WIFI_DEBUG
#define WIFI_DEBUG_PRINT(string)           ((void)0)
#define WIFI_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************
#ifdef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)           (Serial.print(string))
#define HTTP_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)           ((void)0)
#define HTTP_DEBUG_PRINTLN(string)         ((void)0)
#endif
//**************************************************************************************************************

//**************************************************************************************************************
#ifdef HTTP_HEADER_DEBUG
#define HTTP_HEADER_DEBUG_PRINT(string)           (Serial.print(string))
#define HTTP_HEADER_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef HTTP_HEADER_DEBUG
#define HTTP_HEADER_DEBUG_PRINT(string)           ((void)0)
#define HTTP_HEADER_DEBUG_PRINTLN(string)         ((void)0)
#endif
//**************************************************************************************************************


//**************************************************************************************************************
#ifdef HTTP_RESPONSE_DEBUG
#define HTTP_RESPONSE_DEBUG_PRINT(string)           (Serial.print(string))
#define HTTP_RESPONSE_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef HTTP_RESPONSE_DEBUG
#define HTTP_RESPONSE_DEBUG_PRINT(string)           ((void)0)
#define HTTP_RESPONSE_DEBUG_PRINTLN(string)         ((void)0)
#endif
//**************************************************************************************************************

//**************************************************************************************************************
#ifdef SCREEN_DEBUG
#define SCREEN_DEBUG_PRINT(string)         (Serial.print(string))
#define SCREEN_DEBUG_PRINTLN(string)       (Serial.println(string))
#endif

#ifndef SCREEN_DEBUG
#define SCREEN_DEBUG_PRINT(string)         ((void)0)
#define SCREEN_DEBUG_PRINTLN(string)       ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef SCREEN_DATA_DEBUG
#define SCREEN_DATA_DEBUG_PRINT(string)    (Serial.print(string))
#define SCREEN_DATA_DEBUG_PRINTLN(string)  (Serial.println(string))
#endif

#ifndef SCREEN_DATA_DEBUG
#define SCREEN_DATA_DEBUG_PRINT(string)    ((void)0)
#define SCREEN_DATA_DEBUG_PRINTLN(string)  ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef JSON_DEBUG
#define JSON_DEBUG_PRINT(string)           (Serial.print(string))
#define JSON_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef JSON_DEBUG
#define JSON_DEBUG_PRINT(string)           ((void)0)
#define JSON_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef TIMER_DEBUG
#define TIMER_DEBUG_PRINT(string)           (Serial.print(string))
#define TIMER_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef TIMER_DEBUG
#define TIMER_DEBUG_PRINT(string)           ((void)0)
#define TIMER_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef RFID_DEBUG
#define RFID_DEBUG_PRINT(string)           (Serial.print(string))
#define RFID_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef RFID_DEBUG
#define RFID_DEBUG_PRINT(string)           ((void)0)
#define RFID_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef HEAP_DEBUG

//static char * HEAP_DEBUG_MSG = "Heap = ";
//#define HEAP_DEBUG_PRINT(string)           (Serial.print  ( (string == DEFAULT_DEBUG_MESSAGE) ? (HEAP_DEBUG_MSG + (String)ESP.getFreeHeap()) : string ) )
//#define HEAP_DEBUG_PRINTLN(string)         (Serial.println( (string == DEFAULT_DEBUG_MESSAGE) ? (HEAP_DEBUG_MSG + (String)ESP.getFreeHeap()) : string ) )
//#define HEAP_DEBUG_PRINTF(string,uint32_t) (Serial.printf (  string,uint32_t)                                                   )

#define DEFAULT_DEBUG_MESSAGE DefaultDebugMessage()
#define SET_HEAP_MESSAGE(thisStr)	       (setHeapMsg(thisStr))

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
	#define HEAP_DEBUG_PRINT(thisStr)          (Serial.print  (  (DefaultDebugMessage().compareTo(thisStr) == 0) ? (getHeapMsg() + (String)ESP.getFreeHeap()) : (String)thisStr + " Heap = " + (String)ESP.getFreeHeap() + "; ") )
	#define HEAP_DEBUG_PRINTLN(thisStr)        (Serial.println(  (DefaultDebugMessage().compareTo(thisStr) == 0) ? (getHeapMsg() + (String)ESP.getFreeHeap()) : (String)thisStr + " Heap = " + (String)ESP.getFreeHeap() + "; ") )
#else
	#define HEAP_DEBUG_PRINT(string)           ((void)0)
	#define HEAP_DEBUG_PRINTF(string)          ((void)0)
	#define HEAP_DEBUG_PRINTLN(string)         ((void)0)
#endif //  ARDUINO_ARCH_ESP8266

#define HEAP_DEBUG_PRINTF(string,uint32_t) (Serial.printf (  string,uint32_t) )
#endif

#ifndef HEAP_DEBUG
static const char* HEAP_DEBUG_MSG = "";
#define SET_HEAP_MESSAGE(string)		   ((void)0)
#define HEAP_DEBUG_PRINT(string)           ((void)0)
#define HEAP_DEBUG_PRINTF(string)          ((void)0)
#define HEAP_DEBUG_PRINTLN(string)         ((void)0)
#endif

//**************************************************************************************************************

//**************************************************************************************************************
#ifdef SPIFFS_DEBUG
#define SPIFFS_DEBUG_PRINT(string)           (Serial.print(string))
#define SPIFFS_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef SPIFFS_DEBUG
#define SPIFFS_DEBUG_PRINT(string)           ((void)0)
#define SPIFFS_DEBUG_PRINTLN(string)         ((void)0)
#endif
//**************************************************************************************************************

//**************************************************************************************************************

void setHeapMsg(String str);

String getHeapMsg();

String DefaultDebugMessage();




#endif // _GLOBALDEFINE_h

