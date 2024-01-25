#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN D1
#define BUTTON_RED_PIN D2
#define BUTTON_BLACK_PIN D3
#define STRIPSIZE 12

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPSIZE, LED_PIN, NEO_GRB + NEO_KHZ800);

const char *ssid     = "WLAN-NETZWERK";
const char *password = "WLAN-PASSWORT";
const long utcOffsetInSeconds = 3600; //Zeitverschiebung UTC <-> MEZ (Winterzeit) = 3600 Sekunden (1 Stunde) oder Zeitverschiebung UTC <-> MEZ (Sommerzeit) = 7200 Sekunden (2 Stunden)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(BUTTON_RED_PIN, INPUT_PULLUP); // Initialize Red Button
  pinMode(BUTTON_BLACK_PIN, INPUT_PULLUP); // Initialize Black Button
  strip.begin();
  strip.setBrightness(50);  // Lower brightness and save eyeballs!
  strip.show(); // Initialize all pixels to 'off'
  timeClient.begin();
}

void loop() {
  timeClient.update();

  if(!digitalRead(BUTTON_RED_PIN) || !digitalRead(BUTTON_BLACK_PIN)) {
    int start = timeClient.getEpochTime();
    int time = timeClient.getEpochTime();
    int movingPixel = 0;

    for(int i = 0; i <= 3; i++) {
      int time = timeClient.getEpochTime();

      while(timeClient.getEpochTime() <= time + 1440) { //1440 = 24min
        delay(100); //ohne delay bleibt das Programm stehen
        timeClient.update(); //sonst Soft WDT reset

        if(i == 0) {
          timer(255, 255, 255);
          timeClient.update();
        } else if (i == 1) {
          timer(0, 255, 0);
          timeClient.update();
        } else if (i == 2) {
          timer(255, 255, 0);
          timeClient.update();
        } else if (i == 3) {
          timer(255, 0, 0);
          timeClient.update();
        }
      }
      int pause = timeClient.getEpochTime();

      while(timeClient.getEpochTime() <= pause + 300) { //300 = 5min
        delay(100); //ohne delay bleibt das Programm stehen
        colorFill(0, 0, 0);
        timeClient.getEpochTime();
      }
      time = time + 1440;
    }
    end();
  }
}

void end() {
  for(int i = 0; i <= 6; i++) {
    if(i % 2 == 0) {
      colorFill(0, 0, 255);
    } else {
      colorFill(0, 0, 0);
    }
    delay(1000);
  }
}

void colorFill(int r, int g, int b) {
  for(int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(r, g, b));
      strip.show();
  }
}

void timer(int r, int g, int b){ 
  int time = timeClient.getEpochTime();

  for(int j = 0; j <= 12; j++) {
    while(timeClient.getEpochTime() <= time + 120) { //120 = 2min
      delay(100); //ohne delay bleibt das Programm stehen
      timeClient.update();
      strip.setPixelColor(0, strip.Color(r, g, b));
      strip.setPixelColor(j, strip.Color(r, g, b));

      for(int i = 1; i < strip.numPixels(); i++) {
        if(i != j) {
          strip.setPixelColor(i, strip.Color(0, 0, 0));
          strip.show();
        }
      }
    }
    time = time + 120;
  }
}