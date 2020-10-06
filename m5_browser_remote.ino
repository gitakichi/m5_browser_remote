#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include <M5StickC.h>
#include <Wire.h>
#include "secret.h"
#include "index.h"

#define DRV8830_A 0x60//右モーター
#define DRV8830_B 0x64//左モーター
#define CONTROL   0x00
#define FAULT     0x01

#define DIR_N     0x00
//#define DIR_CCW   0x01//タミヤ　ダブルギヤボックス、Bタイプ
//#define DIR_CW    0x02//タミヤ　ダブルギヤボックス、Bタイプ
#define DIR_CW    0x01//タミヤ　ダブルギヤボックス、Cタイプ
#define DIR_CCW   0x02//タミヤ　ダブルギヤボックス、Cタイプ
#define DIR_B     0x03

//Wifi認証情報はsecret.hに記載
//const char* ssid = "........";
//const char* password = "........";

#define BTN_A_PIN 37
#define LED_PIN   10//このLEDは電位を下げることで発光する
#define LED_ON    LOW
#define LED_OFF   HIGH
#define BTN_ON    LOW
#define BTN_OFF   HIGH

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
//環境設定->コンパイラの警告->初期値

void handleRoot(void);
void handleRC(void);
void handleNotFound(void);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void drv8830_setup(void);
void drv8830_func(char device,char dir);
void drv8830_Q(void);
void drv8830_W(void);
void drv8830_E(void);
void drv8830_A(void);
void drv8830_S(void);
void drv8830_D(void);
void drv8830_neutral(void);


uint8_t prev_btn_a = BTN_OFF;
uint8_t btn_a      = BTN_OFF;

#define SPEED_1 0x0D
#define SPEED_2 0x15
#define SPEED_3 0x1D
#define SPEED_4 0x25

char m_speed = SPEED_2;//default

void setup(void) {
  pinMode(LED_PIN,OUTPUT);
  pinMode(BTN_A_PIN,INPUT_PULLUP);
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
  server.on("/remote", handle_remote);
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

  btn_a = digitalRead(BTN_A_PIN);
  if(prev_btn_a == BTN_OFF && btn_a == BTN_ON){
    drv8830_neutral();
    digitalWrite(LED_PIN,LED_ON);
    delay(100);
    digitalWrite(LED_PIN,LED_OFF);
  }
  prev_btn_a = btn_a;
}

void handleRoot(void) {
  digitalWrite(LED_PIN,LED_ON);
  server.send(200, "text/plain", "hello from M5StickC!");
  digitalWrite(LED_PIN,LED_OFF);
}

void handle_remote(void) {
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
       drv8830_neutral();
       break;
    }
    case WStype_CONNECTED:{
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      //send message to client
      //webSocket.sendTXT(num, "Connected");
      break;
    }
    case WStype_TEXT:{//ここでデバッグする
      Serial.printf("[%u] get Text: %s\n", num, payload);//debug
      if(payload[0] == 'q')       drv8830_Q();
      else if(payload[0] == 'w')  drv8830_W();
      else if(payload[0] == 'e')  drv8830_E();
      else if(payload[0] == 'a')  drv8830_A();
      else if(payload[0] == 's')  drv8830_S();
      else if(payload[0] == 'd')  drv8830_D();
      else if(payload[0] == 'n')  drv8830_neutral();
      else if(payload[0] == '1')  m_speed = SPEED_1;
      else if(payload[0] == '2')  m_speed = SPEED_2;
      else if(payload[0] == '3')  m_speed = SPEED_3;
      else if(payload[0] == '3')  m_speed = SPEED_4;
      break;
    }
  }
}

void drv8830_setup(void){
  //I2C初期化
  Wire.begin(0, 26);
  drv8830_neutral();

  //DRV8830 FAULTクリア
  Wire.beginTransmission(DRV8830_A);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(FAULT);//フォルト(障害状態)
  Wire.write(0x80);//フォルトクリア
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了

  Wire.beginTransmission(DRV8830_B);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(FAULT);//フォルト(障害状態)
  Wire.write(0x80);//フォルトクリア
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了

  delay(1000);//1000msec待機(1秒待機);
}

void drv8830_func(char device,char dir){
  Wire.beginTransmission(device);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CONTROL);//コントロール
  if(dir == DIR_N)  Wire.write(0x00);
  else              Wire.write(m_speed << 2 | dir);
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了
}

void drv8830_Q(void){
  drv8830_func(DRV8830_A,DIR_CW);
  drv8830_func(DRV8830_B,DIR_N);
}

void drv8830_W(void){
  drv8830_func(DRV8830_A,DIR_CW);
  drv8830_func(DRV8830_B,DIR_CCW);
}

void drv8830_E(void){
  drv8830_func(DRV8830_A,DIR_N);
  drv8830_func(DRV8830_B,DIR_CCW);
}

void drv8830_A(void){
  drv8830_func(DRV8830_A,DIR_CCW);
  drv8830_func(DRV8830_B,DIR_N);
}

void drv8830_S(void){
  drv8830_func(DRV8830_A,DIR_CCW);
  drv8830_func(DRV8830_B,DIR_CW);
}

void drv8830_D(void){
  drv8830_func(DRV8830_A,DIR_N);
  drv8830_func(DRV8830_B,DIR_CW);
}

void drv8830_neutral(void){
  drv8830_func(DRV8830_A,DIR_N);
  drv8830_func(DRV8830_B,DIR_N);
}
