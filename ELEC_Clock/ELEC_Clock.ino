/*
 *  Code By:  LKBrilliant
 *  Date:     19.01.2022
 *  
 *    __a__
 * f |     | b
 *   |__g__|
 * e |     | c
 *   |_____|
 *      d
 *      
 * Initial time set : 2022/01/19 - 15:00
*/

#include "RTClib.h"

RTC_DS1307 rtc;

#define digitDelay  5
#define dots_pin    2

byte tens, ones;
unsigned long t = 0;

// {a, b, c, d, e, f, g}
const byte seg_pin[] = {9, 8, 7, 6, 5, 4, 3};

const byte digitPin[] = { 10, 11, 12, 13};
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
  for (byte i=2; i <= 13; i++){
      pinMode(i, OUTPUT);
  }
  for (byte i=0; i <= 3; i++){
      digitalWrite(digitPin[i], HIGH);
  }
  Serial.begin(57600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
    
    t = millis;
  }
}

void loop() {
  DateTime now = rtc.now();
  hour(now.hour());  
  minute(now.minute());
  
  if (millis() - t >= 1000){
    digitalWrite(dots_pin, !digitalRead(dots_pin));
    t = millis();
  }
}

void hour(byte num){
  if (num < 10){
    tens =  0;
    ones = num;
  }
  else {
    tens = num/10;
    ones = num % 10;
  }
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  for (byte i=0; i <= 7; i++){
    digitalWrite(seg_pin[i], seg[tens][i]);
  }
  delay(digitDelay);

  digitalWrite(10, HIGH);
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
    for (byte i=0; i <= 7; i++){
    digitalWrite(seg_pin[i], seg[ones][i]);
  }
  delay(digitDelay);
}

void minute(byte num){
  if (num < 10){
    tens =  0;
    ones = num;
  }
  else {
    tens = num/10;
    ones = num % 10;
  }
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  for (byte i=0; i <= 7; i++){
    digitalWrite(seg_pin[i], seg[tens][i]);
  }
  delay(digitDelay);

  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
    for (byte i=0; i <= 7; i++){
    digitalWrite(seg_pin[i], seg[ones][i]);
  }
  delay(digitDelay);
}
