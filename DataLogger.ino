#include <ESP8266WiFi.h>
const char *ssid = "iPhone (4)";
const char *password = "abc12345";
unsigned int WiFiRetries;

int errorCount;
unsigned long lastError = 0;
unsigned long lastLog = 0;
unsigned long period = 5000;

#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11
static const int DHT_SENSOR_PIN = 15;

#include "max6675.h"
int thermoDO = 23;
int thermoCS = 22;
int thermoCLK = 24;

void setup() {
  connectWiFi();
  HTTPClient http;
  DHT_nonblocking dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
  MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
  Serial.begin(9600);
}

void loop() {
  errorCount = 0;
  if((millis() - lastLog) > period){
    if(WiFi.status() == WL_CONNECTED){
      errorCount = 0;
      WiFiClient client;
      HTTPClient http;

      String updateThermocouple = "GET https://api.thingspeak.com/update?api_key=AXCQ3SZWHB5A5CVQ&field1=";
      String updateHumid = "GET https://api.thingspeak.com/update?api_key=AXCQ3SZWHB5A5CVQ&field2=";
      String updateTemp = "GET https://api.thingspeak.com/update?api_key=AXCQ3SZWHB5A5CVQ&field3=";

      float temperature;
      float humidity;
      float thermocouple = readThermocouple();
      if( dht_sensor.measure(temperature, humidity)  == true ){
        //Update Temperature field
        http.begin(client, (updateTemp + temperature).c_str());
        // Send HTTP GET request
        int httpResponseCode = http.GET();
        Serial.println("Temp field HTTP error code:");
        Serial.println(httpResponseCode);

        //Update Humidity field
        http.begin(client, (updateHumid + humidity).c_str());
        // Send HTTP GET request
        int httpResponseCode = http.GET();
        Serial.println("Humidity Field HTTP error code:");
        Serial.println(httpResponseCode);
        http.end();
      }
      
      //Update Thermocuple field
      http.begin(client, (updateThermocouple + thermocouple).c_str());
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      Serial.println("Thermocouple Field HTTP error code:");
      Serial.println(httpResponseCode);
      http.end();

      lastLog = millis();
    }
    else{
      Serial.println("WiFi connection error #" + errorCount);
      Serial.println("Last error ocurred: " + errorCount + " milliseconds ago.");
      connectWifi();
      lastError = millis();
      errorCount++;
    }
  }
}

void connectWiFi(){
  //Connect to WiFi Network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to WiFi");
  Serial.println("...");
  WiFi.begin(ssid, password);

  int i = 0;
  while ((WiFi.status() != WL_CONNECTED) && (i < WiFiRetries)) {
    i++;
    delay(250);
    Serial.print(".");
    WiFi.begin(ssid, password);
  }
  if (i > 5) {
    Serial.println(("WiFi connection failed: Connection timed out (" + (250*WiFiRetries) + " ms).")));
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }s
}

float readThermocouple(){
  return thermocouple.readCelsius()
}