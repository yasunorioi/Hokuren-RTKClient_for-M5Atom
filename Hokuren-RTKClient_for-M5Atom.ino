/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with Atom-Lite/Matrix sample source code
*                          配套  Atom-Lite/Matrix 示例源代码
* Visit for more information: https://docs.m5stack.com/en/products
* 获取更多资料请访问：https://docs.m5stack.com/zh_CN/products
*
* Describe: WIFI TCP.
* Date: 2021/7/29
*******************************************************************************
  M5Atom will sends a message to a TCP server
  M5Atom 将向TCP服务器发送一条数据
*/

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
// Set the name and password of the wifi to be connected.
// 配置所连接wifi的名称和密码
/* const char *ssid     = "";
const char *password = "chaylan22";
*/
WiFiMulti WiFiMulti;


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
}

void loop() {
  WiFiClient client;
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
    
    int maxloops = 1600;
    while (!client.available() && maxloops < 1700) {
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
    Serial2.flush();
    client.stop();
 
    Serial.println(count);
    count++;
    //delay(1000);
}
