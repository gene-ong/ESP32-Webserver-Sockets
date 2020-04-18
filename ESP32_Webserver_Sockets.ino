#include "WiFi.h"
#include "WebSocketServer.h"
#include "FastLED.h"

#define NEO_PIN 26
#define NEO_NB 80
#define RED_LED 0
#define GRN_LED 2
#define BLU_LED 4
#define COL_NB 3
const char* ssid     = "MyESP32AP";
const char* password = "testpassword";
static int colourRGB = 0;
CRGB leds[NEO_NB];

WiFiServer wifiServer(80);
WebSocketServer webSocketServer;


void setup() {

  Serial.begin(115200);

  delay(1000);

  WiFi.softAP(ssid, password);
  Serial.println(WiFi.softAPIP());
  wifiServer.begin();

  //Initialise all LED pins as outputs and off
  pinMode(RED_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(GRN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLU_LED, LOW);
  digitalWrite(GRN_LED, LOW);

  //Initialise Neopixels
  FastLED.addLeds<NEOPIXEL, NEO_PIN>(leds, NEO_NB);  // GRB ordering is assumed

}

void loop() {
  WiFiClient client = server.available();

  if (client.connected() && webSocketServer.handshake(client)) {

    String data;

    while (client.connected()) {

      data = webSocketServer.getData();

      if (data.length() > 0) {
        Serial.println(data);
        //webSocketServer.sendData(data);
        for (int i = 0; i < NEO_NB; i++) {
          leds[i].red = (uint8_t) data[i * 3 + 2];
          leds[i].green = (uint8_t) data[i * 3 + 1];
          leds[i].blue = (uint8_t) data[i * 3 + 0];
        }
        FastLED.show();
      }

      delay(10); // Delay needed for receiving the data correctly
    }

    Serial.println("The client disconnected");
    delay(100);
  }

  delay(100);

}
