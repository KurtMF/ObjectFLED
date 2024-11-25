//Cylon demo for OctoFLED Raw (without FastLED)
#include <ObjectFLED.h>

// How many leds in your strip?
#define NUM_LEDS 32 

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN. Or list of pins for parallel connections to your LED device.
uint8_t DATA_PIN[1] = { 17 };
// Define the array of leds, 3 bytes per
uint8_t leds[NUM_LEDS*3];
// Define the display object with 8-pixel serpentine on 8x8 plane
ObjectFLED cylon( NUM_LEDS, leds, CORDER_GRB, sizeof(DATA_PIN), DATA_PIN, 8);
void setup() { 
    cylon.begin();    //make display object ready at default LED clock rate 800 KHz
    cylon.setBrightness(84);
}

const uint8_t dotColor[3] = { 0xFF, 0, 0 };     //R, G, B
const int delayT = 25;
const int fadeAmt = 40;
void loop() { 
  for( int i=0; i<NUM_LEDS*3; i+=3) {
    memcpy(&leds[i], &dotColor, 3);
    cylon.show();
    fadeToColorBy(leds, NUM_LEDS, 0 ,fadeAmt);
    delay(delayT);
  }
  delay(delayT);
  for( int i=(NUM_LEDS-1)*3; i>=0; i-=3) {
    memcpy(&leds[i], &dotColor, 3);
    cylon.show();
    fadeToColorBy(leds, NUM_LEDS, 0 ,fadeAmt);
    delay(delayT);
  }
  delay(delayT);
}   //loop()
