/**
* lagspacebutton.ino
*
*  Created on: 29.01.2016
*
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

const int ledr = 14;
const int ledg = 13;
const int ledb = 12;
const int button = 5;
const int relay = 16;

int state;
int old_state = 0;

// give me your wifi bro
const char *ssid = "LAG";
const char *password = "somethingeasytoremember";

void setup() {

    USE_SERIAL.begin(115200);
    Serial.println();
    Serial.print("connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

   // really disable spamming wifi ;(
    WiFi.mode(WIFI_STA);
    WiFi.softAPdisconnect(true);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // setup pins
    pinMode(ledr, OUTPUT);
    pinMode(ledg, OUTPUT);
    pinMode(ledb, OUTPUT);
    pinMode(button, INPUT_PULLUP);
    pinMode(relay, OUTPUT);

}

void loop() {
   old_state = state;
   state = digitalRead(button);
   HTTPClient http2;

   if(old_state != state){
     if(state == 1){
         http2.begin("state.lag", 80, "/?spacebutton=closespace");
         http2.GET();
         USE_SERIAL.print("New button state and closing\n");
       }
     else if(state == 0){
         http2.begin("state.lag", 80, "/?spacebutton=openspace");
         http2.GET();
         USE_SERIAL.print("New button state and opening\n");
       }
   } else{
       USE_SERIAL.print("Nothing changed\n");
   }

   HTTPClient http;

   USE_SERIAL.print("[HTTP] begin...\n");
   // configure traged server and url
   http.begin("state.laglab.org", 80, "/botstate"); //HTTP

   USE_SERIAL.print("[HTTP] GET...\n");
   // start connection and send HTTP header
   int httpCode = http.GET();
   if(httpCode) {
       // HTTP header has been send and Server response header has been handled
       USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

       // file found at server
       if(httpCode == 200) {
           String payload = http.getString();
           USE_SERIAL.println(payload);
           if(payload == "closed"){
             analogWrite(ledr, 255);
             analogWrite(ledg, 0);
             analogWrite(ledb, 0);
             digitalWrite(relay, LOW);
             }
           else if(payload == "open"){
             analogWrite(ledr, 0);
             analogWrite(ledg, 255);
             analogWrite(ledb, 100);
             digitalWrite(relay, HIGH);
             }
       }
   } else {
       USE_SERIAL.print("[HTTP] GET... failed, no connection or no HTTP server\n");
   }


delay(3000);
}
