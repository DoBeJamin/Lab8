#include <env.h>
#include <Wire.h>

#include "blynk.h"
#include "oled-wing-adafruit.h"
#include "SparkFun_VCNL4040_Arduino_Library.h"

SYSTEM_THREAD(ENABLED);

//component objects
VCNL4040 proximitySensor;
OledWingAdafruit display;

//name pins
#define LED1 D5
#define LED2 D6
#define LED3 D7

//internal variables
float current_max = 0; 
bool data_displayed_toggle = true;
bool button_C_toggle = true;
int ambient_value;
int prox_value;

void setup() {

  //VNCL setup
  Wire.begin();
  //turns on sensor
  proximitySensor.begin(); 
  proximitySensor.powerOnProximity();
  proximitySensor.powerOnAmbient();
  proximitySensor.enableWhiteChannel();

  //led setup
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  //oled setup
  display.setup();

  //blynk setup
  Blynk.begin(BLYNK_AUTH_TOKEN);

}

void loop() {

  //update display and blynk
  display.loop();
  Blynk.run();

  //button toggles
  if (display.pressedA()) {
    data_displayed_toggle = true;
  }
  if (display.pressedB()) {
    data_displayed_toggle = false;
  }
  if (display.pressedC()) {
    button_C_toggle = !button_C_toggle;
  }

  //collect current proximity information
  prox_value = proximitySensor.getProximity();

  //display proximity on LED's
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  if  (prox_value < 50) {
    digitalWrite(LED1, HIGH);
  } else if (prox_value < 400) {
    digitalWrite(LED2, HIGH);
  } else {
    digitalWrite(LED3, HIGH);
  }

  //Displays information on OLED or BLYNK
  if (button_C_toggle) {
    //displays info on the OLED screen
     format_display();
    if (data_displayed_toggle) {
      display.print(prox_value);
    } else {
      ambient_value = proximitySensor.getAmbient();
      display.print(ambient_value);
    }
    display.display();

  } else {
    //Displays information on BLYNK
    if (data_displayed_toggle) {
      Blynk.virtualWrite(V0, prox_value);
    } else {
      int ambient_value = proximitySensor.getAmbient();
      Blynk.virtualWrite(V1, ambient_value);
    }

  }

}

void format_display () {
  //sets up basic formatting for when using the display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE); 
  display.setCursor(0,0);
}