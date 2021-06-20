#include <Arduino.h>
#include <mem.h>
#include <EEPROM_writeall.h>


#include <Adafruit_Sensor.h>
#include "Adafruit_AM2320.h"

#include <Ultrasonic.h>

#include <ArduinoJson.h>


//MQTT
#include "mqttParam.h"
#include "global.h"
#include "stationParam.h"
#include <calculator.h>


//#include "Mqtt.h"


//#include "Connect.h"

 
#define SECONDS_DS(seconds) ((seconds)*1000UL)


Ultrasonic ultrasonic(12, 14);
Adafruit_AM2320 am2320 = Adafruit_AM2320();
//ESP8266WiFiMulti WiFiMulti;
//rst_info *rsti;


Calculator calculator;
MEM Mem;

//CONNECT Connect;
//MQTT Mqtt;


bool autoUpdate; 



//Rain Gauge
const int REED_PIN = 13; //  D7 Pin connected to reed switch rain guage
int rainCount = 0;

unsigned long startTime;
#define SECONDS_DS(seconds) ((seconds)*1000UL)



///int buad = 115200; 
unsigned long  previousMillis = 0;

//ultrasonic
int distance,beforeLevel ;



//JSON
/*StaticJsonDocument<400> JSONencoder;
char JSONmessageBuffer[400];
uint8_t jsonData_len = 0;
char buff[400];*/

//DynamicJsonDocument doc(1024); //Object for convert json txt


void setupMQTT();
void espRestart();
void rainCount_f();
void printParameter();
int sort_desc(const void *cmp1, const void *cmp2);
//void connectWiFi();

int buad =9600;

void setup()
{
  Serial.begin(buad);
  
  Serial.printf("Device Name : %s \n", deviceName);
  delay(1000);
  Serial.printf("Device ID : %s \n", deviceID);
  delay(1000);
  
  Serial.printf("Software : %s \n", APP_NAME);
  delay(1000);
  Serial.printf("version : %s \n", APP_VERSION);
  delay(1000);
  printParameter();
  Mem.init(buad,EEPROM_SIZE);
  //Mem.clearEEPROM();
  EEPROM_writeAnything(ADS_INTERVAL,interval);
  delay(2000);
  int duration;
  EEPROM_readAnything(ADS_INTERVAL,duration);
 // Serial.println(interval);
 delay(2000);
  Serial.printf("DEVICE ID = %d \n",duration);
  delay(1000);
  

  //Mem.writeDefaultValue();
  //Mem.readDefaultValue();
 
  
  //Connect.init(buad);

  /*if(wifiState){
    Mqtt.init();
  }*/


  am2320.begin();
  pinMode(REED_PIN, INPUT_PULLUP);
  //attachInterrupt(REED_PIN, rainCount_f,FALLING);
  previousMillis = millis();

}

void loop()
{
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > SECONDS_DS(interval)){
      Serial.println("READ DATA");
      previousMillis = millis();
  }
    

   // sendDataToServer();
}

void rainCount_f()
{
  rainCount = rainCount + 1;
  Serial.print("raincount");
  Serial.println(rainCount);
}
void espRestart()
{
  ESP.restart();
}


void printParameter()
{
  Serial.printf("Station height : %d cm \n", stationHeight);
  Serial.printf("Station interval : %ld sec \n", interval);
  Serial.printf("lack Water level : %d cm \n", lackLevel);
  Serial.printf("normal water level : %d cm \n", normalLevel);
  Serial.printf("warning water level : %d cm \n", warnLevel);
  Serial.printf("danger water level : %d cm \n", dangerLevel);
  Serial.printf("Rain factor : %d  \n", rainFactor);
}


int sort_desc(const void *cmp1, const void *cmp2)
{
  // Need to cast the void * to int *
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  // The comparison
  return a > b ? -1 : (a < b ? 1 : 0);
  // A simpler, probably faster way:
  //return b - a;
}
