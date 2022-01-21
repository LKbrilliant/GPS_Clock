/*
    Code By:  LKBrilliant
    Date:     19.01.2022

      __a__
   f |     | b
     |__g__|
   e |     | c
     |_____|
        d

   Initial time set : 2022/01/19 - 15:00
*/

#include "RTClib.h"

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define digitDelay  5
#define ani_delay   500
#define dots_pin    2
#define S_RX        A0
#define S_TX        A1

#define stop_h    0
#define stop_m    5


TinyGPSPlus gps;

SoftwareSerial SoftSerial(S_RX, S_TX);

RTC_DS1307 rtc;

byte tens, ones;
int Y = 0, M = 0, D = 0, h = 0, m = 0, s = 0;
unsigned long t = 0;
boolean debug = true;
boolean timeUpdated = false;

// {a, b, c, d, e, f, g}
const byte seg_pin[] = {9, 8, 7, 6, 5, 4, 3};

const byte digitPin[] = {10, 11, 12, 13};
const boolean seg[10][7] = {  // Active low pins
  //{ a, b, c, d, e, f, g }
  { 0, 0, 0, 0, 0, 0, 1 }, // Zero
  { 1, 0, 0, 1, 1, 1, 1 }, // One
  { 0, 0, 1, 0, 0, 1, 0 }, // Two
  { 0, 0, 0, 0, 1, 1, 0 }, // Three
  { 1, 0, 0, 1, 1, 0, 0 }, // Four
  { 0, 1, 0, 0, 1, 0, 0 }, // Five
  { 0, 1, 0, 0, 0, 0, 0 }, // Six
  { 0, 0, 0, 1, 1, 1, 1 }, // Seven
  { 0, 0, 0, 0, 0, 0, 0 }, // Eight
  { 0, 0, 0, 1, 1, 0, 0 }  // Nine
};

void setup() {
  if (debug) Serial.begin(115200);

  for (byte i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }
  for (byte i = 0; i <= 3; i++) {
    digitalWrite(digitPin[i], HIGH);
  }

  if (! rtc.begin()) {
    if (debug) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
    }
    while (1) delay(10);
  }
  initial_display();
}

void loop() {
  DateTime now = rtc.now();
  if (now.hour() == stop_h && now.minute() == stop_m && !timeUpdated) {
    initial_display();
    SoftSerial.begin(9600);
    getGPS();                 // Loop untill it gets the time data
    SoftSerial.end();
    if (debug) {
      Serial.print(h);
      Serial.print(":");
      Serial.println(m);
    }
    rtc.adjust(DateTime(Y, M, D, h, m, s));
    if (debug) Serial.println("Time Updated");
    timeUpdated = true;
  }
  if (timeUpdated && now.minute() > stop_m + 10) timeUpdated = false;

  hour(now.hour());
  minute(now.minute());

  if (millis() - t >= 1000) {
    digitalWrite(dots_pin, !digitalRead(dots_pin));
    t = millis();
  }
}

void hour(byte num) {
  if (num < 10) {
    tens =  0;
    ones = num;
  }
  else {
    tens = num / 10;
    ones = num % 10;
  }
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  for (byte i = 0; i <= 7; i++) {
    digitalWrite(seg_pin[i], seg[tens][i]);
  }
  delay(digitDelay);

  digitalWrite(10, HIGH);
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  for (byte i = 0; i <= 7; i++) {
    digitalWrite(seg_pin[i], seg[ones][i]);
  }
  delay(digitDelay);
}

void minute(byte num) {
  if (num < 10) {
    tens =  0;
    ones = num;
  }
  else {
    tens = num / 10;
    ones = num % 10;
  }
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  for (byte i = 0; i <= 7; i++) {
    digitalWrite(seg_pin[i], seg[tens][i]);
  }
  delay(digitDelay);

  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  for (byte i = 0; i <= 7; i++) {
    digitalWrite(seg_pin[i], seg[ones][i]);
  }
  delay(digitDelay);
}

void getGPS() {
  unsigned long startTime = millis();
  while (h == 0 && m == 0) {
    if (gps.encode(SoftSerial.read())) {
      if (gps.date.isValid()) {
        Y = gps.date.year();
        M = gps.date.month();
        D = gps.date.day();

      }
      else {
        if (debug) Serial.print("Date INVALID");
      }
      if (gps.time.isValid()) {  //    Get time and compensate for GMT+5:30
        h = gps.time.hour() + 5;
        m = gps.time.minute() + 30;
        s = gps.time.second();
        if (m >= 60) {
          m = m - 60;
          h = h + 1;
        }
        if (h >= 24) h = h - 24;
      }
      else {
        if (debug) Serial.print("Time INVALID");
      }
      if (debug) {
        if (gps.satellites.isValid()) {
          Serial.print("Satellites = ");
          Serial.println(gps.satellites.value());
        }
        else  Serial.println("Satellites Invalid");
      }
      if ((millis() - startTime) >= 300000) break;    // Timeout
    }
    if (debug) {
      Serial.print(Y);
      Serial.print("/");
      Serial.print(M);
      Serial.print("/");
      Serial.print(D);
      Serial.print("--");
      if (h < 10) Serial.print("0");
      Serial.print(h);
      Serial.print(":");
      if (m < 10) Serial.print("0");
      Serial.println(m);
    }
  }
}

void initial_display() {
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  const boolean s[] = { 1, 1, 1, 1, 1, 1, 0 };
  for (byte i = 0; i <= 7; i++) {
    digitalWrite(seg_pin[i], s[i]);
  }
}
