#include <EEPROM.h>
#include <mem.h>
#include "EEPROM_writeall.h"








void MEM::init(int buad,int size){
    Serial.begin(buad);
    EEPROM.begin(size);
}

eepromData MEM::readEEPROM(int address){
    unsigned char k;
    uint8_t len =0;
    eepromData v;
    k = EEPROM.read(address);
    while (k !='\0' && len<30)
    {
        k = EEPROM.read(address+len);
        v.data[len]= k;
        len++;
    }
        v.data[len]='\0';
        return v;
}
void MEM::writeEEPROM(int address,String data){
    int _size = data.length();
    for (size_t i = 0; i < _size; i++)  {
        EEPROM.write(i+address,data[i]);
    }
    EEPROM.write(address+_size,'\0');
    EEPROM.commit();
}
void MEM::writeBoolEEPROM(int address,bool b){
    EEPROM.write(address,b);
    EEPROM.commit();
}

bool MEM::readBooleanEEPROM(int address){
    return EEPROM.read(address);
    
}

void MEM :: clearEEPROM(){
    for (int i =0; i< EEPROM.length();i++){
        EEPROM.write(i,0); //Clear EEPROM
    }
}

/*void MEM :: writeDefaultValue(){
    //use at first start
    for (int i =0; i< EEPROM.length();i++){
        EEPROM.write(i,0); //Clear EEPROM
    }

    EEPROM_writeAnything(ADS_DEVICEID,deviceID);
    EEPROM_writeAnything(ADS_INTERVAL,interval);
    EEPROM_writeAnything(ADS_STATIONHEIGHT,stationHeight);
    EEPROM_writeAnything(ADS_LACKLEVEL,lackLevel);


}
void readDefaultValue(){
    String id ;
    EEPROM_readAnything(ADS_DEVICEID,id);
    Serial.printf('ID = %s\n',id);

}*/
/*eepromData MEM::readEEPROM(int address){
    unsigned char k;
    uint8_t len =0;
    eepromData v;
    k = EEPROM.read(address);
    while (k !='\0' && len<30)
    {
        k = EEPROM.read(address+len);
        v.data[len]= k;
        len++;
    }
        v.data[len]='\0';
        return v;

}
*/


/*void MEM::setAutoupdateFirmware(bool state){
    writeBoolEEPROM(ADD_AUTOUPDATE,state);
}*/