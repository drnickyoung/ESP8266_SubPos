/*
 * Aim is to implement SubPos using Beacon frames for ESP8266 using the Arduino IDE
 * 
 * Based on http://nomartini-noparty.blogspot.co.uk/2016/07/esp8266-and-beacon-frames.html
 */

#include <ESP8266WiFi.h> //more about beacon frames https://mrncciew.com/2014/10/08/802-11-mgmt-beacon-frame/
#include "SSID_Coding.h"

extern "C" {
  #include "user_interface.h"
}

#define _DEBUG TRUE

void setup() {
#ifdef _DEBUG  
  Serial.begin(115200);
#endif 
  
  delay(500);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1); 

  struct sps_data encode_data; 
  
  //Test data
  encode_data.dev_id      = 99;
  encode_data.lat         = 891234567;
  encode_data.lng         = 997654321;
  encode_data.altitude    = 12345678;
  encode_data.tx_pwr      = -900;
  encode_data.off_map     = 1;
  encode_data.three_d_map = 1;
  encode_data.path_loss   = 2;
  encode_data.res         = 0x2f;
  encode_data.app_id      = 99;

 
  
  //Create location SSID here
  encoded_string = encode_ssid(encode_data);

#ifdef _DEBUG
  //SubPos set SSID to a fixed 31 bytes
  
  Serial.println(" ");
  Serial.print("SSID: "); 
  for(int i = 0; i < 31; i++)
    Serial.print(encoded_string[i]); 
  
  Serial.println(" ");
  Serial.println("Setup done");
#endif

}

void loop() {
  sendBeacon("test"); //sends beacon frames with the SSID 'test'
  sendBeacon(encoded_string);
  delay(100);
}

void sendBeacon(char* ssid) {
    // Randomize channel //
    byte channel = 1;//random(1,12); 
    wifi_set_channel(channel);

    uint8_t packet[128] = { 0x80, 0x00, //Frame Control 
                        0x00, 0x00, //Duration
                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //Destination address 
                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //Source address - overwritten later
                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //BSSID - overwritten to the same as the source address
                /*22*/  0xc0, 0x6c, //Seq-ctl
                //Frame body starts here
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, //timestamp - the number of microseconds the AP has been active
                /*32*/  0xFF, 0x00, //Beacon interval
                /*34*/  0x01, 0x04, //Capability info
                /* SSID */
                /*36*/  0x00
                };

    int ssidLen = strlen(ssid);

    //HACK---------------------------------------------------------------
    //SPS limits this to 31. As strlen seems to have issues on ESP8266
    //force it
    if (ssidLen>31)
      ssidLen=31;
    //HACK---------------------------------------------------------------  
      
    packet[37] = ssidLen;

    for(int i = 0; i < ssidLen; i++) {
      packet[38+i] = ssid[i];
    }

    uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                        0x03, 0x01, 0x04 /*DSSS (Current Channel)*/ };

    for(int i = 0; i < 12; i++) {
      packet[38 + ssidLen + i] = postSSID[i];
    }

    packet[50 + ssidLen] = channel;

    //Get true Mac address
    byte mac[6];
    WiFi.macAddress(mac);
    packet[10] = packet[16] = mac[5];
    packet[11] = packet[17] = mac[4];
    packet[12] = packet[18] = mac[3];
    packet[13] = packet[19] = mac[2];
    packet[14] = packet[20] = mac[1];
    packet[15] = packet[21] = mac[0];
    // Randomize SRC MAC
    /*packet[10] = packet[16] = random(256);
    packet[11] = packet[17] = random(256);
    packet[12] = packet[18] = random(256);
    packet[13] = packet[19] = random(256);
    packet[14] = packet[20] = random(256);
    packet[15] = packet[21] = random(256);*/

    int packetSize = 51 + ssidLen;

    wifi_send_pkt_freedom(packet, packetSize, 0);
    //wifi_send_pkt_freedom(packet, packetSize, 0);
   //wifi_send_pkt_freedom(packet, packetSize, 0);
    delay(1);
}

/*
 * SSID Encoder
 * From: https://github.com/subpos/ssid_coder/blob/master/coder/ssid_coder.c
 */
