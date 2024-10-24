#include <TM1637.h>
#include <MD_MAX72XX.h>
#include <MD_Parola.h>
#include <SPI.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secrets.hh"

#define SIMULATION

String fajr, sunrise, dhuhr, asr, maghrib, isha;

// TM1637
#define CLK1 12 
#define DIO1 13 

#define CLK2 27 
#define DIO2 14 

#define CLK3 25 
#define DIO3 33 

#define CLK4 22 
#define DIO4 23 

#define CLK5 18 
#define DIO5 19 

#define CLK6 2
#define DIO6 0

TM1637 display1;
TM1637 display2;
TM1637 display3;
TM1637 display4;
TM1637 display5;
TM1637 display6;

// MAX7219
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 4
#define DATA_PIN   16
#define CLK_PIN    5
#define CS_PIN     17

MD_Parola max72 = MD_Parola(
  HARDWARE_TYPE,
  DATA_PIN,
  CLK_PIN,
  CS_PIN,
  MAX_DEVICES
);


unsigned long t1 = 0;
const long interval = 5000; // 2 seconds 
bool toggle = true;


const uint8_t SEGMENTS[] = {0b01111111, 0b01111111, 0b01111111, 0b01111111};

void connect_eduroam() {
  Serial.print("connecting to ");
  Serial.print(WIFI_SSID);
  Serial.print(" with identity ");
  Serial.print(EAP_IDENTITY);
  Serial.print(": ");
  WiFi.disconnect(true);  //disconnect from wifi to set new wifi connection
  WiFi.mode(WIFI_STA);    //init wifi mode

  WiFi.begin(WIFI_SSID, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);

  int i = 0;
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
}

void prayer_times() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("http://nasif.ca/prayer-times");
    int resp_code = http.GET();

    if (resp_code > 0) {
      String response = http.getString();
      Serial.println(response);

    }
    else {
      Serial.print("error in GET request: ");
      Serial.println(resp_code);
    }

    http.end();  // Free resources
  }
}

void prayer_times2() {
  HTTPClient http;
  http.begin("http://nasif.ca/prayer-times?month=10&day=23");
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      fajr = String(doc["fajr"].as<const char*>());
      sunrise = String(doc["sunrise"].as<const char*>());
      dhuhr = String(doc["dhuhr"].as<const char*>());
      asr = String(doc["asr"].as<const char*>());
      maghrib = String(doc["maghrib"].as<const char*>());
      isha = String(doc["isha"].as<const char*>());

      Serial.println("fajr: " + fajr);
      Serial.println("sunrise: " + sunrise);
      Serial.println("dhuhr: " + dhuhr);
      Serial.println("asr: " + asr);
      Serial.println("maghrib: " + maghrib);
      Serial.println("isha: " + isha);

      display1.displayTime(fajr.substring(0, 2).toInt(), fajr.substring(3, 5).toInt(), 1);
      display2.displayTime(sunrise.substring(0, 2).toInt(), sunrise.substring(3, 5).toInt(), 1);
      display3.displayTime(dhuhr.substring(0, 2).toInt(), dhuhr.substring(3, 5).toInt(), 1);
      display4.displayTime(asr.substring(0, 2).toInt(), asr.substring(3, 5).toInt(), 1);
      display5.displayTime(maghrib.substring(0, 2).toInt(), maghrib.substring(3, 5).toInt(), 1);
      display6.displayTime(isha.substring(0, 2).toInt(), isha.substring(3, 5).toInt(), 1);
    }
    else {
      Serial.println("failed to parse json: " + String(error.c_str()));
    }
  }
  else {
    Serial.println("http get failed: " + String(httpCode));
  }

  http.end();
}


void setup() {
  Serial.begin(9600);
  delay(10);
  // TM1637
  display1.begin(CLK1, DIO1, 4);
  display2.begin(CLK2, DIO2, 4);
  display3.begin(CLK3, DIO3, 4);
  display4.begin(CLK4, DIO4, 4);
  display5.begin(CLK5, DIO5, 4);
  display6.begin(CLK6, DIO6, 4);

  display1.setBrightness(7);
  display2.setBrightness(7);
  display3.setBrightness(7);
  display4.setBrightness(7);
  display5.setBrightness(7);
  display6.setBrightness(7);

  // fetchPrayerTimes();
  display1.displayTime(6, 18, 1);
  display2.displayTime(8, 15, 1);
  display3.displayTime(1, 19, 1);
  display4.displayTime(3, 49, 1);
  display5.displayTime(6, 22, 1);
  display6.displayTime(8, 13, 1);

  // MAX7219
  max72.begin(2);
  max72.setInvert(false);
  delay(100);
  max72.setZone(0, MAX_DEVICES - 4, MAX_DEVICES - 1);
  max72.setZone(1, MAX_DEVICES - 4, MAX_DEVICES - 1);

  // wifi
  #ifdef SIMULATION
    WiFi.begin("Wokwi-GUEST");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }
  #else
  connect_eduroam();
  #endif

}

// void loop() {
//   if (max72.displayAnimate()) {
//     unsigned long t2 = millis();
//     if (t2 - t1 >= interval) {
//       t1 = t2;

//       if (toggle) {
//         max72.displayZoneText(1, "12:45", PA_CENTER, 50, 5000, PA_WIPE_CURSOR);
//       }
//       else {
//         max72.displayZoneText(0, "Oct 23", PA_CENTER, 50, 5000, PA_WIPE_CURSOR);
//       }
//       toggle = !toggle;
//     }
//   }
// }

void loop() {
  
  if (max72.displayAnimate()) {
    unsigned long t2 = millis();
    if (t2 - t1 >= interval) {
      t1 = t2;
      prayer_times();

      if (toggle) {
        max72.displayZoneText(1, "12:45", PA_CENTER, 50, 5000, PA_PRINT);
      }
      else {
        max72.displayZoneText(0, "Oct 23", PA_CENTER, 50, 5000, PA_PRINT);
      }
      toggle = !toggle;
    }
  }
}


// // broken code 
// void loop() {
//   if (max72.displayAnimate()) {
//     unsigned long t2 = millis();
//     if (t2 - t1 >= interval) {
//       t1 = t2;
//       ntp.forceUpdate()
//       int hour = ntp.getHours();
//       int minute = ntp.getMinutes();
//       int second = ntp.getSeconds();
//       Serial.println(hour);
//       Serial.println(minute);

//       time_t epochTime = ntp.getEpochTime();
//       struct tm *tm = localtime(&epochTime);

//       char date_str[10];
//       strftime(date_str, sizeof(date_str), "%b %d", tm);

//       if (toggle) {
//         String time = String(hour) + ":" + String(minute);
//         max72.displayZoneText(1, time.c_str(), PA_CENTER, 50, 5000, PA_PRINT);
//       }
//       else {
//         max72.displayZoneText(0, date_str, PA_CENTER, 50, 5000, PA_PRINT);
//       }
//       toggle = !toggle;
//     }
//   }
// }
