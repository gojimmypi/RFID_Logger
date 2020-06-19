/*
MIT License

Copyright(c) 2020 gojimmypi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

//
// see: https://github.com/miguelbalboa/rfid
//
#include "GlobalDefine.h"
#include <vector>
#include <stdlib.h>
#include <SPI.h>
#include <MFRC522.h>
#ifdef ARDUINO_ARCH_ESP8266
#include <WiFiClientSecure.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
#include <WiFiClientSecure.h>
#endif
#ifdef ARDUINO_SAMD_MKRWIFI1010
#include <WiFiNINA.h>
#endif

#include "WiFiHelper.h"
#include "sslHelper.h"
#include "htmlHelper.h"

// Define our SPI connection and parameters for the RFID reader (VSPI)
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
#define SS_PIN    21
#define RST_PIN   22
#endif
#if defined(ARDUINO_SAMD_MKRWIFI1010)
#define SS_PIN    7
#define RST_PIN   6
#endif


#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16


// Initialize the Wi-Fi client library
// WiFiClient client; // this is the non-TLS/SSL one!

// Use WiFiClientSecure class to create TLS connection
WIFI_CLIENT_CLASS client;

// Initialize our FRID reader with a single instance:
MFRC522 rfid(SS_PIN, RST_PIN);

// Initialize our RFID key (new CARDSN/UID values found here)
MFRC522::MIFARE_Key key;

// Init array that will store new NUID 
byte nuidPICC[4];

void WebServerConnect() {
	while (!client.connect(SECRET_APP_HOST, APP_HTTPS_PORT)) {
		// TODO what if the web server is down? we shouldn't wait here forever...
		Serial.println(F("client.connect failed; check firewall on receiver"));
		Serial.print(F("IP address="));
		Serial.println(WiFi.localIP());
		Serial.print(F("MAC address="));
		Serial.println(wifiMacAddress());
		int retry = 0;
		for (size_t i = 60; i > 0; i--)
		{
			Serial.print(F("."));
			Serial.print(i);
			delay(1000);
		}
		Serial.println();
		Serial.println(F("Trying again!"));
	}

}

bool WiFiConnected() {
	return (WiFi.status() == WL_CONNECTED);
}

typedef struct ItemUID {
	String UID = "";
//	String MAC = "";
	String IP = "";
	String MSG = "";
	bool sent = false;
};


std::vector<ItemUID> QueueOfUID;
ulong SendAttempts = 0;

int SendQueue() {
	// WiFiStart occurs in SaveUID()
	if (QueueOfUID.empty()) {
		Serial.println(F("QueueOfUID is empty!"));
		return 0;
	}

	SendAttempts++;

	if (WiFiConnected()) {
		WebServerConnect();

		if (!client.connected()) {
			Serial.println(F("SaveUID when wifi client.connected is false; check firewall on receiver"));
			Serial.print(F("IP address="));
			Serial.println(WiFi.localIP());
			return 2;
		}

		Serial.println(F(""));
		Serial.print("Items to check: ");
		Serial.println(QueueOfUID.size());

		bool HasUnsentItems = false;
		int thisResult = -1;
		for (uint i = 0; i < QueueOfUID.size(); i++)
		{
			if (QueueOfUID[i].sent) {
				Serial.println(F("Item already sent"));
			}
			else {
				Serial.println(F("Saving UID"));
				String url = String(SECRET_APP_PATH)
					+ F("?UID=") + QueueOfUID[i].UID
					+ F("&MAC=") + wifiMacAddress()
					+ F("&MSG=") + QueueOfUID[i].MSG; // reminder that IIS will return a 302 (moved) for default.aspx that points to default  :/
				String thisRequest = HTML_RequestText(url);
				String thisMovedRequestURL = "";
				thisResult = HTML_SendRequestFollowMove(&client, thisRequest, thisMovedRequestURL);
				if (thisResult == 0) {
					// TODO check if successful, for now, assume it was 
					// HasUnsentItems = true // if it fails we have an unsent item
					Serial.println(F("Item successfully sent!"));
					QueueOfUID[i].sent = true;
				}
				else {
					Serial.println(F("Item NOT sent!"));
					HasUnsentItems = true;
				}


				client.flush();
				delay(100);
				client.stop();
			}
			if (HasUnsentItems) {
				// if we can't send one, we probably can't send any others, so give up
				Serial.println(F("Giving up..."));
				break;
			}
		}

		if (HasUnsentItems) {
			Serial.println(F("There are still unsent items!"));
		}
		else {
			Serial.println(F("Clearing unsent item list..."));
			QueueOfUID.clear();
			Serial.println(F("Cleared unsent item list!"));
			SendAttempts = 0;
		}
	}
	else {
		Serial.print(F("Exiting, WiFi not connected; item not saved; "));
		Serial.print(String(QueueOfUID.size()));
		Serial.println(F(" Items queued"));
		return 3;
	}

}

int SaveUID(String thisUID, String thisMessage) {
	if (thisUID) {
		ItemUID newItem;
		newItem.UID = thisUID;
//		newItem.MAC = wifiMacAddress();
		newItem.MSG = thisMessage;
		newItem.sent = false;
		QueueOfUID.push_back(newItem);

		Serial.println(F("Queue Size: "));
		Serial.print(QueueOfUID.size());

		// we'll only try to turn on WiFi when we know we have an item to send
		if (!WiFiConnected()) {
			WiFiStart(IS_EAP);
		}

		// it's unlikely that we'll be able to connect yet, but let's try:
		SendQueue();
		return 0;
	}
	else {
		return 1;
	}
}

#ifdef _MSC_VER
#pragma region helpers
#endif

String UID_Hex(byte* buffer, byte bufferSize) {
	String res = "";
	for (byte i = 0; i < bufferSize; i++) {
		res += HEX_CHARS[(buffer[i] >> 4) & 0xF];
		res += HEX_CHARS[buffer[i] & 0xF];
	}
	return res;
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void printHex(byte* buffer, byte bufferSize) {
	for (byte i = 0; i < bufferSize; i++) {
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], HEX);
	}
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte* buffer, byte bufferSize) {
	for (byte i = 0; i < bufferSize; i++) {
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], DEC);
	}
}

#ifdef _MSC_VER
#pragma endregion helpers
#endif
 


void setup() {
	Serial.begin(115200);
	while (!Serial) {
		delay(10);
	}
	Serial.println(F("Hello RFID_Logger!"));

	SPI.begin(); // Init SPI bus
	rfid.PCD_Init(); // Init MFRC522 

	for (byte i = 0; i < 6; i++) {
		key.keyByte[i] = 0xFF;
	}

	Serial.println(F("This code scans the MIFARE Classsic NUID."));
	Serial.print(F("Using the following key:"));
	printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

	SaveUID("00000000", "Startup"); // save a marker at startup time

	// some reconnection tests...
	//delay(1000);
	//SaveUID("00000001", "Startup"); // save a marker at startup time
	//delay(5000);
	//SaveUID("00000005", "Startup"); // save a marker at startup time
	//delay(20000);
	//SaveUID("00000020", "Startup"); // save a marker at startup time
	//delay(60000);
	//SaveUID("00000060", "Startup"); // save a marker at startup time
	//delay(120000);
	//SaveUID("00000120", "Startup"); // save a marker at startup time
	//delay(240000);
	//SaveUID("00000240", "Startup"); // save a marker at startup time
	//delay(1000000);
	//SaveUID("00001000", "Startup"); // save a marker at startup time

	Serial.println(F("Setup complete: awaiting card..."));
}

bool IsCardReady() {
	if (!rfid.PICC_IsNewCardPresent()) {
		return false;
	}
	if (!rfid.PICC_ReadCardSerial()) {
		RFID_DEBUG_PRINT(F("PICC_IsNewCardPresent but not PICC_ReadCardSerial."));
		return false;
	}
	MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
	if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
		piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
		piccType != MFRC522::PICC_TYPE_MIFARE_4K) {

		RFID_DEBUG_PRINT(F("Card type not valid."));
		return false;
	}
	return true;
}

// these counters are only general approximations, not accurate time.
int minutes_uptime = 0;
int hours_uptime = 0;
int days_uptime = 0;
int this_ms_counter = 0;
int save_result = 0;

// wait [ms_count] milliseconds; attempt to send data every minute
void wait_ms(int ms_count) {
	this_ms_counter++;
	unsigned long CodeStart = millis();
	bool SentQueue = false; // we'll keep track of whether we attempted a slow operation, like talking to the web server

	if (this_ms_counter >= (60000 / ms_count) ) { 
		minutes_uptime++;
		this_ms_counter = 0;
		Serial.println("Uptime: " + String(days_uptime) + " days; "
		                          + String(hours_uptime) + " hours; "
			                      + String(minutes_uptime) + " minutes. "
		                          + "Queue Size: " + String(QueueOfUID.size()));

		// code that runs every minute

		// once per minute, we'll need to try to connect again if the queue has items to be sent
		if (QueueOfUID.empty()) {

		}
		else {
			// TODO for low power operations, we probably don't want to try this every minute
			Serial.print(F("Send attempt: "));
			Serial.println(SendAttempts);
			SendQueue();
			SentQueue = true;
		}
	}

	// keep track of hours
	if (minutes_uptime >= 60) {
		hours_uptime++;
		minutes_uptime = 0;
       
		// any hourly code would go here
	}

	// keep track of days
	if (hours_uptime >= 24) {
		days_uptime++;
		hours_uptime = 0;

		// any daily code would go here
	}

	// CodeDuration is typically 0, as the above will take less than a millisecond, unless data sent to web server
	unsigned long CodeDuration = millis() - CodeStart;

	if (SentQueue) {
		Serial.print(F("Code Duration: "));
		Serial.println(CodeDuration);
	}
	if ((CodeDuration >= 0) && (CodeDuration < ms_count)) {
		int thisDelay = ms_count - CodeDuration;
		if (SentQueue || thisDelay > -1) {
			TIMER_DEBUG_PRINT(F("waiting: "));
			TIMER_DEBUG_PRINTLN(thisDelay);
		}
		delay(thisDelay);
	}
	else {
		// no delay, we already spend more time in code than the delay!
		Serial.print(F("No delay! Duration="));
		Serial.println(CodeDuration);
		this_ms_counter = this_ms_counter + CodeDuration;
	}
}

void loop() {
	// check to see if we have a card
	if (IsCardReady()) {

		Serial.print(F("Detected UID: "));
		printHex(rfid.uid.uidByte, rfid.uid.size);
		Serial.println();
		Serial.print(F("aka: "));

		String detectedUID = UID_Hex(rfid.uid.uidByte, rfid.uid.size);
		Serial.println(detectedUID);

		save_result = SaveUID(detectedUID, F("Detected"));
		if (save_result == 0) {
			Serial.println(F("Data queued. Awaiting next card..."));
		}
		else {
			Serial.println(F("Error! Data NOT queued. Awaiting next card..."));
		}

		// Halt PICC
		rfid.PICC_HaltA();

		// Stop encryption on PCD
		rfid.PCD_StopCrypto1();
	}
	else {
		wait_ms(100); // data sent while waiting, too
	}

	// check to see of we have any items still queued to send
	if (QueueOfUID.empty()) {
		if (WiFiConnected()) {
			Serial.println(F("WiFi.disconnect"));
			WiFi.disconnect();
		}
	}
	else {
	}
}

