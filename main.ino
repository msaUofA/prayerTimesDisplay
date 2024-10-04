#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"

const char* ssid = "eduroam";
const char* host = "https://www.google.com";
int i = 0;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print("connecting to ");
  Serial.print(ssid);
  Serial.print(" with identity ");
  Serial.print(EAP_IDENTITY);
  Serial.print(": ");
  WiFi.disconnect(true);  //disconnect from wifi to set new wifi connection
  WiFi.mode(WIFI_STA);    //init wifi mode

  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  
    i++;
    if (i >= 60) {  //after timeout - reset board
      ESP.restart();
      return;
    }
  }
  Serial.println();
  Serial.println("connected!");
  Serial.print("ip address: ");
  Serial.println(WiFi.localIP()); 

  
  Serial.print("connecting to host: ");
  Serial.println(host);  

  HTTPClient http;
  http.begin(host);
  int response = http.GET();
  
  if (response > 0) {
    Serial.print("got http response ");
    // String response_str = http.getString();
    Serial.println(String(response));

    
  } else {
    Serial.print("Error in GET request: ");
    Serial.println(String(response));
  }
  
  http.end();  

}

void loop() {}
