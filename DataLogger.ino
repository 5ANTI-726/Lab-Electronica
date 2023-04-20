//DEBUGGING
bool debugSerial = true;
bool debugOLED = false;

//ERRORS, TIME, AND COUNTERS
unsigned int errorCount = 0;
unsigned long lastErrorCode = 0;
unsigned long lastLog = 0;
unsigned long period = 15000;
float m = 1;


//HUMIDITY SENSOR
#include <DHT.h>
#define DHTPIN 15 
DHT dht(DHTPIN, DHT11);

//WIFI CLIENT LIBRARY
#include <ESP8266WiFi.h>
const char *ssid = "INFINITUM259A_2.4";
const char *password = "Bvtyq5SsDp";
WiFiClient  client;

//THINGSPEAK API LIBRARY
#include "ThingSpeak.h"
unsigned long myChannelNumber = 2110271;
const char *myWriteAPIKey = "AXCQ3SZWHB5A5CVQ";

//THERMOCOUPLE
#include "max6675.h"
unsigned int thermoDO = 23;
unsigned int thermoCS = 22;
unsigned int thermoCLK = 24;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

//OLED SCREEN
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET    21  // Reset pin # (or -1 if sharing reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);  //Initialize serial

  delay(100);
  
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    while(1); //Don't proceed, loop forever
  }
  else{
    Serial.println(F("SSD1306 allocation successful."));
  }
}

void loop() {
  if((millis() - lastLog) > period){
    if (WiFi.status() == WL_CONNECTED){
      delay(100);
      if(debugSerial){
        Serial.println();
        Serial.print(F("WiFi status: connected: "));
        Serial.println(WiFi.status());
        Serial.print(F("Assigned IP address: "));
        Serial.println(WiFi.localIP());
      }

      float thermocouple = readThermocouple();
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();

      if (!isnan(thermocouple)){
        //Update Thermocouple field
        ThingSpeak.setField(1, thermocouple);
        if(debugOLED){
          OLEDFieldData("Temp. thermocouple (C): ", thermocouple);
        }
      }
      else{
        if(debugOLED){
          printOLED(F("Error with thermocouple temp."));
        }
      }

      if (!isnan(humidity)){
        //Update Thermocouple field
        ThingSpeak.setField(2, humidity);
        if(debugOLED){
          OLEDFieldData("Humidity (%): ", humidity);
        }
      }
      else{
        if(debugOLED){
          printOLED(F("Error with DHT11 humidity."));
        }
      }

      if (!isnan(thermocouple)){
        //Update Thermocouple field
        ThingSpeak.setField(3, temperature);
        if(debugOLED){
          OLEDFieldData("Temp. DHT11 (C): ", temperature);
        }
      }
      else{
        if(debugOLED){
          printOLED(F("Error with DHT temp."));
        }
      }

      ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey)
      lastLog = millis();
    }
    if (WiFi.status() != WL_CONNECTED){
      lastErrorCode = WiFi.status();
      errorCount++;
      delay(100);
      if(debugSerial){
        Serial.println();
        Serial.println(F("WiFi status: disconnected"));
        Serial.print(F("Error code: "));
        Serial.println(WiFi.status());
        Serial.print(F("Last error code: "));
        Serial.println(lastErrorCode);
      }
      WiFi.begin(ssid, password);
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