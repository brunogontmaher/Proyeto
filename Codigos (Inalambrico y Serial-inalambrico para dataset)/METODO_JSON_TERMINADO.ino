#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <MPU6050.h>
#include <ArduinoJson.h> 

const char *ssid = "AndroidAP618B";
const char *password = "doip8189";
const int serverPort = 8080; 
const char *serverAddress = "192.168.34.107"; 

//SECTOR BOTONES
const int pinAlternar = 13;  // Pin D8 para el pulsador táctil
int contadorAlternar = 0;
const int pinPresentacion = 12;
int contadorPresentacion = 0;
int pinAudio = 14;
int contadorAudio = 0;

MPU6050 mpu;



void setup() {
  Wire.begin();
  mpu.initialize();
  Serial.begin(115200);

  //SECTOR BOTONES
  pinMode(pinAlternar, INPUT_PULLUP); 
  pinMode(pinPresentacion, INPUT_PULLUP); 
  pinMode(pinAudio, INPUT_PULLUP); 

  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");

  Serial.println("Configuring HTTP server...");
}

void loop() {

unsigned long startTime = millis();

while (millis() - startTime < 100) {
  // int16_t ax, ay, az;
  int gx, gy;
  int vx, vy;   

//  int ax, az;

  //SECTOR BOTONES
  int estadoAlternar = digitalRead(pinAlternar);  // Leer el estado del pulsador
  int estadoPresentacion = digitalRead(pinPresentacion);
  int estadoAudio = digitalRead(pinAudio);

 // ax = mpu.getAccelerationX();
 // ay = mpu.getAccelerationY();
 // az = mpu.getAccelerationZ();

   if(contadorPresentacion < 5){
    estadoPresentacion = 0;
    contadorPresentacion++;
  }
  else if(estadoPresentacion == 1){
    estadoPresentacion = 1;
    contadorPresentacion = 0;
  }
  if(contadorAlternar < 5){
     estadoAlternar = 0;
     contadorAlternar++;
     }
  else if(estadoAlternar == 1){
        estadoAlternar = 1;
        contadorAlternar = 0;
    }
  if (contadorAudio < 5) {
    estadoAudio = 0;
    contadorAudio++;
  } 
  else if (estadoAudio == 1) {
    estadoAudio = 1;
    contadorAudio = 0;
  }
  
  gx = mpu.getRotationX();
    gy = mpu.getRotationY();
//  gz = mpu.getRotationZ();

   vx = -(gy-100)/150;   //Calculando os valores de vx e vy.
   vy = -(gx+15)/150;

  // vx = -(ax-100)/150;
  // vy = -(az+15)/150;  
//  float ax_m_s2 = ax * (9.81/16384.0);
//  float ay_m_s2 = ay * (9.81/16384.0);
//  float az_m_s2 = az * (9.81/16384.0);
//  float gx_deg_s = gx * (250.0/32768.0);
//  float gy_deg_s = gy * (250.0/32768.0);
//  float gz_deg_s = gz * (250.0/32768.0);

  // Create a JSON document
  StaticJsonDocument<200> jsonDocument;
//  jsonDocument[0] = ax_m_s2; 
//  jsonDocument[1] = ay_m_s2;
//  jsonDocument[2] = az_m_s2;
//  jsonDocument[3] = gx_deg_s;
//  jsonDocument[4] = gy_deg_s;
//  jsonDocument[5] = gz_deg_s;

  jsonDocument[0] = String(vx); 
  jsonDocument[1] = String(vy);
  jsonDocument[2] = String(estadoAlternar);
  jsonDocument[3] = String(estadoPresentacion);
  jsonDocument[4] = String(estadoAudio);
  // Serialize the JSON document to a string
  String jsonData;
  serializeJson(jsonDocument, jsonData);

  Serial.println(jsonData);

 // Serial.println("Sending data to the server...");

  HTTPClient http;
  WiFiClient client;
  http.begin(client, "http://" + String(serverAddress) + ":" + String(serverPort));
//  http.addHeader("Content-Type", "application/json, charset=utf-8");

   http.addHeader("Content-Type", "application/json");

  // Send the JSON data to the server
  int httpCode = http.POST(jsonData);

  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
  //  Serial.println("Server Response:");
  //  Serial.println(response);
  } else {
 //   Serial.print("HTTP Connection Error: ");
 //   Serial.println(httpCode);
  }

  http.end();

}
  
}
