/* ObjectFLED sample program showing how to use the FastLED built-in ObjectFLED DMA engine,
without loading ObjectFLED.h.  5 LED strings are configured to show blue-cylon lights and
display show() framerate based on 20 back-to-back calls. Tested with FastLED 3.9.11 & 3.9.13. 

When using FastLED without ObjectFLED.h,  each string/pin is added seperately in setup(), and 
show() displays all strings at once.  Strings can be of varying length, but you cannot show 
strings independently.  Only default waveform timing is used. */
//#define FASTLED_USES_OBJECTFLED       //not required for FastLED 3.9.12 and later
#define FASTLED_OVERCLOCK 1.5
#include<FastLED.h>

#define PIX_PER_STR 32
#define NUM_STR 5
CRGB leds[NUM_STR][PIX_PER_STR];      //2D array for all strings, or use separate string arrays

void setup() {
  // pin# 0-4, each with it's own LED string and array row
  FastLED.addLeds<WS2812, 0, RGB>(leds[0], PIX_PER_STR);
  FastLED.addLeds<WS2812, 1, RGB>(leds[1], PIX_PER_STR);
  FastLED.addLeds<WS2812, 17, GRB>(leds[2], PIX_PER_STR);
  FastLED.addLeds<WS2812, 3, GRB>(leds[3], PIX_PER_STR);
  FastLED.addLeds<WS2812, 4, GRB>(leds[4], PIX_PER_STR);
  fill_solid(leds[0], NUM_STR * PIX_PER_STR, 0x0);    //blanks all strings
  FastLED.setBrightness(10);

  Serial.begin(100000);
} //setup()

int delayT = 30;
void loop() {
  //RIGHT blue-cylon pattern on all NUM_STR strings
  for (uint x=0; x<PIX_PER_STR; x++) {
    for (uint y=0; y<NUM_STR; y++) {
      leds[y][x] = CRGB::Blue;
    }
    FastLED.show();
    fadeToBlackBy(leds[0], NUM_STR * PIX_PER_STR, 32);
    delay(delayT);
  }

  //LEFT blue-cylon pattern on all NUM_STR strings
  for (int x=PIX_PER_STR-1; x>=0; x--) {
    for (int y=NUM_STR-1; y>=0; y--) {
      leds[y][x] = CRGB::Blue;
    }
    FastLED.show();
    fadeToBlackBy(leds[0], NUM_STR * PIX_PER_STR, 32);
    delay(delayT);
  }

  //speed test 20 show() calls
  uint totalT, startT = micros();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
    FastLED.show();
  totalT = micros() - startT;
  Serial.printf("Uses FastLED Avg T of 20 :  %.1f uS  FPS:  %.1f\n", totalT / 20.0, 20.0 / totalT * 1000000);
  while (Serial.read()==-1);
} //loop()
