# Remote ESP32 pin controller
## Premise

This is a small project to research the interaction of MQTT with the ESP32, and establish communication over MQTT to control a pin on the ESP32 over a wide network.

A .net MAUI app was made that contacts the MQTT server and send commands to the ESP32 while receiving constant feedback about the state of the pin (HIGH/LOW) from the ESP32.

## Checklist
- [x] Setup MQTT server (Mosquitto)
- [x] ESP32 MQTT communication
- [x] .net MAUI app MQTT communication
- [x] .net MAUI app ESP32 communication
- [x] Port .net MAUI app to mobile
- [x] Link ESP32 to LED and test over long range 
- [ ] (Optional) Timer interrupt on ESP32 for constant status updates
- [ ] (Optional) Dynamic pins (Sent from ESP32 on connection)
- [ ] (Optional) JSON packet to control multiple pins and their states at once
- [ ] (Optional) Ability to set alternating state 
- [ ] (Optional) Make ESP32 gui look better

## URGENT

Current version uses interrupts to send mqtt data, however, pubsubclient (the current library) crashes the esp32 every time it attempts to publish from within an interrupt. Fix: Find a new way to connect to mqtt (not PubSubClient)