//================this is the base code for Team 2 K65-SAE CANSAT PROJECT===========================
//unique modules: Using LoRa receiver and ESP8266 to connect the Thingspeak web server
//NOTE: set the delay to receiver data from Lora sender over 500ms
//===================================================================================================




#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

#define BAND 419E6

String apiKey = "CJNQMYA5M1W5ECB3"; 
String apiKey1 = "SXCPTERW26EHG90A"; 
String ssid  = "N350NT";
String password = "27032016";
 


#define TX_PIN 2
#define RX_PIN 3

#define SERVER "api.thingspeak.com"
#define PORT 80

SoftwareSerial loraSerial(D2, D3); // Khai báo SoftwareSerial cho module LoRa

String device_id;
String temperature;
String pressure;
String altitude;
String humidity;
String rainfall;
String lux;
String accX;
String accY;
String accZ;
String angleX;
String angleY;
String angleZ;
String Latitude;
String Longitude;
WiFiClient client;


void setup() {
  Serial.begin(9600);
  while (!Serial);

  loraSerial.begin(9600); // Khởi động SoftwareSerial

  Serial.println("Chương trình đã khởi động.");
  Serial.println("Connecting to ");
  Serial.println(ssid);
 
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
 
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  
}

void loop() {
  String LoRaMessage;
  int pos1, pos2, pos3, pos4, pos5;
 
  if (loraSerial.available() > 0) {
    String LoRaData = loraSerial.readString();
    Serial.print("Received packet: ");
    Serial.println(LoRaData);

    pos1 = LoRaData.indexOf('a');
    pos2 = LoRaData.indexOf('b');
    pos3 = LoRaData.indexOf('c');
    pos4 = LoRaData.indexOf('d');

    device_id = LoRaData.substring(0, pos1);

    if(device_id.toInt() == 0){
      temperature = LoRaData.substring(pos1 + 1, pos2);
      pressure = LoRaData.substring(pos2 + 1, pos3);
      altitude = LoRaData.substring(pos3 + 1, pos4);
      humidity = LoRaData.substring(pos4 + 1, LoRaData.length());

      Serial.print("Device ID: ");
      Serial.println(device_id);

      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" *C");

      Serial.print("Pressure: ");
      Serial.print(pressure);
      Serial.println(" hPa");

      Serial.print("Altitude: ");
      Serial.print(altitude);
      Serial.println(" m");

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");

      Serial.println();
    
    }else if(device_id.toInt() == 1){
      Serial.print("Device ID: ");
      Serial.println(device_id);
      accX = LoRaData.substring(pos1 + 1, pos2);
      accY = LoRaData.substring(pos2 + 1, pos3);
      accZ = LoRaData.substring(pos3 + 1, pos4);    
      rainfall = LoRaData.substring(pos4 + 1, LoRaData.length());
      Serial.print("AccX: ");
      Serial.print(accX);  

      Serial.print("\tAccY: ");
      Serial.print(accY);  

      Serial.print("\tAccZ: ");
      Serial.print(accZ);  

      Serial.print("\tRainfall: ");
      Serial.print(rainfall);  
      Serial.println();
    }else if(device_id.toInt() == 2){
      Serial.print("Device ID: ");
      Serial.println(device_id);
      angleX = LoRaData.substring(pos1 + 1, pos2);
      angleY = LoRaData.substring(pos2 + 1, pos3);
      angleZ = LoRaData.substring(pos3 + 1, pos4); 
      lux = LoRaData.substring(pos4 + 1, LoRaData.length());

      Serial.print("AngleX: ");
      Serial.print(angleX);  

      Serial.print("\tAngleY: ");
      Serial.print(angleY);  

      Serial.print("\tAngleZ: ");
      Serial.println(angleZ); 

      Serial.print("Light: ");
      Serial.print(lux);
      Serial.println(" lx");
    }else if(device_id.toInt() == 3){
       Serial.print("Device ID: ");
       Serial.println(device_id);
       Latitude = LoRaData.substring(pos1 + 1, pos2);
       Longitude = LoRaData.substring(pos2 + 1, pos3);
      Serial.print("Location: ");
      if(Latitude.toFloat()==0 && Longitude.toFloat()==0){
        Serial.println(F("INVALID"));
      }
      else{
        Serial.print(Latitude.toFloat(),6);
        Serial.print(", ");
        Serial.println(Longitude.toFloat(),6);
      }
    }
  }
  if (client.connect(SERVER, PORT)) {
    if(device_id.toInt() == 0 || device_id.toInt() == 2){
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += temperature;
      postStr += "&field2=";
      postStr += pressure;
      postStr += "&field3=";
      postStr += altitude;
      postStr += "&field4=";
      postStr += humidity;
      postStr += "&field5=";
      postStr += rainfall;
      postStr += "&field6=";
      postStr += lux;
      postStr += "\r\n";

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: ");
      client.print(SERVER);
      client.print("\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: ");
      client.print(apiKey);
      client.print("\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);

      Serial.println("Data sent to ThingSpeak");
      delay(500);
    
    client.stop();
    Serial.println("Waiting...");
    } else {
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += accX;
      postStr += "&field2=";
      postStr += accY;
      postStr += "&field3=";
      postStr += accZ;
      postStr += "&field4=";
      postStr += angleX;
      postStr += "&field5=";
      postStr += angleY;
      postStr += "&field6=";
      postStr += angleZ;
      postStr += "\r\n";

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: ");
      client.print(SERVER);
      client.print("\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: ");
      client.print(apiKey1);
      client.print("\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);

      Serial.println("Data sent to ThingSpeak");
      delay(500);
    
    client.stop();
    Serial.println("Waiting...");
    }
  }
}