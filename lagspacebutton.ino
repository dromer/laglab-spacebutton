 /**
 * lagspacebutton.ino
 *
 *  Created on: 29.01.2016
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

const int ledr = 14;
const int ledg = 13;
const int ledb = 12;

const int button = 5;
int state;
int old_state = 0;
int space_state = 0;

void setup() {

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("LAG", "somethingeasytoremember");

    pinMode(ledr, OUTPUT);
    pinMode(ledg, OUTPUT);
    pinMode(ledb, OUTPUT);
    //pinMode(button, INPUT);
    pinMode(button, INPUT_PULLUP);

}

void loop() {
  
    old_state = state;
    state = digitalRead(button);
    HTTPClient http2;
    
    USE_SERIAL.print("Old state: ");
    USE_SERIAL.print(old_state);
    USE_SERIAL.print("\n");

    USE_SERIAL.print("State: ");
    USE_SERIAL.print(state);
    USE_SERIAL.print("\n");
       
    if(old_state != state){
      if(state == 0){
          http2.begin("state.lag", 80, "/?spacebutton=closespace");
          int http2Code = http2.GET();
          USE_SERIAL.print("httpcode:\n");
          USE_SERIAL.print(http2Code);
          USE_SERIAL.print("\n");
          USE_SERIAL.print("New button state and closing\n");
        }
      else if(state == 1){
          http2.begin("state.lag", 80, "/?spacebutton=openspace");
          int http2Code = http2.GET();
          USE_SERIAL.print("httpcode:\n");
          USE_SERIAL.print(http2Code);
          USE_SERIAL.print("\n");
          USE_SERIAL.print("New button state and opening\n");
        }
    } else{
        USE_SERIAL.print("Nothing changed\n");
      }

  
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("192.168.1.12", 443, "/test.html", true, "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
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
                  space_state = 0;
                  }
                else if(payload == "open"){
                  analogWrite(ledr, 0);
                  analogWrite(ledg, 255);
                  analogWrite(ledb, 100);
                  space_state = 1;
                  }
            }
            
        } else {
            USE_SERIAL.print("[HTTP] GET... failed, no connection or no HTTP server\n");
        }
    }
   

    delay(3000);
}
