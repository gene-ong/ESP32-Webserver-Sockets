#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "Adafruit_NeoPixel.h"

#define NEO_PIN 26
#define NEO_NB 80
#define RED_LED 0
#define GRN_LED 2
#define BLU_LED 4
#define COL_NB 3
const char* ssid     = "TeamEuxnix2016DS";
const char* password = "2714thecrescent21352016";

uint8_t neoPixelMatrix[NEO_NB][COL_NB] = {{0}};

// setting PWM properties
const int freq = 5000;
const int redLedChannel = 0;
const int bluLedChannel = 1;
const int grnLedChannel = 2;
const int resolution = 8;
static int colourRGB = 0;
uint8_t BLU;
uint8_t GRN;
uint8_t RED;



AsyncWebServer wifiServer(80);
AsyncWebSocket ws("/test");
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEO_NB, NEO_PIN, NEO_GRB + NEO_KHZ800);

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {

  if (type == WS_EVT_CONNECT) {

//    Serial.println("Websocket client connection received");

  } else if (type == WS_EVT_DISCONNECT) {
//    Serial.println("Client disconnected");

  } else if (type == WS_EVT_DATA) {
    /*
      //DO NOT DELETE Test code to view the string coming over WEbsocket
        Serial.print("len = ");
        Serial.print(len);

        for (int i=0; i < len; i ++ ) {
          Serial.print(" i = ");
          Serial.print(i);
          Serial.print(" data = ");
          Serial.println(data[i]);
        }
    */
    for (int i = 0; i < NEO_NB; i++) {
      strip.setPixelColor(i, (uint8_t) data[i * 3 + 2], (uint8_t) data[i * 3 + 1], (uint8_t) data[i * 3 + 0]);
    }
    strip.show();
  }
}

void setup() {

//  Serial.begin(115200);

  delay(1000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
//    Serial.println("Connecting to WiFi..");
  }

//  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  ws.onEvent(onWsEvent);
  wifiServer.addHandler(&ws);

  wifiServer.begin();

  //Initialise all LED pins as outputs and off
  pinMode(RED_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(GRN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLU_LED, LOW);
  digitalWrite(GRN_LED, LOW);

  //Initialise Neopixels
  strip.begin();
  strip.setBrightness(255);
  strip.show(); // Initialize all pixels to 'off'
  // configure LED PWM functionalitites
  ledcSetup(redLedChannel, freq, resolution);
  ledcSetup(bluLedChannel, freq, resolution);
  ledcSetup(grnLedChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(RED_LED, redLedChannel);
  ledcAttachPin(BLU_LED, bluLedChannel);
  ledcAttachPin(GRN_LED, grnLedChannel);


}

void loop() {
}
