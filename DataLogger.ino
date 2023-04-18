//HANDLE CONVERSION TO STRING
#include <iostream>
#include <string>
#include <sstream> 

//ARDUINO SENSOR LIBRARY
#include <Adafruit_Sensor.h>

//THINGSPEAK LIBRARY
#include "ThingSpeak.h"
unsigned long myChannelNumber = 2110271;
const char * myWriteAPIKey = "AXCQ3SZWHB5A5CVQ";

//WIFI
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
const char *ssid = "iPhone (4)";
const char *password = "abc12345";
unsigned int WiFiRetries = 10;

//ERRORS AND TIME
int errorCount;
unsigned long lastError = 0;
unsigned long lastLog = 0;
unsigned long period = 5000;

//DHT11 MODULE
#include "DHT.h"
#define DHTPIN 15
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

//THERMOCOUPLE
#include "max6675.h"
int thermoDO = 23;
int thermoCS = 22;
int thermoCLK = 24;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

//OLED SCREEN
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET    21  // Reset pin # (or -1 if sharing reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void connectWiFi(){
  //Connect to WiFi Network
  Serial.println(F("Connecting to WiFi"));
  Serial.println(F("..."));
  WiFi.begin(ssid, password);

  int i = 0;
  while ((WiFi.status() != WL_CONNECTED) && (i < WiFiRetries)) {
    i++;
    delay(250);
    Serial.print(F("."));
    WiFi.begin(ssid, password);
  }
  if (i > 5) {
    Serial.println("WiFi connection failed: Connection timed out.");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected!"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  WiFiClient client;
  ThingSpeak.begin(client);
  connectWiFi();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while(1); //Don't proceed, loop forever
  }
  else{
    Serial.println(F("SSD1306 allocation successful."));
  }
  Serial.begin(9600);
}

void loop() {
  errorCount = 0;
  if((millis() - lastLog) > period){
    if(WiFi.status() == WL_CONNECTED){
      errorCount = 0;

      float thermocouple = readThermocouple();
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();

      if (!isnan(thermocouple)){
        //Update Thermocouple field
        ThingSpeak.writeField(myChannelNumber, 1, thermocouple, myWriteAPIKey);
        OLEDFieldData("Temp. thermocouple (C): ", thermocouple);
        lastLog = millis();
      }
      else{
        printOLED("Error with thermocouple temp.");
      }

      if (!isnan(humidity)){
        //Update Humidity field
        ThingSpeak.writeField(myChannelNumber, 2, humidity, myWriteAPIKey);
        OLEDFieldData("Humidity (%): ", humidity);
        lastLog = millis();
      }
      else{
        printOLED("Error with DHT11 humidity.");
      }

      if (!isnan(temperature)){
        //Update Temperature field
        ThingSpeak.writeField(myChannelNumber, 3, temperature, myWriteAPIKey);
        OLEDFieldData("Temp. DHT11 (C):", temperature);
        lastLog = millis();
      }
      else{
        printOLED("Error with DHT11 temp.");
      }
    }
    else{
      Serial.println();
      Serial.println("WiFi connection error #");
      Serial.println(errorCount);
      Serial.println("Last error ocurred: ");
      Serial.println(errorCount);
      Serial.println(" milliseconds ago.");
      connectWiFi();
      lastError = millis();
      errorCount++;
    }
  }
}

float readThermocouple(){
  return thermocouple.readCelsius();
}

void printOLED(String input){
  display.clearDisplay();
  display.setTextColor(WHITE);  
  display.setTextSize(4);
  display.setCursor(1, 1);
  display.println(input); 
  display.display();
  delay(500);
}
void OLEDFieldData(String input, float value){
  display.clearDisplay();
  display.setTextColor(WHITE);  
  display.setTextSize(3);
  display.setCursor(1, 1);
  display.println(input); 
  display.setCursor(1, 30);
  display.println(value);
  display.display();
  delay(500);
}