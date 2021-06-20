//https://thanapon.info/arduino-platformio-lib/
#ifndef CONNECT_H
#define CONNECT_H


#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/*Update vis Server*/
//#include <ESPmDNS.h>
//#include <Update.h>

//EEPROM


/*FOR SERVER UPDATE*/
//#include <HTTPClient.h>
//#include <ESP32httpUpdate.h>

// MQTT_MAX_PACKET_SIZE : Maximum packet size //https://www.esp8266.com/viewtopic.php?f=160&t=15872

#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 200
#endif


struct wifiInfo{
    String ssid;
    IPAddress ipAddress;
    int8_t rssi;
};



class CONNECT
{

private:
    /* data */
    void response();
   
public:
    CONNECT();
    void init(int buadrate);

    bool connectWifi();
    void startConfigServer();
    wifiInfo getWifiInfo(); //return Wifi connection info
   // void handleRoot();
   bool firmwareUpdate(String url);
   bool checkForUpdate(int _version);
    
protected:

};



class UPDATE{
    private:

    public:
    UPDATE();
};



#endif

