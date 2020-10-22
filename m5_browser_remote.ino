#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>
#include <M5StickC.h>
#include <Wire.h>
#include "index.h"

//for DRV8830
#define DRV8830_A 0x60//右モーター
#define DRV8830_B 0x64//左モーター
#define CONTROL   0x00
#define FAULT     0x01
#define DIR_N     0x00
#define DIR_CCW   0x01
#define DIR_CW    0x02
#define DIR_B     0x03
#define SPEED_0   0x06

//for M5StickC
#define BTN_A_PIN 37
#define LED_PIN   10//このLEDは電位を下げることで発光する
#define LED_ON    LOW
#define LED_OFF   HIGH
#define BTN_ON    LOW
#define BTN_OFF   HIGH

//void handleRoot(void);
void handle_remote(void);
void handleNotFound(void);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void ctrl(char *payload);
void drv8830_setup(void);
void drv8830_func(char m_speed,char device,char dir);
void drv8830_Q(char m_speed);
void drv8830_W(char m_speed);
void drv8830_E(char m_speed);
void drv8830_A(char m_speed);
void drv8830_S(char m_speed);
void drv8830_D(char m_speed);
void drv8830_neutral(void);

//Global variable
WebServer server(80);
IPAddress ip;
DNSServer dnsServer;
WebSocketsServer webSocket = WebSocketsServer(81);
//環境設定->コンパイラの警告->初期値

uint8_t prev_btn_a = BTN_OFF;
uint8_t btn_a      = BTN_OFF;
const char* esp_ssid = "ESP32";
const char* esp_pass = "11111111";//WIFI:S:ESP32;T:WPA;P:11111111;;この文字列でWifiに接続できる

void setup(void) {
  pinMode(LED_PIN,OUTPUT);
  pinMode(BTN_A_PIN,INPUT_PULLUP);
  digitalWrite(LED_PIN,LED_OFF);
  Serial.begin(115200);
  //softap mode
  WiFi.softAP(esp_ssid,esp_pass);
  ip = WiFi.softAPIP();
  dnsServer.start(53, "*", ip);
  Serial.print("IP address: ");
  Serial.println(ip);
  
  M5.begin();
  M5.Axp.ScreenBreath(8); 
  //M5.Lcd.setRotation(1);// 0-3で画面の向き
  //M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("ID:"+String(esp_ssid));
  M5.Lcd.println("PW:"+String(esp_pass));
  M5.Lcd.println(ip);
  M5.Lcd.qrcode("WIFI:S:"+String(esp_ssid)+";T:WPA;P:"+String(esp_pass)+";;",0, 45, 80, 2);

  server.on("/", handle_remote);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  webSocket.begin();//WebSocketサーバー開始
  webSocket.onEvent(webSocketEvent);

  drv8830_setup();
}

void loop(void) {
  static char uart_buf[16];
  static int buf_i = 0;
  
  webSocket.loop();
  dnsServer.processNextRequest();
  server.handleClient();

  if (Serial.available()) { // 受信したデータが存在する
    uart_buf[buf_i] = Serial.read(); // 受信データを読み込む
    if(uart_buf[buf_i] == '\n'){//取り合えずEnterすると先頭に戻る       
      uart_buf[buf_i] = 0;
      buf_i = 0;
      ctrl(uart_buf);
    }
    else if(buf_i < 15) buf_i++;
    else buf_i = 0;
  }
  
  btn_a = digitalRead(BTN_A_PIN);
  if(prev_btn_a == BTN_OFF && btn_a == BTN_ON){
    drv8830_neutral();
    digitalWrite(LED_PIN,LED_ON);
    delay(100);//TBD
    digitalWrite(LED_PIN,LED_OFF);
  }
  prev_btn_a = btn_a;
}
/*
void handleRoot(void) {
  digitalWrite(LED_PIN,LED_ON);
  server.send(200, "text/plain", "hello from M5StickC!");
  digitalWrite(LED_PIN,LED_OFF);
}
*/

void handle_remote(void) {
  digitalWrite(LED_PIN,LED_ON);
  server.send(200, "text/HTML", index_str);
  digitalWrite(LED_PIN,LED_OFF);
}

void handleNotFound(void) {
  String ipStr = "http://" + String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3])+ '/';
  server.sendHeader("Location", ipStr, true);
  server.send(302, "text/plain", "");// Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop();// Stop is needed because we sent no content length
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
    case WStype_TEXT:{
      Serial.printf("[%u] get Text: %s\n", num, payload);//debug
      ctrl((char*)payload);
      break;
    }
  }
}

void ctrl(char *payload){
  int load_len = sizeof(payload);//sizeof("n,2\n") == 4
      
  if(load_len >= 3){     
    char raw_speed;
    if(payload[2]>='0' && payload[2]<='9')       raw_speed = payload[2]-'0';
    else if(payload[2]>='a' && payload[2]<='f')  raw_speed = payload[2]-'a'+0x0A;
    else if(payload[2]>='A' && payload[2]<='F')  raw_speed = payload[2]-'A'+0x0A;
    
    char m_speed = raw_speed + SPEED_0;
    //Serial.print("0x");
    //Serial.println(m_speed,HEX);
  
    if(payload[0] == 'q')       drv8830_Q(m_speed);
    else if(payload[0] == 'w')  drv8830_W(m_speed);
    else if(payload[0] == 'e')  drv8830_E(m_speed);
    else if(payload[0] == 'a')  drv8830_A(m_speed);
    else if(payload[0] == 's')  drv8830_S(m_speed);
    else if(payload[0] == 'd')  drv8830_D(m_speed);
    else if(payload[0] == 'n')  drv8830_neutral();
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
}

void drv8830_func(char m_speed,char device,char dir){
  //Serial.println(m_speed,HEX);
  
  Wire.beginTransmission(device);//I2Cスレーブ「Arduino Uno」のデータ送信開始
  Wire.write(CONTROL);//コントロール
  if(dir == DIR_N)  Wire.write(0x00);
  else              Wire.write(m_speed << 2 | dir);
  Wire.endTransmission();//I2Cスレーブ「Arduino Uno」のデータ送信終了
}

void drv8830_Q(char m_speed){
  drv8830_func(m_speed,DRV8830_A,DIR_N);
  drv8830_func(m_speed,DRV8830_B,DIR_CW);
}

void drv8830_W(char m_speed){
  drv8830_func(m_speed,DRV8830_A,DIR_CCW);
  drv8830_func(m_speed,DRV8830_B,DIR_CW);
}

void drv8830_E(char m_speed){
  drv8830_func(m_speed,DRV8830_A,DIR_CCW);
  drv8830_func(m_speed,DRV8830_B,DIR_N);
}

void drv8830_A(char m_speed){
  drv8830_func(m_speed,DRV8830_A,DIR_N);
  drv8830_func(m_speed,DRV8830_B,DIR_CCW);
}

void drv8830_S(char m_speed){
  drv8830_func(m_speed,DRV8830_A,DIR_CW);
  drv8830_func(m_speed,DRV8830_B,DIR_CCW);
}

void drv8830_D(char m_speed){
  drv8830_func(m_speed,DRV8830_A,DIR_CW);
  drv8830_func(m_speed,DRV8830_B,DIR_N);
}

void drv8830_neutral(void){
  drv8830_func(SPEED_0,DRV8830_A,DIR_N);
  drv8830_func(SPEED_0,DRV8830_B,DIR_N);
}
