#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <WebSocketServer.h>
#include "FastLED.h"

//MACROS
#define DEVELOPMENT_LOCATION 0 //0=strathfield home, 1=your chemist shop Randwick
#define WIFI_TYPE 1//0 = Local WiFi Websockets, 1 = Access Point Websockets
#define SERIAL_PRINT 1 //0 = disable ALL serial printing, 1 = enable serial printing
#define SERIAL_PRINT_DEBUG 1 //0 = disable excessive serial printing, enable excessive serial Printing

#define NEO_PIN 26
#define NEO_NB 80
#define RED_LED 0
#define GRN_LED 2
#define BLU_LED 4
#define COL_NB 3

#if WIFI_TYPE == 0
#if DEVELOPMENT_LOCATION == 0
const char* ssid     = "TeamEuxnix2016DS";
const char* password = "2714thecrescent21352016";
#elif DEVELOPMENT_LOCATION == 1
const char* ssid     = "Chemist_Shop";
const char* password = "High_66_Randwick";
#endif
#elif WIFI_TYPE == 1
const char *ssid = "MyESP32AP";
const char *password = "testpassword";
#endif

uint8_t neoPixelMatrix[NEO_NB][COL_NB] = {{0}};

static int colourRGB = 0;
CRGB leds[NEO_NB];

#if WIFI_TYPE == 0
AsyncWebServer wifiserver(80);
AsyncWebSocket ws("/test");

#elif WIFI_TYPE == 1
WiFiServer wifiserver(80);
WebSocketServer webSocketServer;
#endif

#if WIFI_TYPE == 0
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {

  if (type == WS_EVT_CONNECT) {
#if SERIAL_PRINT == 1
    Serial.println("Websocket client connection received");
#endif
  } else if (type == WS_EVT_DISCONNECT) {
#if SERIAL_PRINT == 1
    Serial.println("Client disconnected");
#endif
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
      leds[i].red = (uint8_t) data[i * 3 + 2];
      leds[i].green = (uint8_t) data[i * 3 + 1];
      leds[i].blue = (uint8_t) data[i * 3 + 0];
    }
    FastLED.show();
#if SERIAL_PRINT == 1 && SERIAL_PRINT_DEBUG == 1
    Serial.println(millis());
#endif

  }
}
#elif WIFI_TYPE == 1
#endif


void setup() {
#if SERIAL_PRINT == 1
  Serial.begin(115200);
#endif
  delay(1000);


#if WIFI_TYPE == 0
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
#if SERIAL_PRINT == 1
    Serial.println("Connecting to WiFi..");
    Serial.println(ssid);
    Serial.println(password);
#endif

  }
#if SERIAL_PRINT == 1
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
#endif
  ws.onEvent(onWsEvent);
  wifiserver.addHandler(&ws);
  wifiserver.begin();

#elif WIFI_TYPE == 1
  WiFi.softAP(ssid, password);
#if SERIAL_PRINT == 1
  Serial.println(WiFi.softAPIP());
#endif


  wifiserver.begin();
#endif


  //Initialise all LED pins as outputs and off
  pinMode(RED_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(GRN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLU_LED, LOW);
  digitalWrite(GRN_LED, LOW);

  //Initialise Neopixels
  FastLED.addLeds<NEOPIXEL, NEO_PIN>(leds, NEO_NB);  // GRB ordering is assumed
  FastLED.setBrightness( 50 );
}

void loop() {
#if WIFI_TYPE == 1
  WiFiClient client = wifiserver.available();

  if (client.connected() && webSocketServer.handshake(client)) {

    String data;

    while (client.connected()) {

      data = webSocketServer.getData();

      if (data.length() > 0) {
        //        Serial.println(data);
        //        webSocketServer.sendData(data);
        //        for (int i = 0; i < NEO_NB; i++) {
        //          Serial.print((uint8_t)data[i]);
        //          Serial.print(" ");

        for (int i = 0; i < NEO_NB; i++) {
          leds[i].red = (uint8_t) data[i * 3 + 2];
          leds[i].green = (uint8_t) data[i * 3 + 1];
          leds[i].blue = (uint8_t) data[i * 3 + 0];
        }
        FastLED.show();
        //        }
#if SERIAL_PRINT == 1 && SERIAL_PRINT_DEBUG == 1
        Serial.println(millis());
#endif

      }

      delay(10); // Delay needed for receiving the data correctly
    }
#if SERIAL_PRINT == 1
    Serial.println("The client disconnected");
#endif
    delay(100);
  }
  delay(100);
#endif
}
