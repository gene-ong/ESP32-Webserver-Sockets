#include "WiFi.h"
#define RED_LED 0
#define GRN_LED 2
#define BLU_LED 4

const char* ssid     = "TeamEuxnix2016DS";
const char* password = "2714thecrescent21352016";

// setting PWM properties
const int freq = 5000;
const int redLedChannel = 0;
const int bluLedChannel = 1;
const int grnLedChannel = 2;
const int resolution = 8;

WiFiServer wifiServer(80);

void processReceivedValue(char command) {

  static int counter;
  Serial.print("counter = ");
  Serial.println(counter);
  switch (counter) {
    case 0: ledcWrite(redLedChannel, command);
      Serial.println(command, DEC);
      Serial.println(command, BIN);
      break;
    case 1: ledcWrite(bluLedChannel, command);
      Serial.println(command, DEC);
      Serial.println(command, BIN);
      break;
    case 2: ledcWrite(grnLedChannel, command);
      Serial.println(command, DEC);
      Serial.println(command, BIN);
      break;
    default: counter = 0;
  }
  counter++;
  if (counter >= 3) {
    counter = 0;
  }

  return;
}

void setup() {

  Serial.begin(115200);

  delay(1000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  wifiServer.begin();

  //Initialise all LED pins as outputs and off
  pinMode(RED_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(GRN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLU_LED, LOW);
  digitalWrite(GRN_LED, LOW);

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

  // // increase the LED brightness
  //  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){
  //    // changing the LED brightness with PWM
  //    ledcWrite(redLedChannel, dutyCycle);
  //    delay(1);
  //  }
  //
  //  // decrease the LED brightness
  //  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
  //    // changing the LED brightness with PWM
  //    ledcWrite(redLedChannel, dutyCycle);
  //    delay(1);
  //  }

  WiFiClient client = wifiServer.available();

  if (client) {

    while (client.connected()) {

      while (client.available() > 0) {
        char c = client.read();
        if ((c != '\r') && (c != 10)) { // if you got anything else but a carriage return character and the decimal value of 10
          processReceivedValue(c);
          Serial.println(c);
        }
      }

      delay(10);
    }

    client.stop();
    Serial.println("Client disconnected");

  }
}
