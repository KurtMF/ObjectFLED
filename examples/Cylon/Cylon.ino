//Cylon demo for OctoFLED
#include <ObjectFLED.h>
#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 32 

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN. Or list of pins for parallel connections to your LED device.
uint8_t DATA_PIN[1] = { 17 };
// Define the array of leds
CRGB leds[NUM_LEDS];
// Define the display object with 8-pixel serpentine on 8x8 plane
ObjectFLED cylon( NUM_LEDS, leds, CORDER_GRB, sizeof(DATA_PIN), DATA_PIN, 8);
void setup() { 
    cylon.begin();    //make display object ready at default LED clock rate 800 KHz
    cylon.setBrightness(84);
}

CRGB dotColor = 0xff0000;
const int delayT = 25;
const int fadeAmt = 40;
void loop() { 
  for( int i=0; i<NUM_LEDS; i++) {
    leds[i] = dotColor;
    cylon.show();
    fadeToBlackBy(leds, NUM_LEDS, fadeAmt);
    delay(delayT);
  }
  delay(delayT);
  for( int i=NUM_LEDS-1; i>=0; i--) {
    leds[i] = dotColor;
    cylon.show();
    fadeToBlackBy(leds, NUM_LEDS, fadeAmt);
    delay(delayT);
  }
  delay(delayT);
}   //loop()
