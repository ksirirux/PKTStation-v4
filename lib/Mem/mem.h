#ifndef MEMTOOL_H
#define MEMTOOL_H


#include <Arduino.h>

struct wifiConfig{
    char ssid;
    char password;
};
 

struct eepromData
{
    char data[30];
};



class MEM
{
    
    public:
        MEM(){}
        void init(int buad,int size);
        eepromData readEEPROM(int address);
        //write String only
        void writeEEPROM(int address,String data);
        //Write Bool Only
        void writeBoolEEPROM(int address,bool b);


        bool readBooleanEEPROM(int address);

        void clearEEPROM();
        
        
        
       
        //void getDefaultValue(); //getDefault value
        void readDefaultValue();
        //void setAutoupdateFirmware(bool state);

        //void saveBatConfig();



        //For First Run Only
        void writeDefaultValue();
        
        
   

};

#endif