/******************************************
 * MQTT server
 * 
 * 
 ********************************************/

#include <Arduino.h>
#include "Mqtt.h"
#include "global.h"


#include "mqttParam.h"
#include "Connect.h"

#include "mem.h"
#include "stationParam.h"


WiFiClient espClient;
PubSubClient client(espClient);


CONNECT mConnect;
MEM mMem;




/************************************
 * MQTT PARAMETER TO ruamjai.net
 *************************************/
//const char* mqtt_server = MQTT_SERVER;
//const char *mqtt_username = MQTT_USER;
//const char *mqtt_password = MQTT_PASS;
//const int mqtt_port = MQTT_PORT;

/*Node Param */
bool mqttStatus;
// 

  const char MQTTRecieveChannel[]= "pktsos/%s/cmd";
  char CH[30];
  const char MQTTTopic[] = "pktsos/%s/%s";
  char TOPIC[40];
  char EVENT[200];


//long lastMsg = 0;
//long lastSendDataToServer;  //last send data to server time;
//int sendDataDuration = 5; //duration to send Data to server(minute)


//JSON
StaticJsonDocument<400> JSONencoder;
char JSONmessageBuffer[400];  
uint8_t jsonData_len = 0;
char buff[400];


//Inner Function
void clearJson();

void callback(char* topic,byte* payload,unsigned int length);
void callbackCmd(String msgId,bool res);
bool reconnect();
void sendLog(String msg,uint8_t state);



/********************************************
 * MQTT CMD list
 ************************************************/
enum CMD
{
  REBOOT,
  SETTIME,
  READDATA,
  SETPARAM
};
//void logMsg(String msg,uint8_t state);

MQTT::MQTT(){

}
void MQTT::init(int buad){
    Serial.begin(buad);
    Serial.println(MQTT_INIT);
    client.setServer(MQTT_SERVER,MQTT_PORT);
    client.setCallback(callback);
    sprintf(CH, MQTTRecieveChannel, cmdChannel);
    Serial.println(CH);
    
    
    client.subscribe(CH);
    //client.connect(nodeId, mqtt_username, mqtt_password);
    mqttStatus = client.connected();
    Serial.printf("MQTT STATUS %s", mqttStatus ? "true" : "false");
    regisNode();
    //sendDataToServer();
}


