//
// see: https://github.com/miguelbalboa/rfid
//

#include <SPI.h>
#include <MFRC522.h>
// #include <WiFi.h>
#include <WiFiClientSecure.h>
#include "GlobalDefine.h"
#include "WiFiHelper.h"
#include "sslHelper.h"

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

void setup() {
	Serial.begin(115200);
	while (!Serial) {
		delay(10);
	}
	Serial.println("Go!");

	wifiConnect(50);
	testSSL();

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

		// Halt PICC
		rfid.PICC_HaltA();

		// Stop encryption on PCD
		rfid.PCD_StopCrypto1();
	}
	else {
		delay(100);
	}

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