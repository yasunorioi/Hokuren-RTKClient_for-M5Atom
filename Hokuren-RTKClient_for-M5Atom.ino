#include <M5Atom.h>
#include <WiFi.h>
#include <WiFiMulti.h>
//#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
//SFE_UBLOX_GNSS myGNSS;

#include "Config.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const uint16_t hokuren_port = HOKUREN_PORT;
String hokuren_userid = HOKUREN_USERID;
String  hokuren_password = HOKUREN_PASSWORD;
const char* hokuren_server = HOKUREN_SERVER;
const uint16_t hokuren_loginport = HOKUREN_LOGINPORT;
String hokuren_version = HOKUREN_VERSION;
int loop_num =1460;

WiFiMulti WiFiMulti;
WiFiClient client;

int count=0;
int login=0;
uint8_t recv_index = 0x12;
void setup() {
    M5.begin(true, false, true);  
    Serial2.begin(115200, SERIAL_8N1, 22, 19);
    pinMode(0,OUTPUT);
    digitalWrite(0,LOW);
    
    WiFiMulti.addAP( ssid, password);  
    Serial.print( "\nWaiting connect to WiFi...");  
    M5.dis.fillpix(0xff0000);  
    while (WiFiMulti.run() !=  WL_CONNECTED) {  
        Serial.print(".");
        delay(300);
    }
    M5.dis.fillpix(0x00ff00);  
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  
    delay(500);
      
  if (login == 0){
    String login_line;
    login_line = "login,"+hokuren_userid+","+hokuren_password+","+hokuren_version;
    if(!client.connect(hokuren_server,hokuren_loginport)){
      Serial.println("Server Connect faled.");
      return;
    }
    client.print(login_line);
    String line = client.readStringUntil ('\r');
    Serial.println(line);
    client.stop();
    login++;
    String login_message = client.readStringUntil('\r');
    Serial.println(login_message);
  }
    if (!client.connect(
            hokuren_server,
            hokuren_port)) {  
        Serial.println("Data Connection failed.");
        delay(5000);
        return;
    }
    client.print(hokuren_userid+","+hokuren_port+",NORMAL\\"); 
    
    int maxloops = 0;
    while (!client.available() && maxloops < loop_num) {
        maxloops++;
        delay(1);  
    }
    
    while (client.available() > 0 ){
      String c = client.readStringUntil('\r');
      Serial2.print(c);
    }
/*    size_t RTCM_Count = 0;
    uint8_t RTCM_Data[512 *4];
    while ( client.avalable()) {
      RTCM_Data[RTCM_Count++] = client.read();
      if (RTCM_Count == sizeof(RTCM_Data)){
        RTCM_Count = 0;
        break;
      }
    }
    */
}

void loop() {

    Serial2.flush();
    //client.stop();
 
    Serial.println(count);
    count++;
    //delay(1000);
}
