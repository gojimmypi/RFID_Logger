# RFID Logger

This RFID Logger project uses [this ThingPulse board](https://thingpulse.com/product/epulse-low-power-esp32-development-board/)
(likely adaptable to other ESP32 boards, as well) along with an RFID reader such as [this Mifare RC522 RFID Reader on Amazon](https://smile.amazon.com/gp/product/B07KGBJ9VG/).
The intention of this project is to keep track of staff movements similar to an IN/OUT attendance board. Clearly much of the reliability
is based on the honor system to ensure cards are consitently used. 

The ESP32 waits for a card to be detected. Once a card is present, the UID is read and sent to a web server.

At this time both the ESP8266 and ESP32 are supported, using either regular or enterprise Wi-Fi, and communicating over SSL. 

In concept, this project could be used to control pretty much anything with the swipe of an RFID card.

The code is Arduino-style C created in Visual Studio usign the [Visual Micro Extension](https://www.visualmicro.com/); see also the [Arduino IDE for Visual Studio (Visual Micro) on the marketplace](https://marketplace.visualstudio.com/items?itemName=VisualMicro.ArduinoIDEforVisualStudio). 
It should also work just fine in the Arduino IDE (see the [RFID_Logger.ino](./RFID_Logger.ino) file)

This solution should NOT be used for security applications, as noted [here](https://github.com/miguelbalboa/rfid#security) and repeated here:

# Security
-------
* The **UID** of a card **can not be used** as an unique identification for security related projects. Some Chinese cards allow to change the UID which means you can easily clone a card. For projects like *access control*, *door opener* or *payment systems* you **must implement** an **additional security mechanism** like a password or normal key.

* This library only supports crypto1-encrypted communication. Crypto1 has been known as _broken_ for a few years, so it does NOT offer ANY security, it is virtually unencrypted communication. **Do not use it for any security related applications!**

* This library does not offer 3DES or AES authentication used by cards like the Mifare DESFire, it may be possible to be implemented because the datasheet says there is support. We hope for pull requests :).

# Libraries needed:

In the Arduino IDE, under `File` - `Prewferences`, enter a comma-delimited `Additional Boards Manager URL` such as these two for the ESP8266 and ESP32:

```
https://arduino.esp8266.com/stable/package_esp8266com_index.json,https://dl.espressif.com/dl/package_esp32_index.json
```



# Parts

* [ePulse – Low Power ESP32 development board](https://thingpulse.com/product/epulse-low-power-esp32-development-board/)
* [Mifare RC522 RF IC Card Sensor Module](https://www.amazon.com/dp/B01CSTW0IA/)
* [Project Enclosure](https://www.amazon.com/dp/B07TYNYW1S/) (consider getting something with mounting ears if needed, and if you need room for a battery)
* internal mounting hardware: platic screws, ziptie, foam to insulate boards to avoid touching each other.

# Not PN532 NFC/RFID

This code is not compatible with the PN532 NFC/RFID technology, such as the [Adafruit PN532 ](https://www.adafruit.com/products/364)

# Troubleshooting

* Make sure firewall port 443 is open on target system for the Wi-Fi IP address of your device. (beware DHCP addresses can _change_ over time)

# Resources, Inspiration, Credits, and Other Links: 
* [Visual Studio Tips and Traps](https://gojimmypi.blogspot.com/2020/05/visual-studio-fpga-tips-and-traps.html)
* [Random Nerd Tutorials: ESP32 External Wake Up from Deep Sleep](https://randomnerdtutorials.com/esp32-external-wake-up-deep-sleep/)
* [Nordic ID: How Different Power Saving Options Affect Your RFID Reader’s Battery Life and Power Consumption](https://www.nordicid.com/resources/expert-article/how-different-power-saving-options-affect-your-rfid-readers-battery-life-and-power-consumption/)
* [Andreas Spiess YouTube: BLE Human Presence Detector using an ESP32 (Tutorial, Arduino IDE)](https://www.youtube.com/watch?v=KNoFdKgvskU)
* [Use an ESP8266 Beacon Spammer to Track Smartphone Users](https://null-byte.wonderhowto.com/how-to/use-esp8266-beacon-spammer-track-smartphone-users-0187599/)
* [TS9013 500mA Low Quiescent Current CMOS LDO](https://www.mouser.com/datasheet/2/395/TS9013_I1608-1115581.pdf)
* [ePulse – Low Power ESP32 development board](https://thingpulse.com/product/epulse-low-power-esp32-development-board/)
* [JeroenBeemster ESP32 WPA2 Enterprise example simplified for Arduino](https://github.com/JeroenBeemster/ESP32-WPA2-enterprise)
* [arduino-esp32 WiFiClientSecureEnterprise.ino example](https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFiClientSecure/examples/WiFiClientSecureEnterprise/WiFiClientSecureEnterprise.ino)
* [esp8266wifi client secure examples](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-secure-examples.html)
