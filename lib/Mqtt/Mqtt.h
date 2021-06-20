/********************************
 *  Class for MQTT Server
 * 
 * 
 *********************************/

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>



class MQTT
{
private:
    /* data */
public:
    MQTT(/* args */);
    void init(int buad);
    void run();
    void regisNode();

    //void Publish(char* topic,char msg);

    void sendLogMsg(String msg);
    //void callbackCmd(String msgId,bool res);


    //Send data to server
    void sendDataToServer();
    
};


