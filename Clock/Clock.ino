/* ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄    ▄▄▄▄▄▄▄ ▄▄▄     ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ ▄▄▄   ▄
  █       █       █       █  █       █   █   █       █       █   █ █ █
  █   ▄▄▄▄█    ▄  █  ▄▄▄▄▄█  █       █   █   █   ▄   █       █   █▄█ █
  █  █  ▄▄█   █▄█ █ █▄▄▄▄▄   █     ▄▄█   █   █  █ █  █     ▄▄█      ▄█
  █  █ █  █    ▄▄▄█▄▄▄▄▄  █  █    █  █   █▄▄▄█  █▄█  █    █  █     █▄
  █  █▄▄█ █   █    ▄▄▄▄▄█ █  █    █▄▄█       █       █    █▄▄█    ▄  █
  █▄▄▄▄▄▄▄█▄▄▄█   █▄▄▄▄▄▄▄█  █▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█▄▄▄█ █▄█

   Project:     GPS CLOCK
   version:     1.0.0
   Institution: University of Kelaniya
   Code By:     Anuradha Gunawardhana
   Date:        12.02.2022
      __a__
   f |     | b
     |__g__|
   e |     | c
     |_____|
        d
  --------------------------------------------------------------------------------------
  Libraries: RTCLib(v2.0.2) by Adafruit: https://github.com/adafruit/RTClib
             TinyGPS++ (v1.0.2) by mikalhart : https://github.com/mikalhart/TinyGPSPlus
  --------------------------------------------------------------------------------------
  MIT License

  Copyright (c) 2022 Anuradha Gunawardhana

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "RTClib.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define digitDelay  5         // Digit show time in miliseconds
// #define ani_delay   500
#define dots_pin    2         // Hours minutes separate dots 
#define S_RX        A0
#define S_TX        A2

// Updating the time at stop_h hours and stop_m minutes every day
#define stop_h    12
#define stop_m    38


TinyGPSPlus gps;
RTC_DS1307 rtc;
SoftwareSerial SoftSerial(S_RX, S_TX);

byte tens, ones;
int Y = 0, M = 0, D = 0, h = 0, m = 0, s = 0;
unsigned long t = 0;
boolean debug = false;
boolean timeUpdated = false;
boolean gotGPS_time = false;

//                     {a, b, c, d, e, f, g}
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
  pinMode(A1, INPUT);                 // Time update Button with external 10k pullup

  if (! rtc.begin()) {
    if (debug) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
    }
  }
  initial_display();
  delay(1000);
}

void loop() {
  DateTime now = rtc.now();
  if ((now.hour() == stop_h && now.minute() == stop_m && !timeUpdated) || !digitalRead(A1)) {
    initial_display();
    SoftSerial.begin(9600);
    getGPS();                 // Loop until it gets the time data
    SoftSerial.end();
    if (debug) {
      Serial.print(h);
      Serial.print(":");
      Serial.println(m);
    }
    if (gotGPS_time) {
      rtc.adjust(DateTime(Y, M, D, h, m, s));   // Write the new date and time to the RTC
      if (debug) Serial.println("Time Updated");
      timeUpdated = true;
      gotGPS_time = false;
    }
  }
  if (timeUpdated && (now.minute() > (stop_m + 10))) timeUpdated = false; // only allow next time updating after 10m

  hour(now.hour());         // show hours
  minute(now.minute());     // show minutes

  if (millis() - t >= 1000) {             // Blink the middle dots
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
  while (true) {
    if (gps.encode(SoftSerial.read())) {
      if (gps.date.isValid()) {
        Y = gps.date.year();
        M = gps.date.month();
        D = gps.date.day();
      }
      else if (debug) Serial.print("Date INVALID");

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
      else if (debug) Serial.print("Time INVALID");

      if (debug) {
        if (gps.satellites.isValid()) {
          Serial.print("Satellites = ");
          Serial.println(gps.satellites.value());
        }
        else  Serial.println("Satellites Invalid");
      }
      if (h != 5 && m != 30 && h != 0 && m != 0) {
        gotGPS_time = true;
        break;                  // break if gps time received successfully
      }
      if ((millis() - startTime) >= 60000) {
        gotGPS_time = false;
        break;    // Timeout
      }
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