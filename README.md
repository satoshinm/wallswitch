# wallswitch: smarten up your dumb light switch

Arduino sketch for ESP8266 D1 Mini to sense wall light switches from an AC outlet

* Plug in a USB charger to an always-on AC outlet, and to the ESP8266 power
* Plug in a USB charger to the wall AC outlet which connects to the light switch, wire as follows:
* D2: Connect to optocoupler detector, example for FOD817:
 * 10k pullup resistor to 3V3 on phototransistor output
 * 220 ohm current limiting resistor on light-emitting diode input, to 5V and ground on charger
* (Optional) A0: connect via 47k/10k voltage divider to 5V from the charger, ground to ground - for voltage sensing
* Edit creds.h with your Wi-Fi network information

Packets will be sent when D2 changes. A web server at http://wallswitch.local shows the optocoupler status and measured voltage.

Intended for use with home automation, to allow a device to activate in response to an ordinary home wall switch.

## License

MIT
