#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <LoRa.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <math.h>
#include <MQ135.h>

#define DHTPIN 5        // Chân kết nối cảm biến DHT22
#define DHTTYPE DHT22   // Loại cảm biến DHT (DHT22)
#define GPS_TX_PIN 6 // Chân TX của GPS kết nối với chân RX của Arduino
#define GPS_RX_PIN 7 // Chân RX của GPS kết nối với chân TX của Arduino
#define lora_TX_PIN 4
#define lora_RX_PIN 3
#define MPU6050_ADDR 0x68
#define SERVO_PIN 8
#define PIN_MQ135 A2

DHT dht(DHTPIN, DHTTYPE);
MQ135 mq135_sensor = MQ135(PIN_MQ135);
Adafruit_BMP085 bmp;
TinyGPSPlus gps; // Khởi tạo đối tượng TinyGPS++
Servo myServo;
SoftwareSerial gpsSerial(6,7);
SoftwareSerial loraSerial(3,4);

// Biến thời gian để theo dõi thời gian giữa các lần đọc dữ liệu
unsigned long currentMillis;
unsigned long previousMillis;
int interval = 2000;
float initialPressure;
float press,longti,lati,ppm,humid,temper,roll,pitch,alti;
int16_t AcX,AcY,AcZ;
String data;
bool isActivated = false;

void gpsdata(float &lati,float &longti){
  while (gpsSerial.available() > 0) {
  gps.encode(gpsSerial.read());
  }
  if (gps.location.isUpdated()) {
  lati = gps.location.lat();
  longti = gps.location.lng();
  }
}
void dhtsensor(float &humid,float &temper){
  humid = dht.readHumidity();
  temper = dht.readTemperature();
}
void mq135(float humid,float temper,float &ppm){
  ppm = mq135_sensor.getCorrectedPPM(temper, humid);
}
void rollValue(int16_t AcX,int16_t AcY,int16_t AcZ,float &roll){
  roll = atan(AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180/3.14;
}
void pitchValue(int16_t AcX,int16_t AcY,int16_t AcZ,float &pitch){
  pitch = atan(-AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180/3.14;
}
void bmpsensor(float &press,float &alti,float initialPressure){
  press = bmp.readPressure();
  alti = 44330 * (1 - pow(press / initialPressure, 0.1903));
}
void setup(){
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
  // digitalWrite(8,LOW);
  // myServo.write(90);
  loraSerial.begin(9600); // Khởi tạo lora
  gpsSerial.begin(9600);
  Wire.begin(); // Khởi tạo giao diện I2C
  dht.begin();
  bmp.begin();
  initialPressure = bmp.readPressure();
  Serial.println(initialPressure);
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.println("khoi tao thanh cong!");
  }
void loop() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 6, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { // Nếu đã trôi qua 2 giây
    previousMillis = currentMillis; // Cập nhật thời điểm cuối cùng
      gpsdata(lati,longti);
      rollValue(AcX,AcY,AcZ,roll);
      pitchValue(AcX,AcY,AcZ,pitch);
      dhtsensor(humid,temper);
      mq135(humid,temper,ppm);
      bmpsensor(press,alti,initialPressure);
      data = String(lati,6) + "!" + String(longti,6) + "@" + String(roll,0) + "#" + String(pitch,0) + "$" + String(currentMillis/1000) + "%" + String(temper,1) + "^" + String(press/1000,3) + "&" + String(humid,1) + "*" + String(ppm) + "?" + String(alti,0);
      loraSerial.println(data);
      Serial.println(data);
    }
  // if(!isActivated){
  //   rollValue(AcX,AcY,AcZ,roll);
  //   pitchValue(AcX,AcY,AcZ,pitch);
  //   float tiltang = atan(sqrt( pow(tan(roll*3.14/180),2) + pow(tan(pitch*3.14/180),2))) * 180/3.14;
  //   Serial.println(tiltang);
  //   if(!isActivated && (abs(tiltang) > 30||AcZ < 3000)){
  //     isActivated = true;
  //     digitalWrite(8, HIGH);
  //     myServo.write(180);
  //     delay(500);
  //     myServo.write(90);
  //     digitalWrite(8, LOW);
  //   }
  // } 
}