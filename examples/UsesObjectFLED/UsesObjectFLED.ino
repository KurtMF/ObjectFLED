/* ObjectFLED sample program showing how to use ObjectFLED.h with FastLEDs.  5 LED strings 
are configured to show blue-cylon lights and display show() framerate based on 20 back-to-back 
calls.  Tested with FastLED 3.9.11 & 3.9.13. 

When using FastLED with ObjectFLED.h,  an ordered array of pin numbers is passed to the ObjectFLED
instance along with the 2D array for all 5 strings, and show() displays all strings at once.
If strings are of varying length, then the 2D array holds the largest string, and extra data in the 
array for shorter strings is effectively ignored, even if sent to the string.  

You can optionally define separate 1D arrays for varying size strings with a separate ObjectFLED 
instance for each string.  With this approach, each string has it's own show(), get/setBrightness(), 
get/setBalance(), overclock, custom waveform timing, and color order.  */
#include<ObjectFLED.h>        //must include this before FastLED.h
#include<FastLED.h>

#define PIX_PER_STR 32
#define NUM_STR 5
CRGB leds[NUM_STR][PIX_PER_STR];      //2D array for all strings, or use separate string arrays
// pin# 0-4, each with it's own LED string and array row
uint8_t pinList[NUM_STR] = {0, 1, 17, 3, 4};
ObjectFLED dispLeds(PIX_PER_STR * NUM_STR, leds, CORDER_RGB, 5, pinList );

void setup() {
  dispLeds.begin(1.5);                //1.5 Overclock Factor
  fill_solid(leds[0], NUM_STR * PIX_PER_STR, 0x0);    //blanks all strings
  dispLeds.setBrightness(10);
  Serial.begin(100000);
} //setup()

int delayT = 30;
void loop() {
  //RIGHT blue-cylon pattern on all NUM_STR strings
  for (uint x=0; x<PIX_PER_STR; x++) {
    for (uint y=0; y<NUM_STR; y++) {
      leds[y][x] = CRGB::Blue;
    }
    dispLeds.show();
    fadeToBlackBy(leds[0], NUM_STR * PIX_PER_STR, 32);
    delay(delayT);
  }

  //LEFT blue-cylon pattern on all NUM_STR strings
  for (int x=PIX_PER_STR-1; x>=0; x--) {
    for (int y=NUM_STR-1; y>=0; y--) {
      leds[y][x] = CRGB::Blue;
    }
    dispLeds.show();
    fadeToBlackBy(leds[0], NUM_STR * PIX_PER_STR, 32);
    delay(delayT);
  }

  //speed test 20 show() calls
  uint totalT, startT = micros();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
    dispLeds.show();
  totalT = micros() - startT;
  Serial.printf("Uses ObjectFLED Avg T of 20 :  %.1f uS  FPS:  %.1f\n", totalT / 20.0, 20.0 / totalT * 1000000);
  while (Serial.read()==-1);
} //loop()
