


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>

#include <Connect.h>
#include "global.h"

#include "mem.h"
#include "Mqtt.h"

#include "text.h"




WiFiMulti wifiMulti;
WebServer server(80);
WiFiClient espClient;
MEM cMem;
MQTT cMqtt;

String ipAddress; //keepp ssid & ip address is connect
char* ssid;
char* pass;

bool wifiState = false;

/*********************
 * Update Server
 *********************/
//const char* fwUrlBase = "http://ruamjai.net/firmware/"; net project
//uint8_t autoUpdate;




/*******************************************
 * HTML CODE for webserver
 *********************************************/

const String HtmlHtml = "<html><head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" charset=\"UTF-8\" /></head>";

const String HtmlHtmlClose = "</html>";
const String HtmlTitle = "<h1>SMART FARM Setup</h1><br/>\n";
const String HtmlTitle2 ="<p>Please select  Wifi network</p>";
const String HtmlCenter = "<center>";
const String HtmlCloseCenter = "</center>";
const String HtmlLedState1 = "<h2>Light 1 </h2><br/>\n";
const String HtmlLedState2 = "<h2>Light 2 </h2><br/>\n";
const String FormHeader  = "<form method=\"POST\" action=\"testConnect\">";
const String btnConnect = "<button type=\"submit\">Submit</button>";
const String ipWifiPass = "<label>Password</label>:<input type=\"text\" name=\"password\" />";

const String FormFooter  = "</form>";
const String ScanWifi = "<a href=\"/ScanWifi\"><button>Scan Wifi</button></a>";
const String btnBack = "<a href=\"/\"><button>Back</button></a>";



void response();
void handleRoot();
void handleTestConnect();
void handleWifiScan();
String IpAddress2String(const IPAddress& ipAddress);

CONNECT::CONNECT(){
    
}

void CONNECT::init(int buadrate){
    
   /* ssid = cMem.readEEPROM(ADD_SSID).data;//
    Serial.println(ssid);
    pass = cMem.readEEPROM(ADD_PASS).data;//
    Serial.println(pass);
    wifiState = connectWifi();*/
      
    
}

bool CONNECT::connectWifi(){
    
    
   /* uint8_t loop = 0;
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid,pass);
    Serial.print(WIFI_INITIALIZE);
   
    String pcs = "wait for";
    while (WiFi.status()!= WL_CONNECTED && loop <30){
        
        Serial.print(".");
        pcs = pcs+'.';
        delay(1000);
        loop++;
    }
    Serial.println("TEST/n");
    
    if (WiFi.status()== WL_CONNECTED){
        Serial.printf("Connected to : %s @ %s",WiFi.SSID(),WiFi.localIP());
        delay(1000);
        wifiState = true;
        return true;
    }else{
        Serial.println(WIFI_CONNECT_FAIL);
        delay(1000);
        wifiState = false;
        WiFi.disconnect();
        return false;
    }*/

}
/***************************
 * For configuration WIFI network
 * @ip 192.168.4.1
 * ***************************/


void CONNECT::startConfigServer(){
   
    //WiFi.mode(WIFI_AP_STA); //create station
    WiFi.mode(WIFI_AP_STA);
    const char *ssid = DEFAULT_SSID;
    WiFi.softAP(ssid);
    Serial.print("IP :");
    Serial.println(WiFi.softAPIP());  
    //char* ss = WiFi.softAPIP();           // ip address ของหน้าเว็บ
   

    // เมื่อเข้ามาที่ลิ้ง /
    server.on("/", handleRoot);
    server.on("/ScanWifi",handleWifiScan);
    server.on("/testConnect",handleTestConnect);
    server.begin(); // เปิดการใช้งาน Web Server
    while(true){
     // _TFT.iconCmd();
      server.handleClient();
    }

    
}


 void handleRoot(){
         response();
        
    }


void response(){
  // _TFT.drawString("mobile connected",20,300,2,ML_DATUM,TFT_RED,TFT_WHITE);
  String htmlRes = HtmlHtml + HtmlCenter;
  htmlRes += HtmlTitle;
  htmlRes += HtmlTitle2;
  htmlRes += ScanWifi;
  htmlRes += HtmlCloseCenter;
  htmlRes += HtmlHtmlClose;
  server.send (200, "text/html", htmlRes);
}
/************************
 *  handle get /testConnect
 ***************************/