void MQTT::sendLogMsg(String msg){
    clearJson();
    Serial.println("send log to server");
    JSONencoder["event"] = msg;
    serializeJsonPretty(JSONencoder,JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    sprintf(TOPIC,MQTTTopic,nodeId,"log");
   
    if(!client.connected()){
      mqttStatus = reconnect();
    }
    if (mqttStatus){
       client.publish(TOPIC,JSONmessageBuffer);
    }
}




/********************************************************
 * Command to control system  pass MQTT Server
 * 
 * 
 * ******************************************************/
void callback(char* topic,byte* payload,unsigned int length){
  String msg;
  for (int i = 0; i < length; i++)
  {
    msg += (char)payload[i];
  }
  Serial.printf("Message arrive [ %s ] \n",topic);
  Serial.println(msg);
  DynamicJsonDocument doc(1024);
  auto error = deserializeJson(doc,msg);
  if (error){
    Serial.print(F("deserializeJson () FAIL with code"));
    Serial.println(error.c_str());
    return;
  }
  JsonObject obj = doc.as<JsonObject>();
  int cmd = obj[String("cmd")];
  Serial.printf("cmd : %d \n",cmd);
  String msgId = obj[String("msgId")];

  //ยินยันว่าได้รับ MSG
  callbackCmd(msgId,true);
  switch (cmd)
  {
  case ON:// Turn ON Pump
    {// Change Pump State to on
      Serial.println("Pump Run State ON");
      pumpRunState = true;
      sendLog("Pump enable",LOG_MSG_PUMP_ENABLE);   
      
    } break;
  case OFF : {
      Serial.println("Pump Run State OFF");
      pumpRunState = false;
      sendLog("Pump disable",LOG_MSG_PUMP_DISABLE);   

  }break;
  case PUMPRUNMANUAL:{
    int valve = obj[String("valveId")];
    int duration = obj[String("duration")];
    Serial.printf("Valve %d duration %d min \n",valve,duration);
    mValve.manualValveRun(valve,duration);
  }break;

  case STOPPUMPRUN:{
    Serial.println("CMD TO STOP PUMP");
    /*mPump.turnOffPump();
    delay(2000);
    mValve.valveOFF(valveOnId);*/
    nextState = millis();
  }break;
  case RESET:{
      Serial.println("Reset Device");
      mTFT.restartSystem();
  }break;
  case UPDATE :{ //Update Firmware
    Serial.println("Update Firmware");
    int fwVersion = obj[String("version")].as<int>();
    Serial.printf("Version :%d \n",fwVersion);
    if (fwVersion == 0){ //change to AutoUpdate
     
      
      autoUpdate = 1;
      mConnect.checkForUpdate(0);
    }else{ // custom fw version and set autoupdate to false
      autoUpdate = 0;
      mConnect.checkForUpdate(fwVersion);
    }
    //mMem.setAutoupdateFirmware(autoUpdate);
    mMem.writeBoolEEPROM(ADD_AUTOUPDATE,autoUpdate);
  }break;
 
  default:
    break;
  }
  
}
/************************
 * LOG Msg ไว้ ส่ง log ต่างๆ ที ระบบทำงาน
 *************************/
void MQTT::regisNode(){
    clearJson();
    wifiInfo info = mConnect.getWifiInfo();
    const size_t CAPACITY = JSON_ARRAY_SIZE(24);
    StaticJsonDocument<CAPACITY> docArr;
    JsonArray array = docArr.to<JsonArray>();

    for (size_t i = 0; i < numValve; i++)
      {
          array.add(valveState[i]);
      }
    
    Serial.printf("ssid :%s \n",info.ssid.c_str());
    Serial.printf("IP: %s",info.ipAddress.toString().c_str());
    JSONencoder["ssid"] =info.ssid.c_str();
    JSONencoder["ip"] = info.ipAddress.toString().c_str();
    JSONencoder["cmd"] = "connection";
    JSONencoder["valve"] =array;
    serializeJsonPretty(JSONencoder,JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    sprintf(TOPIC,MQTTTopic,deviceID,"connect");
    Serial.println(TOPIC);
    if(!client.connected()){
      mqttStatus =  reconnect();
    }
    if(mqttStatus){
      client.publish(TOPIC,JSONmessageBuffer);
    }
    

}



bool reconnect() {
  Serial.println("reconnect");
   // Loop until we’re reconnected
   if (!wifiState){
      bool res = mConnect.connectWifi();
      if (!res){
        Serial.printf("WIFI state NOT CONNECTED \n");
        return false;
      } 
   } 
   client.connect(nodeId,MQTT_USER, MQTT_PASSWORD);
   Serial.println(CH);
   client.subscribe(CH);
   delay(200);
  
   return client.connected();
}



//Callback MSG
void callbackCmd(String msgId,bool res){
  clearJson();
  Serial.println("Send callback to server");
   
    JSONencoder["msgId"] = msgId;
    JSONencoder["result"] = res;
    
    serializeJsonPretty(JSONencoder,JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    sprintf(TOPIC,"SF/%s/callback",nodeId);
    Serial.println(TOPIC);
    if(!client.connected()){
      mqttStatus =  reconnect();
        
    }
    if (mqttStatus){
      client.publish(TOPIC,JSONmessageBuffer);
    }else{
      Serial.println("Send MQTT ERROR");
    }
    
}





/**********************************
 * JSON Method
 ***********************************/

void clearJson(){
    JSONencoder.clear();
    memset(JSONmessageBuffer, 0 , strlen(JSONmessageBuffer));
}


void MQTT::run(){
  if(!client.connected()){
    mqttStatus =  reconnect();
  }
 
  client.loop();
}


/*****************************************
 *  Publish Station Data to Server
 ***************************************/

void MQTT::sendDataToServer(){
    clearJson();
    JSONencoder["RSSI"] = WiFi.RSSI();
    JSONencoder["temp"] = temp_arr[arr_size-1];//random(25, 39);
    JSONencoder["humi"] = humi_arr[arr_size-1] ;//random(20,100);
    /*if(devicesFound >0){
      ina226data dt =readINA226_2();
      JSONencoder["vBus"] = dt.vBus;
    }else{
      JSONencoder["vBus"] = 0;
    }*/
    JSONencoder["vBus"] = vol_arr[arr_size-1]; //24.56;
    
    
    //Todo:
        //ส่งค่า Vbat
        //temp humidity มาด้วย
    serializeJsonPretty(JSONencoder,JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);
    sprintf(TOPIC,"SF/%s/data",nodeId);
    Serial.println(TOPIC);
    if(!client.connected()){
       reconnect();
    }
    Serial.printf("MQTT STATUS %s \n", mqttStatus ? "true" : "false");
    if (mqttStatus){
      client.publish(TOPIC,JSONmessageBuffer);
      Serial.println("Success");
    }
    
   
    lastSendDataToServer = millis();
}

//Return Valve State
JsonArray getJSONValveState(){
    const size_t CAPACITY = JSON_ARRAY_SIZE(24);
    StaticJsonDocument<CAPACITY> docArr;
    JsonArray array = docArr.to<JsonArray>();

    for (size_t i = 0; i < numValve; i++)
      {
          array.add(valveState[i]);
      }
    
      return array;
}


      
    