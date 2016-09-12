#include "FastLED.h"
#include "TimeLib.h"
#include "ESP8266WiFi.h"
#include "WiFiUdp.h"

#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define NUM_LEDS      17
#define DATA_PIN      2
#define TIME_HEADER   "T"
#define LED_TYPE      WS2812B
#define COLOR_ORDER   GRB

CRGB leds[NUM_LEDS];

IPAddress timeServerIP;
WiFiUDP udp;
const char* ntpServerName = "de.pool.ntp.org";
const char ssid[] = "";
const char pass[] = "";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
unsigned int localPort = 2390;

void setup() {
  const unsigned long DEFAULT_TIME = 1470161384;
  
  Serial.begin(9600);
  
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  setTime(DEFAULT_TIME);
  syncNTPtime();
}

void loop() {
  String hourString = String(hour(), BIN);
  String minuteString = String(minute(), BIN);
  String secondString = String(second(), BIN);

  hourString = zeroPadLeft(hourString, 5);
  minuteString = zeroPadLeft(minuteString, 6);
  secondString = zeroPadLeft(secondString, 6);
  
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());
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
  
  for (int i = 0; i <= 5; i++) {
    char c = minuteString.charAt(i);
    if (c == '1') {
      leds[i+5] = CRGB::Green;
    } else {
      leds[i+5] = CRGB::Black;
    }
  }

  for (int i = 0; i <= 5; i++) {
    char c = secondString.charAt(i);
    if (c == '1') {
      leds[i+11] = CRGB::Orange;
    } else {
      leds[i+11] = CRGB::Black;
    }
  }

  FastLED.show();
  FastLED.delay(1000);
}

void syncNTPtime() {
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println(WiFi.localIP());

  udp.begin(localPort);
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP);

  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);


    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second

    setTime(epoch);
    adjustTime(7200);
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

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
