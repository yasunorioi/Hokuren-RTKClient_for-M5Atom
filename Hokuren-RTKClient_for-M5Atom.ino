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
String hokuren_password = HOKUREN_PASSWORD;
const char* hokuren_server = HOKUREN_SERVER;
const uint16_t hokuren_loginport = HOKUREN_LOGINPORT;
String hokuren_version = HOKUREN_VERSION;

WiFiMulti WiFiMulti;
WiFiClient client;

bool connected = false;

// ログイン(ポート7001)→データポート接続の一連の手順
bool connectToRTK() {
    // Step 1: ログインポートに接続してログイン
    WiFiClient loginClient;
    Serial.println("[RTK] Connecting to login port " + String(hokuren_loginport) + "...");
    if (!loginClient.connect(hokuren_server, hokuren_loginport)) {
        Serial.println("[RTK] Login connection failed.");
        return false;
    }
    String login_line = "login," + hokuren_userid + "," + hokuren_password + "," + hokuren_version;
    loginClient.print(login_line);
    String response = loginClient.readStringUntil('\r');
    Serial.println("[RTK] Login response: " + response);
    loginClient.stop();
    delay(200);

    // Step 2: データポートに接続
    Serial.println("[RTK] Connecting to data port " + String(hokuren_port) + "...");
    if (!client.connect(hokuren_server, hokuren_port)) {
        Serial.println("[RTK] Data connection failed.");
        return false;
    }
    client.print(hokuren_userid + "," + hokuren_port + ",NORMAL\\");
    Serial.println("[RTK] Data connection established.");
    return true;
}

// WiFi切断時に再接続（ブロッキング）
void ensureWiFi() {
    if (WiFiMulti.run() == WL_CONNECTED) return;
    Serial.println("[WiFi] Connection lost. Reconnecting...");
    M5.dis.fillpix(0xff0000);  // 赤: 未接続
    connected = false;
    client.stop();
    while (WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\n[WiFi] Reconnected. IP: " + WiFi.localIP().toString());
}

void setup() {
    M5.begin(true, false, true);
    Serial2.begin(115200, SERIAL_8N1, 22, 19);
    pinMode(0, OUTPUT);
    digitalWrite(0, LOW);

    M5.dis.fillpix(0xff0000);  // 赤: 未接続

    WiFiMulti.addAP(ssid, password);
    Serial.print("\n[WiFi] Connecting...");
    while (WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println("\n[WiFi] Connected. IP: " + WiFi.localIP().toString());

    M5.dis.fillpix(0xffff00);  // 黄: RTK接続中
    delay(500);

    if (connectToRTK()) {
        connected = true;
        M5.dis.fillpix(0x00ff00);  // 緑: 接続完了
    } else {
        M5.dis.fillpix(0xff0000);  // 赤: 接続失敗（loop()でリトライ）
    }
}

void loop() {
    M5.update();

    // 1. WiFi接続を確保
    ensureWiFi();

    // 2. RTKデータポート切断時はログインから再接続
    if (!client.connected()) {
        connected = false;
        M5.dis.fillpix(0xffff00);  // 黄: 再接続中
        client.stop();
        Serial.println("[RTK] Disconnected. Retrying...");
        delay(2000);
        if (connectToRTK()) {
            connected = true;
            M5.dis.fillpix(0x00ff00);  // 緑: 再接続完了
        } else {
            M5.dis.fillpix(0xff0000);  // 赤: 接続失敗
            delay(3000);
            return;
        }
    }

    // 3. 受信データをSerial2に転送（持続受信ループ）
    if (client.available() > 0) {
        M5.dis.fillpix(0x0000ff);  // 青: データ受信中
        while (client.available() > 0) {
            Serial2.write(client.read());
        }
        M5.dis.fillpix(0x00ff00);  // 緑: 受信完了、接続維持中
    }

    delay(10);
}
