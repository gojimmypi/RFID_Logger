# RFID Logger Enclosure

This project is extremely easy to house in an enclosure. There are no buttons, no moving parts, no display!

Often the hard part for the DIY maker is getting USB power to to board inside the enclosure. One method
is to perfectly mount the board near the perimeter of the box. Then, use a drill or dremmel tool to cut
a nice, perfectly shaped USB connector hole. Easier said than done.

One possibility is to 3D print an enclosure. Not everyone has accecss to a 3D print

An alternative method is to drill a hole at seam between the lid and the box. For best results, have the lid
snuggly secured when drilling:

![drill_enclosure_hole.png](./images/drill_enclosure_hole.png)

With just-the-right-size bit (typically 1/8") - this will allow the USB housing to be used as a strain relief:

![enclosure_hole_opened_closeup](./images/enclosure_hole_opened_closeup.png)

As half of the hol is in the base, and the other it in in lid, cable insertion is quite easy:

![enclosure_hole_opened_super_closeup](./images/enclosure_hole_opened_super_closeup.png)

USB cables come in different size diameters, to it is best to measure to get a nice tight fit.

For long term durability it is best to get boards withouot headers already soldered, 
and solder the wires directly to the board. In for prototypes, it is usually best to use jumper wires.

Here's a closeup of jumper wires connected to the RFID Reader Board:

![rfid_rc522_wiring](./images/rfid_rc522_wiring.png)

Here's a closeup of the other end of the wires, conencted to the MKR WiFi 1010 board:

![arduino_wiring_closeup](./images/arduino_wiring_closeup.png)

Both boards connected to each other:

![arduino_rfid_connections](./images/arduino_rfid_connections.png)

Test fit:

![arduino_rfid_test_fit](./images/arduino_rfid_test_fit.png)

Held in place for gluing:

![glue_rfid_ardunio_mkr](./images/glue_rfid_ardunio_mkr.png)

Enclosure lid in place:

![enclosure_hole_closed_closeup](./images/enclosure_hole_closed_closeup.png)

Note that for maximum range, the RFID PCB with the printing should be as close to the swipe seruface as possible. 
The side with the printing is the side with the copper traces as an antenna. It will work from either side, but 
might as well do everything possible for best range.



The Atmel ICE Squid Cable Connections:

```
             USB

         NC -  - 3  (GND)
(SWDCLK)  4 -  - 0  (RST)
(SWDIO)   2 -  - 1  (3v3)

 (as viewed from bottom)
```
See also [SWD Debugging the Arduino MKR-WiFi-1010 with Atmel ICE](https://gojimmypi.blogspot.com/2018/12/swd-debugging-arduino-mkr-wifi-1010.html)