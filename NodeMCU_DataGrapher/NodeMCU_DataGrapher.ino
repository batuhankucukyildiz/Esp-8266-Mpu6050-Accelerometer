
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include "index.h" //Html Üzerinden Js Dosyası

const double VCC = 3.3;             // NodeMCU 3.3 V 
const double R2 = 10000;            // 10k ohm Resıstor Eğer CPU Kullanılmak İstenirse
const double adc_resolution = 1023; // 10-bit adc

const double A = 0.001129148;   
const double B = 0.000234125;
const double C = 0.0000000876741; 

// MPU6050 Adresleme
const uint8_t MPU6050SlaveAddress = 0x68;

// SDA SCL PİNLERİ İÇİN  I2C İletişimi 
const uint8_t led =D0;
const uint8_t scl =D6;
const uint8_t sda =D7;


const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;


const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;

  int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;



  const char* ssid = "Accelerometer(192.168.0.186)";
  IPAddress IPaddr (192,168,0,186);
  IPAddress IPmask(255, 255, 255, 0);
  ESP8266WebServer server(80); //Server on port 80
 
  void handleRoot() {
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
  }

 void handleADC() {
 double Vout, Rth, temperature, adc_value; 

  adc_value = analogRead(A0);
  Vout = (adc_value * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;


  temperature = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)),3))));  
  temperature = temperature - 273.15;  
  int a = temperature;

  double Ax, Ay, Az, T, Gx, Gy, Gz;
  
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  
  Ax = (double)AccelX/AccelScaleFactor;
  Ay = (double)AccelY/AccelScaleFactor;
  Az = (double)AccelZ/AccelScaleFactor;
  T = (double)Temperature/340+36.53; 
  Gx = (double)GyroX/GyroScaleFactor;
  Gy = (double)GyroY/GyroScaleFactor;
  Gz = (double)GyroZ/GyroScaleFactor;

 String data = "{\"ADC\":\""+String(a)+"\", \"Temperature\":\""+ String(T) +"\", \"Ax\":\""+ String(Ax) +"\", \"Ay\":\""+ String(Ay) +"\", \"Az\":\""+ String(Az) +"\", \"Gx\":\""+ String(Gx) +"\", \"Gy\":\""+ String(Gy) +"\", \"Gz\":\""+ String(Gz) +"\"}";
 server.send(200, "text/plane", data); 
 
  Serial.print("Ax: "); Serial.print(Ax);
  Serial.print(" Ay: "); Serial.print(Ay);
  Serial.print(" Az: "); Serial.print(Az);
  Serial.print(" Gx: "); Serial.print(Gx);
  Serial.print(" Gy: "); Serial.print(Gy);
  Serial.print(" Gz: "); Serial.println(Gz);
  
  Serial.print(temperature, 1);
  
}

  void setup()
  {
  pinMode(led,OUTPUT);
  Serial.begin(115200);
  Wire.begin(sda, scl);
  MPU6050_Init();
  Serial.println();
  WiFi.softAP(ssid);     
  WiFi.softAPConfig(IPaddr, IPaddr, IPmask);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Acces Point");
  Serial.print(myIP);
  Serial.println(WiFi.softAPIP());
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(myIP);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());  
  server.on("/", handleRoot);      
  server.on("/readADC", handleADC); 
  server.begin(); 
  Serial.println("HTTP server started");
    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOT: FS kullandıysanız FS.end()komutu ile dosyayı kapatmalısınız
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Hata[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Yetki Hatası");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Baslatma Basarisiz");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Baglanti Basarisiz");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Veri Alinamadi");
    } else if (error == OTA_END_ERROR) {
      Serial.println("Sonlandirma Basarisiz");
    }
  });
  ArduinoOTA.begin();//OTA'yi başlatıyor
  Serial.println("Sistem Hazir");
  Serial.print("IP Adresiniz: ");
  Serial.println(WiFi.localIP());

  }

  void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
 }


  void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress){
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  AccelX = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelY = (((int16_t)Wire.read()<<8) | Wire.read());
  AccelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  GyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}


void MPU6050_Init(){
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
  }

  void loop()
 {
  ArduinoOTA.handle(); 
  server.handleClient();   
  digitalWrite(led,LOW);
  
 }
