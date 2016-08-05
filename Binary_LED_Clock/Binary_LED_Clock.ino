#include "FastLED.h"
#include "TimeLib.h"

#define NUM_LEDS 12
#define DATA_PIN 3
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void setup() {
  const unsigned long DEFAULT_TIME = 1470168684;
  
//  Serial.begin(9600);
  
  FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  setTime(DEFAULT_TIME);
}

void loop() {
  String hourString = String(hour(), BIN);
  String minuteString = String(minute(), BIN);

  hourString = zeroPadLeft(hourString, 5);
  minuteString = zeroPadLeft(minuteString, 6);
  
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.println();
  Serial.print("(hs " + hourString + "):");
  Serial.print("(ms" + minuteString + ")");
  Serial.println();
  for (int i = 0; i <= 4; i++) {
    char c = hourString.charAt(i);
    if (c == '1') {
      Serial.print(c);
      leds[i] = CRGB::Red; 
    } else {
      Serial.print(0);
      leds[i] = CRGB::Black;
    }
  }

  leds[5] = CRGB::Orange;
  FastLED.show();
  delay(500);
  
  Serial.println();
  
  for (int i = 0; i <= 5; i++) {
    char c = minuteString.charAt(i);
    if (c == '1') {
      Serial.print(c);
      leds[i+6] = CRGB::Green;
    } else {
      Serial.print(0);
      leds[i+6] = CRGB::Black;
    }
  }
  Serial.println();
  
  leds[5] = CRGB::Black;
  FastLED.show();
  delay(500);
}

String zeroPadLeft(String input, int length) {
  if (input.length() < length) {
    int padLength = length - input.length();
    String zeros = "000000";
    return zeros.substring(0, padLength) + input;
  } else {
    return input;
  }
}
