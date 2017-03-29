//The tag is always valid SSID charset
//SPS = Version 1
//SP2 = Version 2 (not yet release).

static const unsigned char tag[] = "SPS";  
char *encoded_string = NULL;

//SPS SSID data structure 248 bits total
struct sps_data {
  uint32_t      dev_id;      //24 bit
  int32_t       lat;         //32 bit
  int32_t       lng;         //32 bit
  int32_t       altitude;    //26 bit (with extra sign bit)
  int16_t       tx_pwr;      //11 bit
  bool          off_map;     //1  bit
  bool          three_d_map; //1  bit
  uint16_t      res;         //12 bit
  uint32_t      app_id;      //24 bit
  uint8_t       path_loss;   //3  bit
};



