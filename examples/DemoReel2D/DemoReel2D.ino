/// @file    DemoReel100.ino
/// @brief   FastLED "100 lines of code" demo reel, showing off some effects
/// @example DemoReel100.ino
// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED.
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014
//
// Ported to use ObjectFLED, input brightness via Serial Nov 2024 by Kurt Funderburg
#include <ObjectFLED.h>
#include <FastLED.h>

#define NUM_LEDS 8        //per row
#define NUM_STRIPS 8     //should be even multiple of NUM_PINS; 24 = 4 rows per pin
uint BRIGHTNESS = 6;
const uint NUM_PINS = 2;
//uint8_t ports[NUM_PINS] = { 1, 8, 14, 17, 24, 29 };
uint8_t ports[NUM_PINS] = { 17, 24 };
uint8_t ports2[4] = { 1, 8, 14, 29 };

//Create 2 display objects using the same drawing array to display image on 2 devices
CRGB plane[NUM_STRIPS][NUM_LEDS];
//8x8 grid with data connection broken between 4th & 5th rows to drive with 2 pins parallel
ObjectFLED dispPlane(NUM_LEDS*NUM_STRIPS, plane, CORDER_GRB, NUM_PINS, ports, 8);
//4 4x4 grids of LEDs on my breadboard (for better results, replace this with device of same 
//  or even multiples of the first device dimensions)
ObjectFLED dispPlane2(NUM_LEDS*NUM_STRIPS, plane, CORDER_RGB, 4, ports2, 4);

void setup() {
  Serial.begin(1000000);
  Serial.print("*********************************************\n");
  Serial.print("* DemoReel1002D.ino                         *\n");
  Serial.print("*********************************************\n");
  Serial.printf("CPU Speed: %d MHz  Temp: %.1f C  %.1f F  Brightness %d\n", F_CPU_ACTUAL / 1000000, \
                  tempmonGetTemp(), tempmonGetTemp() * 9.0 / 5.0 + 32, BRIGHTNESS);
  Serial.println("Enter brightness level at any time via serial monitor.");
  
  //start both display objects
  dispPlane.begin();
  dispPlane2.begin();
  dispPlane.setBrightness(BRIGHTNESS);
  dispPlane2.setBrightness(BRIGHTNESS);
} // setup()


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0;  // Index number of which pattern is current
static uint8_t gHue = 0;                   // rotating "base color" used by many of the patterns
uint strIdx = 0, strLen = 8;         //for reading brightness int from serial
char serInput[8], x;

void loop() {
  // Call the current pattern function once, updating the 'plane' array
  gPatterns[gCurrentPatternNumber]();
  

  // send the 'plane' array out to both displays
  dispPlane.show();
  dispPlane2.show();
  delay(40);  // too see the glitter

  // do some periodic updates
  EVERY_N_MILLISECONDS(30) {
    gHue++;
  }  // slowly cycle the "base color" through the rainbow

  EVERY_N_SECONDS(1) {    // check/read serial for new brightness#
    strIdx = 0;
    while(Serial.available() && strIdx < strLen) {
      if ((x = Serial.read()) != -1) {
        if(x >= '0' && x <= '9') { serInput[strIdx++] = x; }  //store it if a digit
      } 
    } //while R bytes
    Serial.clear();   // got my 8B, anything else must be cat on keybaord

    if(strIdx > 0) {    //str > int, then set brightness
      BRIGHTNESS = 0;
      for(uint i=0; i<strIdx; i++) { 
        BRIGHTNESS *= 10;
        BRIGHTNESS += serInput[i] - '0';
      }
      Serial.printf("New BRIGHTNESS level: %d\n", BRIGHTNESS);
      dispPlane.setBrightness(BRIGHTNESS);
    }

  }

  EVERY_N_SECONDS(45) {
    nextPattern();
  }  // change patterns periodically
} //loop()

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern() {
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow(plane[0], NUM_LEDS * NUM_STRIPS, gHue, 1);
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(127);
}

void addGlitter(fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    plane[random8(NUM_STRIPS)][random8(NUM_LEDS)] = CRGB::White;
}
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(plane[0], NUM_LEDS * NUM_STRIPS, 15);
  plane[random16(NUM_STRIPS)][random16(NUM_LEDS)] += CHSV(gHue + random8(64), 200, 255);
}

static uint phase = 0;
uint phaseInc = 96;      //controls phase shift in sinelon
void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToColorBy(plane, NUM_LEDS * NUM_STRIPS, 0x000060, 10);
  int posX = beatsin16(80, 0, NUM_LEDS - 1);
  int posY = beatsin16(80, 0, NUM_STRIPS - 1, 0, phase); // Phase 0 - 65535 = 0 - 180 degrees);
  plane[posY][posX] = CHSV(gHue, 255, 255);

  if ( (phase > 32864) || (phase < 0)) phaseInc = -phaseInc;
  phase += phaseInc;  
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 30;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int j = 0; j < NUM_STRIPS; j++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      plane[j][i] = ColorFromPalette(palette, i*32+j*32, beat);
    }
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(plane[0], NUM_LEDS * NUM_STRIPS, 15);
  uint8_t dothue = 0;
    for (int k = 0; k < NUM_STRIPS; k++) {
      for (int i = 0; i < NUM_LEDS; i++) {
        plane[k][beatsin16(40, 0, NUM_LEDS - 1)] = CHSV(dothue, 255, 255);
        dothue += 2;
      }
    }
}