char * encode_ssid(struct sps_data encode_data){

  char *encoded_string = (char *) malloc(sizeof(char) * 31);
  //For the string data, we can just read it raw; nothing fancy here.
  //Encoding SSIDs doesn't happen often.
  
  //SPS TAG
  encoded_string[ 0] = tag[0];
  encoded_string[ 1] = tag[1];
  encoded_string[ 2] = tag[2];
  
  //Device ID
  encoded_string[ 3] = (encode_data.dev_id        >> 16) & 0xFF;
  encoded_string[ 4] = (encode_data.dev_id        >>  8) & 0xFF;
  encoded_string[ 5] = (encode_data.dev_id             ) & 0xFF;
  
  //Latitude
  encoded_string[ 6] = (encode_data.lat           >> 24) & 0xFF;
  encoded_string[ 7] = (encode_data.lat           >> 16) & 0xFF;
  encoded_string[ 8] = (encode_data.lat           >>  8) & 0xFF;
  encoded_string[ 9] = (encode_data.lat                ) & 0xFF;

  //Longitude
  encoded_string[10] = (encode_data.lng           >> 24) & 0xFF;
  encoded_string[11] = (encode_data.lng           >> 16) & 0xFF;
  encoded_string[12] = (encode_data.lng           >>  8) & 0xFF;
  encoded_string[13] = (encode_data.lng                ) & 0xFF;
    
  //Application ID
  encoded_string[14] = (encode_data.app_id        >> 16) & 0xFF;
  encoded_string[15] = (encode_data.app_id        >>  8) & 0xFF;
  encoded_string[16] = (encode_data.app_id             ) & 0xFF;
  
  //Altitude
  encoded_string[17] = (abs(encode_data.altitude) >> 18) & 0xFF;
  encoded_string[18] = (abs(encode_data.altitude) >> 10) & 0xFF;
  encoded_string[19] = (abs(encode_data.altitude) >>  2) & 0xFF;
  
  encoded_string[20] = ((abs(encode_data.altitude)        & 0x03) << 6)
                       | (encode_data.altitude < 0) << 5
                       |  encode_data.off_map       << 4 
                       |  encode_data.three_d_map   << 3 
                       | ((encode_data.tx_pwr + 1000)>> 8   & 0x07); //Note that tx_power is assumed to be shifted (x10)
  encoded_string[21] = (encode_data.tx_pwr + 1000)       & 0xFF;
  

  encoded_string[22] = (encode_data.path_loss     << 5)  & 0xE0
                     |((encode_data.res >> 8) & 0x1F);
  encoded_string[23] = (encode_data.res               )  & 0xFF;

  //Transform encoded string and create "ASCII" 7bit mask
  int x;
  uint32_t ascii_mask = 0; //21 bits (for 21 bytes) to encode
  for (x = 3; x <= 23; x++)
  {
    //check if MSB of byte is 1
    if ((encoded_string[x] & 0x80) == 0x80) {
      encoded_string[x] = encoded_string[x] & 0x7F;
      ascii_mask = (ascii_mask << 1) | 1;
    } else {
      ascii_mask = (ascii_mask << 1) | 0;
    }
    //printf("%x\n",ascii_mask);
  }
  
  //create mask in such a way that we don't have to mask the mask:
  //7 bit coding since we have a nice factor of 7 for num of bytes
  
  encoded_string[24] = (ascii_mask   >> 14) & 0x7F;
  encoded_string[25] = (ascii_mask   >>  7) & 0x7F;
  encoded_string[26] = (ascii_mask        ) & 0x7F;
  
  //Calculate coding mask, check to see if any chars contain
  //special control characters
  //LF - 0A
  //CR - 0D
  //"  - 22 
  //+  - 2B
  //nul- 00
  //sp - 20
  
  //Note 0x7f can never be encoded out if 0x00 is encoded out.

  uint32_t coding_mask = 0;
  //Offset to 3 as we have knowinly set first 3 to SPS
  for (x = 3; x <= 26; x++)
  {
    if (encoded_string[x] == 0x0a ||
      encoded_string[x] == 0x0d ||
      encoded_string[x] == 0x22 ||
      encoded_string[x] == 0x2b ||
      encoded_string[x] == 0x00 ||
      encoded_string[x] == 0x20 ) 
    {
      coding_mask = (coding_mask << 1) | 1;
      encoded_string[x] = encoded_string[x] + 1;
 
    } else {
      coding_mask = (coding_mask << 1) | 0;        
    }
    //printf("%x\n",coding_mask);
  }
  //Shift to leave 4 bits to encode out control chars from coding mask .
  coding_mask = coding_mask << 4;
  uint8_t byte_temp = 0;
  uint32_t bit_mask;
  //Now check the coding mask for any control chars. Treat as 7 bit word
  for (x = 3; x >= 0; x--)
  {
     bit_mask = 0x7F << x*7;
     byte_temp = ((coding_mask >> x*7) & 0x7F);
     if (byte_temp == 0x0a ||
         byte_temp == 0x0d ||
       byte_temp == 0x22 ||
           byte_temp == 0x2b ||
           byte_temp == 0x00 ||
           byte_temp == 0x20 ) 
    {
      byte_temp = byte_temp + 1;
      coding_mask = (coding_mask & ~bit_mask) | (byte_temp << x*7);
      coding_mask = coding_mask  | (1 << x);
    }   
  }
 
  //Encode as 7 bits x4 (28 bytes being masked; neat factor, no wastage, except for the 4 bits which we can't use)
  encoded_string[27] = (coding_mask   >> 21) & 0x7F;
  encoded_string[28] = (coding_mask   >> 14) & 0x7F;
  encoded_string[29] = (coding_mask   >> 7 ) & 0x7F;
  encoded_string[30] = (coding_mask        ) & 0x7F;
  
  return encoded_string;
};

