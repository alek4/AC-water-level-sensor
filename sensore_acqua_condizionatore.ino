#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

String url = "http://maker.ifttt.com/trigger/{YOUR_EVENT}/with/key/{YOUR_API_KEY}";

#define POWER_PIN  16
#define SIGNAL_PIN A0

int value = 0;
bool notifSent = false;

void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Connected!");

  pinMode(POWER_PIN, OUTPUT);   
  digitalWrite(POWER_PIN, LOW); 
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("Sensor value: ");
  Serial.println(value);

  if (value > 200 && !notifSent) {
    triggerEvent();
    notifSent = true;
  }

  if (value < 150) {
    notifSent = false;
  }

  delay(30000); // wait 30 sec
}

void triggerEvent() {
   WiFiClient client;
   HTTPClient http;
   
   Serial.print("[HTTP] begin...\n");
    if (http.begin(client, url)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
 }
