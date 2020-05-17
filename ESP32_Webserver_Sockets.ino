//MACROS
#define SERIAL_PRINT 0 //0 = disable ALL serial printing, 1 = enable serial printing
#define SERIAL_PRINT_DEBUG 0 //0 = disable excessive serial printing, enable excessive serial Printing
#define LEDSTRING_NB 8 //0 = single LED string, otherwise value equals number of strings

#include "WiFi.h"
#include "FastLED.h"
#include <WebSocketServer.h>

#if LEDSTRING_NB == 0
#define NEO_PIN 18
#define NEO_NB 80
#else
#define NEO_STRING_NB 208//64 x 26 matrix, divided by 8 strings = 208
#define NEO_PIN0 12
#define NEO_PIN1 2
#define NEO_PIN2 0
#define NEO_PIN3 5
#define NEO_PIN4 18
#define NEO_PIN5 19
#define NEO_PIN6 21
#define NEO_PIN7 22
#endif

#define RED_LED 0
#define GRN_LED 2
#define BLU_LED 4
#define WDT 2000 //watch dog timer timeout in milliseconds


const char *ssid = "MyESP32AP";
const char *password = "testpassword";

#if LEDSTRING_NB == 0
CRGB leds[NEO_NB];
#else
CRGB leds[LEDSTRING_NB][NEO_STRING_NB];
#endif

unsigned long pseudoWDT = 0; //2s watchdog timer

WiFiServer wifiserver(80);
WebSocketServer webSocketServer;

void setup() {

  //Initialise all LED pins as outputs and off
  pinMode(RED_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(GRN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLU_LED, LOW);
  digitalWrite(GRN_LED, LOW);

#if SERIAL_PRINT == 1
  Serial.begin(115200);
#endif
  delay(1000);

#if LEDSTRING_NB == 0
  //Initialise Neopixels
  FastLED.addLeds<NEOPIXEL, NEO_PIN>(leds, NEO_NB);  // GRB ordering is assumed
  FastLED.setBrightness( 50 );
#else

  FastLED.addLeds<NEOPIXEL, NEO_PIN0>(leds[0], NEO_STRING_NB);  // GRB ordering is assumed
  FastLED.addLeds<NEOPIXEL, NEO_PIN1>(leds[1], NEO_STRING_NB);  // GRB ordering is assumed
  FastLED.addLeds<NEOPIXEL, NEO_PIN2>(leds[2], NEO_STRING_NB);  // GRB ordering is assumed
  FastLED.addLeds<NEOPIXEL, NEO_PIN3>(leds[3], NEO_STRING_NB);  // GRB ordering is assumed
  FastLED.addLeds<NEOPIXEL, NEO_PIN4>(leds[4], NEO_STRING_NB);  // GRB ordering is assumed
  FastLED.addLeds<NEOPIXEL, NEO_PIN5>(leds[5], NEO_STRING_NB);  // GRB ordering is assumed
  FastLED.addLeds<NEOPIXEL, NEO_PIN6>(leds[6], NEO_STRING_NB);  // GRB ordering is assumed
  FastLED.addLeds<NEOPIXEL, NEO_PIN7>(leds[7], NEO_STRING_NB);  // GRB ordering is assumed
  FastLED.setBrightness( 255 );
#endif



#if LEDSTRING_NB == 0
  //Initialise a red LED to indicate we are connecting
  leds[0].red = 255;
  leds[0].green = 0;
  leds[0].blue = 0;
  FastLED.show();
#else
for(int i=0; i<LEDSTRING_NB; i++)
{
  //Initialise a red LED to indicate we are connecting
  leds[i][0].red = 255;
  leds[i][0].green = 0;
  leds[i][0].blue = 0;
}
  FastLED.show();
#endif

  WiFi.softAP(ssid, password);

#if LEDSTRING_NB == 0
  //Once connected change to a blue LED
  leds[0].red = 0;
  leds[0].green = 0;
  leds[0].blue = 255;
  FastLED.show();
  #else
for(int i=0; i<LEDSTRING_NB; i++)
{
  //Initialise a red LED to indicate we are connecting
  leds[i][0].red = 0;
  leds[i][0].green = 0;
  leds[i][0].blue = 255;
}
  FastLED.show();
#endif

#if SERIAL_PRINT == 1
  Serial.println(WiFi.softAPIP());
#endif


}

void loop() {
  static int firstConnect = 0;
  wifiserver.begin();
  
#if SERIAL_PRINT == 1
  Serial.println("WebServer begin");
#endif
  WiFiClient client = wifiserver.available();

  if (client.connected() && webSocketServer.handshake(client)) {
    String data;

    while (client.connected()) {

      data = webSocketServer.getData();

      if (data.length() > 0) {
        //the ESP32 has connected once now and sent data
        firstConnect = 1;
        //kick watchdog timer
        pseudoWDT = millis() + WDT;
#if SERIAL_PRINT_DEBUG == 1
        Serial.println("WDT kicked");
        Serial.println(data.length());
#endif
        //        Serial.println(data);
        //        webSocketServer.sendData(data);
        //        for (int i = 0; i < NEO_NB; i++) {
        //          Serial.print((uint8_t)data[i]);
        //          Serial.print(" ");
#if LEDSTRING_NB == 0
        for (int i = 0; i < NEO_NB; i++) {
          leds[i].red = (uint8_t) data[i * 3 + 2];
          leds[i].green = (uint8_t) data[i * 3 + 1];
          leds[i].blue = (uint8_t) data[i * 3 + 0];
        }
#else
        for (int i = 0; i < LEDSTRING_NB; i++) {
          for (int x = 0; x < NEO_STRING_NB; x++) {
            leds[i][x].red = (uint8_t) data[(i*NEO_STRING_NB + x) * 3 + 2];
            leds[i][x].green = (uint8_t) data[(i*NEO_STRING_NB + x) * 3 + 1];
            leds[i][x].blue = (uint8_t) data[(i*NEO_STRING_NB + x) * 3 + 0];
          }
        }
#endif
        FastLED.show();
        //        }
#if SERIAL_PRINT == 1 && SERIAL_PRINT_DEBUG == 1
        Serial.println(millis());
#endif
      }
      else
      {
#if SERIAL_PRINT == 1 && SERIAL_PRINT_DEBUG == 1
        Serial.println("data.length <= 0");
#endif
      }
      delay(10); // Delay needed for receiving the data correctly
      //If the current millis is greater than what the watchdog timer is set to, restart ESP32
      if ((millis() >= pseudoWDT) && (firstConnect == 1))
      {
#if SERIAL_PRINT == 1
        Serial.println("ESP32 Restarted 3");
#endif
        ESP.restart();
      }
    }
#if SERIAL_PRINT == 1
    Serial.println("The client disconnected");
#endif
    delay(100);
    //If the current millis is greater than what the watchdog timer is set to, restart ESP32
    if ((millis() >= pseudoWDT) && (firstConnect == 1))
    {
#if SERIAL_PRINT == 1
      Serial.println("ESP32 Restarted 2");
#endif
      ESP.restart();

    }
  }
#if SERIAL_PRINT == 1
  Serial.println("The client disconnected and the handshake with client is no good?");
#endif
  delay(100);

  //If the current millis is greater than what the watchdog timer is set to, restart ESP32
  if ((millis() >= pseudoWDT) && (firstConnect == 1))
  {
#if SERIAL_PRINT == 1
    Serial.println("ESP32 Restarted 1");
#endif
    ESP.restart();

  }
}
