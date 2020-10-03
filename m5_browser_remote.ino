#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <M5StickC.h>
#include "secret.h"
#include "index.h"

// このLEDは、GPIO10の電位を下げることで発光するタイプ
#define LED_ON  LOW
#define LED_OFF HIGH

WebServer server(80);

const int led = 10;//edit

void handleRoot() {
  digitalWrite(led, LED_ON);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, LED_OFF);
}

void handleRC() {
  digitalWrite(led, LED_ON);
  server.send(200, "text/HTML", index_str);
  digitalWrite(led, LED_OFF);
}

void handleNotFound() {
  digitalWrite(led, LED_ON);
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
  digitalWrite(led, LED_OFF);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, LED_OFF);
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

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/rc", handleRC);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