char _ssid[30];
char _pass[30];
void handleTestConnect(){
  
  


 
  //String _s,_p;
  String _s = server.arg("ssid");
  String _p = server.arg("password");
  _s.toCharArray(_ssid,_s.length()+1);
  _p.toCharArray(_pass,_p.length()+1);

  WiFi.mode(WIFI_AP_STA); 
  uint8_t loop = 0;
  Serial.println(_ssid);
  WiFi.begin(_ssid, _pass);
  char buff[100];
  sprintf(buff,"Connect to %s pass %s",_ssid,_pass);
  Serial.println(buff);
  //_TFT.drawString(buff,20,280,3,ML_DATUM,TFT_RED,TFT_WHITE);
  String pcs = ".";
  while (WiFi.status()!= WL_CONNECTED && loop >30 )
  {
    delay(1000);
    Serial.print(".");
    pcs+='.';
    loop++;
  }
  delay(2000);
  if (WiFi.status() == WL_CONNECTED){
    
    Serial.println(_ssid);
    Serial.println("Connect Success");
    
    mem.writeEEPROM(ADD_SSID,server.arg("ssid"));
    
    mem.writeEEPROM(ADD_PASS,server.arg("password"));
    
    ssid = _ssid;//server.arg("ssid");
    pass = _pass; // server.arg("password");

    String htmlRes= HtmlHtml + HtmlCenter;
    htmlRes +=HtmlTitle;
    htmlRes += HtmlCenter;
    htmlRes += "<h1>Connect Success</h1>";
    htmlRes += "open App for Configuration your Device";
    htmlRes += HtmlCloseCenter;
    htmlRes += HtmlHtmlClose;
    server.send (200, "text/html", htmlRes);
  
    
    
  }else{
    Serial.println("Connect Fail");
  
    String htmlRes= HtmlHtml + HtmlCenter;
    htmlRes += "<h1>Connect Fail Scan Wifi Again</h1>";
    htmlRes += btnBack;
    htmlRes += HtmlCloseCenter;
    htmlRes += HtmlHtmlClose;
    server.send (200, "text/html", htmlRes);
    
    
  }
}
/************************
 *  handle get /ScanWifi
 ***************************/

void handleWifiScan(){
  Serial.println("Scan Network");
  WiFi.mode(WIFI_AP_STA);
  int n = WiFi.scanNetworks();
  
  Serial.printf("Scan Done found %d network \n",n);
  
  char buff[100];
  String htmlRes;
  
  if  (n<=0){
    Serial.println(" No Wifi Network Found");
  //  _TFT.drawString("No network found",20,280,3,ML_DATUM,TFT_RED,TFT_WHITE);
    htmlRes = HtmlHtml + HtmlCenter;
    htmlRes +=HtmlTitle;
    htmlRes += "<p>No network found</p>";
    htmlRes +=ScanWifi;
    htmlRes += HtmlCloseCenter;
    htmlRes += HtmlHtmlClose;
    

  } 
  else{
    sprintf(buff," Found %d wifi service",n);
   // _TFT.drawString(buff,20,280,3,ML_DATUM,TFT_BLUE,TFT_WHITE);
    Serial.println("Network Found");
    htmlRes = HtmlHtml + HtmlCenter;
    htmlRes +=HtmlTitle;
    htmlRes += "<p>Found "+ String(n) + "  wifi</p>";
    htmlRes += FormHeader;
    if (n >10 ) n =10;
    for (size_t i = 0; i < n; i++)
    {   
      htmlRes += "<input type=\"radio\" id=\""+WiFi.SSID(i)+"\" name=\"ssid\" value=\""+WiFi.SSID(i)+"\"/><label>"+WiFi.SSID(i)+"("+ WiFi.RSSI(i)+" db)"+"</label><br>" ;
    }
    htmlRes +=  "<br>"+ipWifiPass+"<br>";
    htmlRes +="<br>" + btnConnect;
    //htmlRes += ScanWifi;
    htmlRes += FormFooter;
    htmlRes += HtmlCloseCenter;
    htmlRes += HtmlHtmlClose;
  
  }
  /*htmlRes = HtmlHtml + HtmlCenter;
  htmlRes +=HtmlTitle;
  htmlRes = n + "network Found";*/
  server.send (200, "text/html", htmlRes);

}


