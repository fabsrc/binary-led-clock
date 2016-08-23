#include "FastLED.h"
#include "TimeLib.h"

#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define NUM_LEDS      12
#define DATA_PIN      2
#define TIME_HEADER   "T"
#define LED_TYPE      WS2812B
#define COLOR_ORDER   GRB

CRGB leds[NUM_LEDS];

void setup() {
  const unsigned long DEFAULT_TIME = 1470161384;
  
  Serial.begin(9600);
  
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  setTime(DEFAULT_TIME);
}

void loop() {
  if (Serial.available()) {
    processSyncMessage();

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Red;
    }
    FastLED.show();
  } else {
    String hourString = String(hour(), BIN);
    String minuteString = String(minute(), BIN);
  
    hourString = zeroPadLeft(hourString, 5);
    minuteString = zeroPadLeft(minuteString, 6);
    
    Serial.print(hour());
    Serial.print(":");
    Serial.print(minute());
    Serial.println();
//    Serial.print("(hs " + hourString + "):");
//    Serial.print("(ms" + minuteString + ")");
//    Serial.println();
    for (int i = 0; i <= 4; i++) {
      char c = hourString.charAt(i);
      if (c == '1') {
        leds[i] = CRGB::Red; 
      } else {
        leds[i] = CRGB::Black;
      }
    }
  
    leds[5] = CRGB::Orange;
    FastLED.show();
    FastLED.delay(500);
    
    for (int i = 0; i <= 5; i++) {
      char c = minuteString.charAt(i);
      if (c == '1') {
        leds[i+6] = CRGB::Green;
      } else {
        leds[i+6] = CRGB::Black;
      }
    }
    
    leds[5] = CRGB::Black;
    FastLED.show();
    FastLED.delay(500);
  }
}

void processSyncMessage() {
  unsigned long pctime;

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    setTime(pctime);
    Serial.print("Set Time");
  }
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
