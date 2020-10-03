#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include <M5StickC.h>
#include <Wire.h>
#include "secret.h"
#include "index.h"

#define DRV8830_A 0x60
#define DRV8830_B 0x64
#define CONTROL 0x00
#define FAULT 0x01

//Wifi認証情報はsecret.hに記載
//const char* ssid = "........";
//const char* password = "........";

#define LED_PIN 10//このLEDは電位を下げることで発光する
#define LED_ON  LOW
#define LED_OFF HIGH

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
//環境設定->コンパイラの警告->初期値

void handleRoot(void);
void handleRC(void);
void handleNotFound(void);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void drv8830_setup(void);
void drv8830_fwd(void);
void drv8830_rv(void);
void drv8830_neutral(void);

void setup(void) {
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LED_OFF);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  M5.begin();
  M5.Lcd.setRotation(3);// 0-3で画面の向き
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println(WiFi.localIP());
  M5.Lcd.println("http://esp32.local/");
  
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);
  server.on("/remote", handleRC);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  webSocket.begin();//WebSocketサーバー開始
  webSocket.onEvent(webSocketEvent);

  drv8830_setup();
}

void loop(void) {
  webSocket.loop();
  server.handleClient();
}

void handleRoot(void) {
  digitalWrite(LED_PIN,LED_ON);
  server.send(200, "text/plain", "hello from M5StickC!");
  digitalWrite(LED_PIN,LED_OFF);
}

void handleRC(void) {
  digitalWrite(LED_PIN,LED_ON);
  server.send(200, "text/HTML", index_str);
  digitalWrite(LED_PIN,LED_OFF);
}

void handleNotFound(void) {
  digitalWrite(LED_PIN,LED_ON);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED_PIN,LED_OFF);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:{
       Serial.printf("[%u] Disconnected!\n", num);
       break;
    }
    case WStype_CONNECTED:{
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      // send message to client
      webSocket.sendTXT(num, "Connected");
      break;
    }
    case WStype_TEXT:{//ここでデバッグする
      Serial.printf("[%u] get Text: %s\n", num, payload);
      // send message to client
      // webSocket.sendTXT(num, "message here");
      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
    }
  }
}

void drv8830_setup(void){
  //I2C初期化
  Wire.begin(0, 26);

  //DRV8830 CONTROLクリア
  Wire.beginTransmission(DRV8830_A);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CONTROL);//コントロール
  Wire.write(0x00);//コントロールクリア
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了

  //DRV8830 FAULTクリア
  Wire.beginTransmission(DRV8830_A);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(FAULT);//フォルト(障害状態)
  Wire.write(0x80);//フォルトクリア
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了

  //DRV8830 CONTROLクリア
  Wire.beginTransmission(DRV8830_B);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CONTROL);//コントロール
  Wire.write(0x00);//コントロールクリア
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了

  //DRV8830 FAULTクリア
  Wire.beginTransmission(DRV8830_B);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(FAULT);//フォルト(障害状態)
  Wire.write(0x80);//フォルトクリア
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了

  delay(1000);//1000msec待機(1秒待機);
}

void drv8830_fwd(void){
  Wire.beginTransmission(DRV8830_B);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CONTROL);//コントロール
  Wire.write(0x12 << 2 | 0x01);
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了

  Wire.beginTransmission(DRV8830_A);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CONTROL);//コントロール
  Wire.write(0x12 << 2 | 0x02);
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了
}

void drv8830_rv(void){
  Wire.beginTransmission(DRV8830_B);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CONTROL);//コントロール
  Wire.write(0x12 << 2 | 0x02);
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了

  Wire.beginTransmission(DRV8830_A);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CONTROL);//コントロール
  Wire.write(0x12 << 2 | 0x01);
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了
}

void drv8830_neutral(void){
  Wire.beginTransmission(DRV8830_A);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CONTROL);//コントロール
  Wire.write(0x00);
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了

  Wire.beginTransmission(DRV8830_B);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CONTROL);//コントロール
  Wire.write(0x00);
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了
}
