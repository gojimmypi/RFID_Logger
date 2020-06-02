#RFID Logger

This RFID Logger project uses an ESP32 board such as [this ThingPulse board](https://thingpulse.com/product/epulse-low-power-esp32-development-board/)
and and RFID reader such as [this Mifare RC522 RFID Reader on Amazon](https://smile.amazon.com/gp/product/B07KGBJ9VG/).
The intention of this project is to keep track of staff movements similar to an IO/OUT attendance board. Clearly much of the reliability
is based on the honor system to ensure cards are consitently used.

The ESP32 waits for a card to be detected. Once a casrd is present, the UID is read and sent to a web server.

This solution should NOT be used for security applications, as noted [here](https://github.com/miguelbalboa/rfid#security) and repeated here:

Security
-------
* The **UID** of a card **can not be used** as an unique identification for security related projects. Some Chinese cards allow to change the UID which means you can easily clone a card. For projects like *access control*, *door opener* or *payment systems* you **must implement** an **additional security mechanism** like a password or normal key.

* This library only supports crypto1-encrypted communication. Crypto1 has been known as `broken`_ for a few years, so it does NOT offer ANY security, it is virtually unencrypted communication. **Do not use it for any security related applications!**

* This library does not offer 3DES or AES authentication used by cards like the Mifare DESFire, it may be possible to be implemented because the datasheet says there is support. We hope for pull requests :).



* [Random Nerd Tutorials: ESP32 External Wake Up from Deep Sleep](https://randomnerdtutorials.com/esp32-external-wake-up-deep-sleep/)
* [Nordic ID: How Different Power Saving Options Affect Your RFID Reader’s Battery Life and Power Consumption](https://www.nordicid.com/resources/expert-article/how-different-power-saving-options-affect-your-rfid-readers-battery-life-and-power-consumption/)
* [Andreas Spiess YouTube: BLE Human Presence Detector using an ESP32 (Tutorial, Arduino IDE)](https://www.youtube.com/watch?v=KNoFdKgvskU)
* [Use an ESP8266 Beacon Spammer to Track Smartphone Users](https://null-byte.wonderhowto.com/how-to/use-esp8266-beacon-spammer-track-smartphone-users-0187599/)
