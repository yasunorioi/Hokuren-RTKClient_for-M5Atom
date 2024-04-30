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
#include <Packetizer.h> // https://github.com/hideakitai/Packetizer

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
    M5.begin(true, false, true);  // Init Atom(Initialize serial port, LED).
                                  // 初始化 ATOM(初始化串口、LED)
    Serial2.begin(115200, SERIAL_8N1, 22, 19);
    WiFiMulti.addAP(
        ssid,
        password);  // Add wifi configuration information.  添加wifi配置信息
    Serial.print(
        "\nWaiting connect to WiFi...");  // Serial port output format string.
                                          // 串口输出格式化字符串
    M5.dis.fillpix(0xff0000);  // Make the LED light show red.  使led灯显示红色
    while (WiFiMulti.run() !=
           WL_CONNECTED) {  // If the connection to wifi is not established
                            // successfully.  如果没有与wifi成功建立连接
        Serial.print(".");
        delay(300);
    }
    M5.dis.fillpix(0x00ff00);  // Make the LED light green.  使led灯显示绿色
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  // The serial port outputs the IP address
                                     // of the M5Atom.  串口输出M5Atom的IP地址
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
            hokuren_port)) {  // Connect to the server. 0 is returned if the
                      // connection fails. 连接服务器,若连接失败返回0
        Serial.println("Data Connection failed.");

        delay(5000);
        return;
    }
    client.print(hokuren_userid+","+hokuren_port+",NORMAL\\"); 
    Packetizer::subscribe(client, recv_index,
        [&](const uint8_t* data, const size_t size){
            for (size_t i=0;i <size; ++i)
              Serial2.print(data[i]);
          }
        );
    Serial.println(count);
    count++;
    client.stop();
    delay(1000);
}
