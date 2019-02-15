/*
 *  Magic Crystal to alert you
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>

#define PIN 14
#define NUM_LEDS 7
#define BRIGHTNESS 100
#define RED 0
#define ORANGE 1
#define YELLOW 2
#define GREEN 3
#define BLUE 4
#define INDIGO 5
#define VIOLET 6
#define PINK 7
#define WHITE 8
#define OFF 9 


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

const char* ssid     = "SSID";
const char* password = "password";

const char* mercuryHost = "mercuryretrogradeapi.com";
const char* moonHost = "isitfullmoon.com";

const int httpPort = 80;
const int httpsPort = 443;

// specific to the mecuryretrogradeapi.com cert 
const char fingerprint[] PROGMEM = "4a 36 25 b9 1f 4c 6d 61 c4 6f d3 8d 65 70 21 96 b0 05 13 0f";

uint32_t COLORS[] = {
  strip.Color(255, 0, 0),    // red 
  strip.Color(255, 165, 0),  // orange
  strip.Color(255, 255, 0),  // yellow
  strip.Color(0, 255, 0),    // green
  strip.Color(0, 0, 255),    // blue
  strip.Color(75, 0, 130),   // indigo
  strip.Color(128, 0, 128),  // violet
  strip.Color(255, 192, 203),// pink
  strip.Color(255, 255, 255),// white
  strip.Color(0, 0, 0)       // off
};

void setup() {
  Serial.begin(115200);
  delay(100);
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  if (isMercuryRetrograde()) {
    set_color_to(RED);
  } else if (isMoonFull()) {
    set_color_to(WHITE);
  } else {
    set_color_to(OFF);
  }

  delay(1000000);  
}

bool isMoonFull() {
  WiFiClient client;
  String url = "/api.php";
  
  if (!client.connect(moonHost, httpPort)) {
    // connection failed
    return false;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + moonHost + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
  
  String response = "";
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    response += line;
  }

  Serial.println(response);
 
  if(response.indexOf("\"status\":\"No\"") >= 0){
    return false;
  } else {
    return true;  
  }
}

bool isMercuryRetrograde() {
  WiFiClientSecure client;
  String url = "/";

  client.setFingerprint(fingerprint);
    if (!client.connect(mercuryHost, httpsPort)) {
    // connection failed
    return false;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + mercuryHost + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);
  
  String response = "";
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    response += line;
  }

  Serial.println(response);

  if(response.indexOf("{\"is_retrograde\":false}") >= 0){
    return false;
  } else {
    return true;
  }
}

// Set all the LEDs to the color specified by the index in the color array
void set_color_to(int color_idx) {
  strip.fill(COLORS[color_idx]);
  strip.show();
}
