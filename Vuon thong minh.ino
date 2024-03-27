#include <WiFi.h>
#include "time.h"
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>                                 
#include <Arduino.h>                                                 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0X27,16,2);
#include<HTTPClient.h>

const char* ssid       = "30 PDG";     
const char* password   = "bktech1017";      

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
String Web_App_URL = "https://script.google.com/macros/s/AKfycbzqB6nK12Py1oo2p2uL86NVr8Xprxs4XpW-JdKA7v2Eom55kQpLWtdVZ84yumOMtrdE/exec?";

int frame;
int f, setTemp, setHum, setSoil, t, h;

String frameState;

#define DHTPIN 19              // Digital pin connected to the DHT sensor
#define Doam 34
#define DHTTYPE DHT11          // DHT 11

#define Buzzer 17
DHT dht(DHTPIN, DHTTYPE);

String readDHTTemperature() {
  float t = dht.readTemperature();

  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    return String(t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return "--";
  }
  else {
    return String(h);
  }
}

void setup() {
  Serial.begin(115200);
  
  dht.begin();
  pinMode(Buzzer, OUTPUT);
  pinMode(Doam, INPUT);
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  lcd.init();                    
  lcd.backlight(); 
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  Serial.println("/nCONNECTED");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
  server.on("/1", HTTP_GET, [](AsyncWebServerRequest *request){
    f = 1;             //frame = 1;
    request->send(SPIFFS, "/index.html");
  });
  server.on("/4", HTTP_GET, [](AsyncWebServerRequest *request){
    f = 4;            //frame = 4;
    request->send(SPIFFS, "/index.html");
  });
  server.on("/5", HTTP_GET, [](AsyncWebServerRequest *request){
    f = 5;            //frame = 5;
    request->send(SPIFFS, "/index.html");
  });
  server.on("/6", HTTP_GET, [](AsyncWebServerRequest *request){
    f = 6;            //frame = 6;
    request->send(SPIFFS, "/index.html");
  });
   server.on("/submit", HTTP_POST, [](AsyncWebServerRequest *request){
    // Xử lý dữ liệu được gửi từ trình duyệt ở đây
    if (request->hasArg("tree")) {
      String tree = request->arg("tree");
      String air_temp = request->arg("air_temp");
      String air_humidity = request->arg("air_humidity");
      String soil_humidity = request->arg("soil_humidity");
      digitalWrite(Buzzer, HIGH);
      delay(100);
      digitalWrite(Buzzer, LOW);
      delay(100);
      digitalWrite(Buzzer, HIGH);
      delay(100);
      digitalWrite(Buzzer, LOW);
      delay(100);
      // Xử lý dữ liệu nhận được ở đây
  
      setTemp = air_temp.toInt();
      setHum = air_humidity.toInt();
      setSoil = soil_humidity.toInt();    
//      Serial.println("Tree: " + tree);
//      Serial.println("Air Temperature: " + air_temp);
//
//      Serial.println("Air Humidity: " + air_humidity);
//      Serial.println("Soil Humidity: " + soil_humidity);
    }
    request->send(SPIFFS, "/index.html");
  });
  server.begin();
}

void lcdout() {
  int SoilHum = analogRead(Doam);
  SoilHum = map(SoilHum, 0, 4095, 0, 100);
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.setCursor(5, 0);
  lcd.print(t);
  lcd.setCursor(8, 0);
  lcd.print("Humd:");
  lcd.setCursor(13, 0);
  lcd.print(h);
  lcd.setCursor(0, 1);
  lcd.print("Soil:");
  lcd.setCursor(5, 0);
  lcd.print(SoilHum);
}

void loop() {
  t = dht.readTemperature();
  h = dht.readHumidity();
  lcdout();
  if(t > setTemp || f > setHum) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Canh bao");
    digitalWrite(Buzzer, HIGH);
    delay(5000);
    digitalWrite(Buzzer, LOW);
  }
  if (WiFi.status() == WL_CONNECTED) {
      String Send_Data_URL = Web_App_URL;
      Send_Data_URL += "Temperature=" + String(t);
      Send_Data_URL += "&Humidity=" + String(h);

        HTTPClient http;
    
        http.begin(Send_Data_URL.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    
        int httpCode = http.GET(); 
        http.end();
      delay(2000);
    }
  if((f>=1)&&(f<3))
  {
    f++;
  }
  else if((f>=6)&&(f<11))
  {
    f++;  
  }
  else if( f == 4)
  {
    
  }
  else if( f == 5)
  {
   
  }
  else 
  {
    
  }
}
