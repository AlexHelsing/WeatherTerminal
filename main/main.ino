#include "rpcWiFi.h"
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <TFT_eSPI.h>
#include "config.h"

// Replace with your country code and city
String city = "Boras";
String countryCode = "SE";

unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
unsigned long timerDelay = 60000;

String jsonBuffer;

TFT_eSPI tft;

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);

  Serial.begin(115200);

  WiFi.begin(SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  tft.drawString("Connecting to wifi", 0, 0);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.drawString("Connecting to wifi", 0, 0);
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  tft.drawString("Connected to wifi ", 0, 0);
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
  tft.fillScreen(TFT_BLACK);
  }

void loop() {
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
    
      String temperature =  JSON.stringify(myObject["main"]["temp"]);;
      String humidity =  JSON.stringify(myObject["main"]["humidity"]);
      String windspeed = JSON.stringify(myObject["wind"]["speed"]);
      String description = JSON.stringify(myObject["weather"][0]["main"]);
    
      tft.fillScreen(TFT_BLACK);

      // Weather Updates on screen
      tft.setTextSize(4);
      tft.drawString("Boras", 100, 10);
      tft.setTextSize(2);
      tft.drawString("Temperature: " + temperature + "c", 20, 70);
      tft.drawString("Humidity: " + humidity, 20, 100);
      tft.drawString("Wind Speed: " + windspeed, 20, 130);
      tft.drawString("Status: " + description, 20, 160);

    }
    else {
      Serial.println("WiFi Disconnected");
      tft.drawString("Wifi disconnected", 0, 0);
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}