#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;

#define S_RX    4
#define S_TX    3

SoftwareSerial SoftSerial(S_RX, S_TX);

void setup(void) {
  Serial.begin(9600);
  SoftSerial.begin(9600);
}

void loop() {

  while (SoftSerial.available() > 0) {

    if (gps.encode(SoftSerial.read())) {
      if (gps.date.isValid()) {
        int M = gps.date.month();
        int D = gps.date.day();
        int Y = gps.date.year();
      }
      else
      {
        Serial.print(F("INVALID"));
      }

      if (gps.time.isValid()) {
        Serial.print("Time (GMT+5:30) : ");
        int s = gps.time.second();
        int m = gps.time.minute() + 30;
        int h = gps.time.hour() + 5;

        if (m >= 60) {
          m = m - 60;
          h = h + 1;
        }
        if (h >= 24) h = h - 24;

        if (h < 10)     Serial.print("0");
        Serial.print(h);
        Serial.print(":");

        if (m < 10)   Serial.print("0");
        Serial.print(m);
        Serial.print(":");
        if (s < 10)   Serial.print("0");
        Serial.println(s);

        if (gps.satellites.isValid()) {
          Serial.print("Satellites = ");
          Serial.println(gps.satellites.value());
        }
        else  Serial.println("Satellites Invalid");
      }
    }
  }
