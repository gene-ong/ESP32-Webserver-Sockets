/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#define NEO_PIN 26 
#define NEO_NB 80
#define RED_LED 0
#define GRN_LED 2
#define BLU_LED 4

const char* ssid     = "TeamEuxnix2016DS";
const char* password = "2714thecrescent21352016";


WiFiServer server(80);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEO_NB, NEO_PIN, NEO_GRB + NEO_KHZ800);
int redLEDState = 0;
int bluLEDState = 0;
int grnLEDState = 0;

void setup()
{
//Init for WIFI
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    server.begin();

//Init for Neopixels
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'

  pinMode(RED_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(GRN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLU_LED, LOW);
  digitalWrite(GRN_LED, LOW);
  
}

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("Click <a href=\"/G\">here</a> to toggle the green LED on or off.<br>");
            client.print("Click <a href=\"/R\">here</a> to toggle the red LED on or off.<br>");
            client.print("Click <a href=\"/B\">here</a> to toggle the blue LED on or off.<br>");
            client.println();
            break;

          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
               
        if (currentLine.endsWith("GET /G")) {
          colorWipe(strip.Color(0, 255, 0), 0); // Green
          grnLEDState = !grnLEDState;
          
        }
        if (currentLine.endsWith("GET /R")) {
          colorWipe(strip.Color(255, 0, 0), 0); // Red
          redLEDState = !redLEDState;
        }
         if (currentLine.endsWith("GET /B")) {
          colorWipe(strip.Color(0, 0, 255), 0); // Blue
          bluLEDState = !bluLEDState;
        }
        digitalWrite(GRN_LED, grnLEDState);
        digitalWrite(BLU_LED, bluLEDState);
        digitalWrite(RED_LED, redLEDState);
      }
    }
    // close the connection:
    client.stop();
    //Serial.println("Client Disconnected.");
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
