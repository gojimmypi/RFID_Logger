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

#include <SPI.h>
#include <MFRC522.h>
#include <WiFiClientSecure.h>
#include "GlobalDefine.h"
#include "WiFiHelper.h"
#include "sslHelper.h"
#include "htmlHelper.h"

// Define our SPI connection and parameters for the RFIS reader (VSPI)
#define SS_PIN    21
#define RST_PIN   22
#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16


// Initialize the Wi-Fi client library
// WiFiClient client;
// Use WiFiClientSecure class to create TLS connection
WiFiClientSecure client;


// Initialize our FRID reader with a single instance:
MFRC522 rfid(SS_PIN, RST_PIN);

// Initialize our RFID key will be found (new CARDSN/UID values found here)
MFRC522::MIFARE_Key key;

// Init array that will store new NUID 
byte nuidPICC[4];

int SaveUID(String thisUID) {

	if (thisUID) {
		if (!client.connected()) {
			Serial.println("SaveUID when wifi client.connected is false; check firewall on receiver");
			Serial.print("IP address=");
			Serial.println(WiFi.localIP());
			return 2;
		}

		Serial.println("Saving UID");
		String url = "/RFID/default.aspx?UID=" + thisUID; // reminder that IIS will return a 302 (moved) for default.aspx that points to default  :/
		String thisRequest = HTML_RequestText(url);
		String thisMovedRequestURL = "";
		HTML_SendRequestFollowMove(&client, thisRequest, thisMovedRequestURL);

		return 0;
	}
	else {
		return 1;
	}
}

#ifdef _MSC_VER
#pragma region helpers
#endif

static const char* HEX_CHARS = "0123456789ABCDEF";
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
	Serial.println("Hello RFID_Logger!");

	wifiConnect(50);

	while (!client.connect(SECRET_APP_HOST, APP_HTTPS_PORT)) {
		Serial.println("client.connect failed; check firewall on receiver");
		Serial.print("IP address=");
		Serial.println(WiFi.localIP());
		Serial.print("MAC address=");
		Serial.println(WiFi.macAddress());
		int retry = 0;
		for (size_t i = 60; i > 0; i--)
		{
			Serial.print(".");
			Serial.print(i);
			delay(1000);
		}
		Serial.println();
		Serial.println("Trying again!");
	}

	SaveUID("00000000"); // save a marker at startup time

	// testSSL();

	SPI.begin(); // Init SPI bus
	rfid.PCD_Init(); // Init MFRC522 

	for (byte i = 0; i < 6; i++) {
		key.keyByte[i] = 0xFF;
	}

	Serial.println(F("This code scan the MIFARE Classsic NUID."));
	Serial.print(F("Using the following key:"));
	printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

}

bool IsCardReady() {
	if (!rfid.PICC_IsNewCardPresent()) {
		return false;
	}
	if (!rfid.PICC_ReadCardSerial()) {
		Serial.println(F("PICC_IsNewCardPresent but not PICC_ReadCardSerial."));
		return false;
	}
	MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
	if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
		piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
		piccType != MFRC522::PICC_TYPE_MIFARE_4K) {

		Serial.println(F("Card type not valid."));
		return false;
	}
	return true;
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
		SaveUID(detectedUID);

		// Halt PICC
		rfid.PICC_HaltA();

		// Stop encryption on PCD
		rfid.PCD_StopCrypto1();
	}
	else {
		delay(100);
	}

}