wifiInfo CONNECT::getWifiInfo(){
  wifiInfo info;
  info.ssid = WiFi.SSID();
  info.ipAddress =WiFi.localIP();
  info.rssi = WiFi.RSSI();
  Serial.println(info.ipAddress.toString().c_str());
  //String d = mem.readEEPROM(ADD_SSID).data;
  Serial.print("GET WIFI INFO SSID:");
  Serial.println(info.ssid);

  return info;
}


String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}



/****************************************
 * Update Firmware  
 * 
 ******************************************/

bool CONNECT::firmwareUpdate(String url){
    if (!wifiState) connectWifi();  
    Serial.print("Start update firmware form :");
    Serial.println(url);
    t_httpUpdate_return ret = ESPhttpUpdate.update(url);
    switch (ret)
    {
    case HTTP_UPDATE_FAILED:{
      char buff[300];
          sprintf(buff,"HTTP_UPDATE_FAILED ERROR (%d) : %s ",ESPhttpUpdate.getLastError(),ESPhttpUpdate.getLastErrorString().c_str());
          Serial.println(buff);
          cMqtt.logMsg(buff,LOG_MSG_STATE_NONE);
          delay(2000);
          return false;
        }break;
      case HTTP_UPDATE_NO_UPDATES : {
          Serial.println("NO Update found"); 
          delay(500);
          return true;
        }break;

      case HTTP_UPDATE_OK:{
            Serial.println("Update Success");
            cMqtt.logMsg("UPDATE SUCCESS",LOG_MSG_STATE_NONE);
            delay(2000);
            return true;
            }break;
    default:
      break;
    }

}

bool CONNECT::checkForUpdate(int _version){
  String fwURL = String(fwUrlBase); // ลิงค์ ของ เว็บทืาเก็บ firmware
    if (_version !=0){ //update แบบ กำหนด เวอร์ชั่น

        String  fwImageURL = fwURL+'/';
        fwImageURL.concat(_version);
        fwImageURL.concat(".bin");
        Serial.println(fwImageURL);
        bool res = firmwareUpdate(fwImageURL);
        if(res){
          cMqtt.logMsg("Update firmware to "+ _version,LOG_MSG_STATE_AUTOUPDATE_OFF);
          return true;
          //resetSystem();
        } else{
          cMqtt.logMsg("Fail to update firmware "+_version,LOG_MSG_STATE_NONE);
          return false;
        }

    }else{
    fwURL.concat(HWID); //add ประเภทของ HWID เพื่อไป get data from URL
    //String fwVersionURL = fwURL;
    //fwVersionURL.concat(".version");
    Serial.println( "Checking for firmware updates." );
    Serial.print( "Firmware version URL: " );
    Serial.println( fwURL);

    HTTPClient httpClient;
    httpClient.begin(fwURL);
    int httpCode = httpClient.GET();
    if (httpCode ==200){ //connect success
      String newFWVersion = httpClient.getString();
      Serial.printf("Current firmware version : %d\n",FW_VERSION );
      Serial.print("Available firmware version :");
      Serial.println(newFWVersion);

      int newVersion = newFWVersion.toInt();
      if (newVersion > FW_VERSION){
          Serial.println("Prepare Update");
          String  fwImageURL = fwURL+'/';
          fwImageURL.concat(newVersion);
          fwImageURL.concat(".bin");
          Serial.println(fwImageURL);
          cMqtt.logMsg("Update firmware to "+ newFWVersion,LOG_MSG_STATE_AUTOUPDATE_ON);
          firmwareUpdate(fwImageURL);
          /*if(res){
            cMqtt.logMsg("Update firmware to "+ newFWVersion,LOG_MSG_STATE_NONE);
            delay(2000);
            //resetSystem();
            return true;
          } else{
            cMqtt.logMsg("Fail to update firmware "+newFWVersion,LOG_MSG_STATE_NONE);
            return false;
          }*/
        }else{
          Serial.println("firmware is up to date");
          cMqtt.logMsg("firmware is uptodate",LOG_MSG_STATE_AUTOUPDATE_ON);
          return false;
          
        }

      }else{
        Serial.printf("error httpRequest : %d",httpCode);
      }
    
    }

}