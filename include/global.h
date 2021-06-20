

#define APP_NAME "PKTSOS-NBIOT"
#define APP_VERSION "v 1.01.01"
#define FW_VERSION 2021010101 //year|version xx.xx.xx
#define HWID "AISNB-01"




/*****************************
 * EEPROM Parametre
 *  ADS =>Address
 * ****************************/
#define EEPROM_SIZE 512
#define ADS_DEVICEID 0
#define ADS_INTERVAL 60
#define ADS_STATIONHEIGHT 65
#define ADS_LACKLEVEL 68



// ADDRESS 10-59 WifiConnection
#define ADD_SSID 20 //keep ssid  20 char 
#define ADD_PASS 40 //keep password 20 char

#define ADD_AUTOUPDATE 340










extern String ipAddress; //keepp ssid & ip address is connect
extern char* ssid;
extern char* pass;
extern bool wifiState; //Wifi Connect State
extern char dayOfTheWeek;