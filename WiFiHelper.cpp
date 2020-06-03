#include "GlobalDefine.h"

#include "WiFiHelper.h"

//#include <ESP8266HTTPClient.h> // includes WiFiClient.h
//#include <ESP8266WiFi.h>


#undef FOUND_BOARD
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
}
#define FOUND_BOARD ESP8266
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <HTTPClient.h>
#include <WiFi.h>
#define FOUND_BOARD ESP32
#endif

#ifndef FOUND_BOARD
#pragma message(Reminder "Error Target hardware not defined !")
#endif // ! FOUND_BOARD


String myMacAddress;

String wifiMacAddress() {
    return myMacAddress;
}

void WiFiStart(bool EnterpriseMode = false) {
	WIFI_DEBUG_PRINTLN(DEBUG_SEPARATOR);
	WIFI_DEBUG_PRINT("wifiConnect: Connecting to ");
	WIFI_DEBUG_PRINTLN(SECRET_WIFI_SSID);
	WIFI_DEBUG_PRINTLN(DEBUG_SEPARATOR);

#ifdef ARDUINO_ARCH_ESP8266
	Serial.println("ARDUINO_ARCH_ESP8266");
#endif
	if (EnterpriseMode) {
		// WPA2 Connection starts here
		// Setting ESP into STATION mode only (no AP mode or dual mode)
		Serial.println("Starting Enterprise...");

#ifdef ARDUINO_ARCH_ESP8266
	// WPA2 Connection starts here
	// Setting ESP into STATION mode only (no AP mode or dual mode)
		Serial.println("Starting ESP8266 Enterprise...");
		wifi_set_opmode(STATION_MODE);

		struct station_config wifi_config;
		memset(&wifi_config, 0, sizeof(wifi_config));
		strcpy((char*)wifi_config.ssid, SECRET_WIFI_SSID);

		wifi_station_set_config(&wifi_config);

		wifi_station_clear_cert_key();
		wifi_station_clear_enterprise_ca_cert();
		wifi_station_set_wpa2_enterprise_auth(1);
		wifi_station_set_enterprise_identity((uint8*)SECRET_EAP_ID, strlen(SECRET_EAP_ID));
		wifi_station_set_enterprise_username((uint8*)SECRET_EAP_USERNAME, strlen(SECRET_EAP_USERNAME));
		wifi_station_set_enterprise_password((uint8*)SECRET_EAP_PASSWORD, strlen(SECRET_EAP_PASSWORD));
		wifi_station_connect();
		// WPA2 Connection ends here
#endif
	}
	else {
		Serial.println("Starting regular Wi-Fi...");
		HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
		WiFi.mode(WIFI_STA);
		HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
		WiFi.begin(SECRET_WIFI_SSID, WIFI_PWD);
		HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	}
}

//*******************************************************************************************************************************************
// wifiConnect 
// 
//   WiFi.begin with repeated attempts with TFT screen and optional serial progress indication
//
//*******************************************************************************************************************************************
int wifiConnect(int maxAttempts) {
	int countAttempt = 0;
	WiFiStart(IS_EAP);


	myMacAddress = WiFi.macAddress(); // this returns 6 hex bytes, delimited by colons

	WIFI_DEBUG_PRINTLN("Starting WiFi Connection Loop...");
	while (WiFi.status() != WL_CONNECTED) {  // try to connect wifi for 6 sec then reset

											 // this tft code is not actualy DOING anything yet

		WIFI_DEBUG_PRINT(".");
		delay(250);
        if (IS_EAP) {
			// TODO - do we ever give up on EAP?
        }
        else
        {
            countAttempt++;
            if (countAttempt > maxAttempts) {
				countAttempt = 0;
				WIFI_DEBUG_PRINTLN("WiFi Disconnect... ");
				WiFi.disconnect();
				delay(5000);
				WIFI_DEBUG_PRINTLN("WiFi Retrying. ");
				WiFi.mode(WIFI_STA);
				WiFi.begin(SECRET_WIFI_SSID, WIFI_PWD);
				// TODO reboot?
			}
		}
	}
	WIFI_DEBUG_PRINTLN("Connected!");
	delay(5000); // TODO why wait?
	myMacAddress.replace(":", "");
	myMacAddress.replace("-", ""); // probably not used, but just in case they MAC address starts returning other well known delimiters such as dash
	myMacAddress.replace(" ", ""); // or perhaps even a space

	Serial.println("MAC Address=" + myMacAddress);
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	WIFI_DEBUG_PRINT("wifiConnect: Done! /n/n");
	return 0;
}