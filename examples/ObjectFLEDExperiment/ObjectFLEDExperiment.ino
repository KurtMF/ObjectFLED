/*  OctoExperiment.ino - Test program to show revolving electron around edges of 
  3 4x4 planes laid out horizontally (on a breadboard) while fading the nucleus
  (an 8x8 plane) between blue and orange in sync with electron.
  Kurt Funderburg - Nov 2024

  The 3 planes are each connected to a Teensy pin, driven in parallel, while the 
  8x8 plane is separated into 2 groups of 4 rows, each group connected to a pin.
  You can alter the variables to reflect the geometry and signal path for your LED
  devices.
  
  In code, the 3 planes are stored in CRGB electronLED[Z][Y][X].  The nucleus plane
  is CRGB nucleusLED[Y][X].  This allows for intuitive loop control, and ObjectFLED
  requires drawing buffer arrays to be in row-major order.
*/
#include <ObjectFLED.h> 
#include <FastLED.h>

//3x4x4 YF923 RGB "cube" actually 3 planes laid out horizontally in 4x12.
#define PIX_PER_ROW     4
#define ROWS_PER_PLANE  4
#define NUM_PLANES      3
#define NUM_PINS        3
#define NUM_LEDS        (PIX_PER_ROW * ROWS_PER_PLANE * NUM_PLANES)
const byte pinList[NUM_PINS] = {1, 8, 14};
const int config = CORDER_RGB;
CRGB electronLED[NUM_PLANES][ROWS_PER_PLANE][PIX_PER_ROW];  
ObjectFLED electron(NUM_LEDS, electronLED, config, NUM_PINS, pinList, PIX_PER_ROW);

//8x8 WS2812B GRB plane from Amazon
#define PIX_PER_ROW2     8
#define NUM_ROWS2        8
#define NUM_PINS2        2
#define NUM_LEDS2       (PIX_PER_ROW2 * NUM_ROWS2)
const byte pinList2[NUM_PINS2] = {17, 24};
const int config2 = CORDER_GRB;
CRGB nucleusLED[NUM_ROWS2][PIX_PER_ROW2];    //Octo assumes this buffer size/type
ObjectFLED nucleus(NUM_LEDS2, nucleusLED, config2, NUM_PINS2, pinList2, 8);

const uint32_t background = 0xFFFF00;
const uint32_t drawColor = 0x0000FF;
const uint8_t brightness = 4;
const int delayT = 60;    // mS loop delay
const int fade = 50;      // fade electron
int fade2 = 20;           // fade nucleus
uint planeColor1 = 0x0000FF;
uint planeColor2 = 0xff7000;
uint planeColor = planeColor1;
void setup() {
  Serial.begin(1000000);
  Serial.println("OctoExperiment");

  //"nucleus" plane 8x8 fades back and forth from orange to blue driven by 2 pins
  nucleus.begin();      //use default LED timing: 800 KHz (1250 nS) with 75 uS latch delay
  nucleus.setBrightness(brightness);
  nucleus.setBalance(0xdae0ff);
  fill_solid(nucleusLED[0], NUM_LEDS2, planeColor2);
  nucleus.show();

  //"electron orbit" around 3 4x4 planes driven by 3 pins
  //1.68 OC factor with these timing values are max OC for YF923's I found posing as WS2812B's
  electron.begin(1.68, 1250, 420, 840, 72);     
  electron.setBrightness(brightness);
  electron.setBalance(0xdae0ff);
  fill_solid(electronLED[0][0], NUM_LEDS, background);
  electron.show();
//while(Serial.read()==-1);
}

void loop() {
  for (int i=0; i<ROWS_PER_PLANE; i++) {    //electron up left edge of left plane
    electronLED[0][i][0] = drawColor;
    electron.show();
    fadeToColorBy(electronLED[0][0], NUM_LEDS, background, fade);
    drawSquare(nucleusLED, NUM_ROWS2, PIX_PER_ROW2, 4 - i - 1, 4 - i - 1, i * 2 + 2, 0x800000);
    nucleus.show();
    fadeToColorBy(nucleusLED[0], NUM_LEDS2, planeColor, fade2);
    delay(delayT);
//while(Serial.read()==-1);
  }
  for (int j=0; j<NUM_PLANES; j++) {     //electron right across top of planes
    for (int i=0; i<PIX_PER_ROW; i++) {
      electronLED[j][ROWS_PER_PLANE-1][i] = drawColor;
      electron.show();
      fadeToColorBy(electronLED[0][0], NUM_LEDS, background, fade);
      nucleus.show();
      fadeToColorBy(nucleusLED[0], NUM_LEDS2, planeColor, fade2);
      delay(delayT);
    }
  }
  for (int i=ROWS_PER_PLANE-1; i>=0; i--) {    //electron down right edge of right plane
    electronLED[NUM_PLANES-1][i][PIX_PER_ROW-1] = drawColor;
    electron.show();
    fadeToColorBy(electronLED[0][0], NUM_LEDS, background, fade);
    drawSquare(nucleusLED, NUM_ROWS2, PIX_PER_ROW2, 4 - i - 1, 4 - i - 1, i * 2 + 2, 0x800000);
    nucleus.show();
    fadeToColorBy(nucleusLED[0], NUM_LEDS2, planeColor, fade2);
    delay(delayT);
  }
  for (int j=NUM_PLANES-1; j>=0; j--) {     //electron left across bottom of planes
    for (int i=PIX_PER_ROW-1; i>=0; i--) {
      electronLED[j][0][i] = drawColor;
      electron.show();
      fadeToColorBy(electronLED[0][0], NUM_LEDS, background, fade);
      nucleus.show();
      fadeToColorBy(nucleusLED[0], NUM_LEDS2, planeColor, fade2);
      delay(delayT);
    }
  }

  if(planeColor == planeColor2) { planeColor = planeColor1; fade2 = fade2 / 2; }
  else { planeColor = planeColor2; fade2 = fade2 * 2; }
} // loop()
