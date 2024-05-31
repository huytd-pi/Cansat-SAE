#include <HardwareSerial.h>
#include <LoRa.h>
#include <WiFi.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WebSocketsServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define ESP32_TX  16 // Connect ESP32 TX to GPIO 16
#define ESP32_RX  17 // Connect ESP32 RX to GPIO 17
#define SD_CS 5


AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
HardwareSerial LoRaSerial(1);

String delimiter = "!@#$%^&*?";
String longti,lati,roll,pitch,press,alti,temper,humid,ppm,clockTime = ""; // Các biến để lưu thông tin
const char* ssid     = "ace";
const char* password = "hiep1109";
const char* dataPath = "/dataLogger.txt";
String data,jsonData;

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
}

void onEvent(uint8_t client_num, WStype_t type, uint8_t *payload, size_t length) {
}

void hashdata(String data,String &longti,String &lati,String &roll,String &pitch,String &clockTime,String &temper,String &press,String &alti,String &humid,String &ppm) {
  int prevIndex = 1;
  int delimiterIndex;
  for (int i = 0; i < 9; i++) {
    delimiterIndex = data.indexOf(delimiter.charAt(i));
    if (delimiterIndex != -1) {
      switch (i) {
        case 0: lati = data.substring(prevIndex + 1, delimiterIndex); break;
        case 1: longti = data.substring(prevIndex + 1, delimiterIndex); break;
        case 2: roll = data.substring(prevIndex + 1, delimiterIndex); break;
        case 3: pitch = data.substring(prevIndex + 1, delimiterIndex); break;
        case 4: clockTime = data.substring(prevIndex + 1, delimiterIndex); break;
        case 5: temper = data.substring(prevIndex + 1, delimiterIndex); break;
        case 6: press = data.substring(prevIndex + 1, delimiterIndex); break;
        case 7: humid = data.substring(prevIndex + 1, delimiterIndex); break;
        case 8: ppm = data.substring(prevIndex + 1, delimiterIndex); break;
      }
      prevIndex = delimiterIndex;
    }
  }
  alti = data.substring(prevIndex + 1);
}

void initSDCard(){
  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }
}
void appendFile(fs::FS &fs, const char * path, const char * message) {
  File file = fs.open(path, FILE_APPEND);
  file.println(message);
  file.close();
}
void initWiFi(const char* ssid, const char* pass) {
    WiFi.begin(ssid, pass);
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600); // Khởi tạo Serial Monitor
  LoRaSerial.begin(9600, SERIAL_8N1, 16, 17); // UART1, baud rate, RX, TX
  initWiFi(ssid,password);
  initSDCard();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SD, "/index.html", "text/html");
    });
  server.serveStatic("/", SD, "/").setDefaultFile("index.html");
  server.begin();
  webSocket.begin();
  webSocket.onEvent(onEvent);
}

void loop() {
  if (LoRaSerial.available() > 0) {
    webSocket.loop(); // Kiểm tra xem có dữ liệu nhận được từ LoRa không
    data = LoRaSerial.readString();
    hashdata(data, longti,lati,roll,pitch,clockTime,temper,press,alti,humid,ppm);
    jsonData = "{\"temperature\": " + temper + ", \"humidity\": " + humid + ", \"pressure\": " + press + ", \"altitude\": " + alti + ", \"latitude\": " + lati + ", \"longitude\": " + longti + ", \"roll\": " + roll + ", \"pitch\": " + pitch + ", \"time\": " + clockTime + "}";
    Serial.println(jsonData);
    webSocket.broadcastTXT(jsonData);
    appendFile(SD, "/dataLogger.txt", jsonData.c_str());
}
}
