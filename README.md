# ESP8266_SubPos
A SubPos migration to ESP8266 IDE

I have started to work through the SubPos Node code (see subpos.org). That runs an ESP8266 (ESP12) via a PIC chip using AT commands to create a beacon frame containing location information. I am attempting to take the code and convert it to use the Arduino IDE/API and program the ESP8266 directly.

## Progress

### 26 March 2017

Messy code I know but more interested to get it working and then worry about enchancing the code. At present it generates a beacon frame on a random channel and transmits it. Wireshark detects it althoug it cannot decode it. I have not yet coded a decoder. 

### Todo
- [ ] Keeps crashing, need to sort
- [ ] Port ISR code over rather than a loop to call beacon code.
- [ ] Port spread spectrum / frequency hoping over.
- [ ] Port USB config over.
- [ ] Code a receiever/decoder.
