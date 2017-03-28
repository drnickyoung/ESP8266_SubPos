# ESP8266_SubPos
A SubPos migration to ESP8266 IDE

I have started to work through the SubPos Node code (see subpos.org). That runs an ESP8266 (ESP12) via a PIC chip using AT commands to create a beacon frame containing location information. I am attempting to take the code and convert it to use the Arduino IDE/API and program the ESP8266 directly.

## Progress

Code works and broadcasts. 

## Todo
- [x] Keeps crashing, need to sort
- [ ] Port ISR code over rather than a loop to call beacon code.
- [x] Port spread spectrum / frequency hoping over.
- [ ] Port USB config over.
- [ ] Code a receiever/decoder.
