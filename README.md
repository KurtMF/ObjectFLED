# ObjectFLED.h

Allows for configuration and control of multiple (physical) digital LED display objects 
independently in code.  Display objects use DMA transfer (from [OctoWS2811 library](http://www.pjrc.com/teensy/td_libs_OctoWS2811.html))
on any or all 40 or 55 pins of Teensy 4.0 or 4.1 to drive digital LEDs with massive parallelism, 
if needed.

The show() function is non-blocking, returning control to the graphics drawing code in just 6% of 
the time it takes to complete buffer transmission to an LED string (tested with WS2812B, 1.6 
overclock factor, 256 LED per pin x 16 pins = 4096 LEDs total at 204 back-to-back fps).  

ObjectFLED works with FastLED arrays of CRGB, or other drawing buffer in RGB 3-byte format.  

### Glossary: 
* "LED object" or "display object" refers to the ObjectFLED display object(s) in your code.  
* "LED device" refers to physical LEDs: strings, planes, cubes, etc.
* "Segment" refers to physical LEDs connected to a single pin.  This can be a group of LED devices 
(physically daisy-chained) or a subset of LED devices in a larger LED device (rows of a plane, 
planes of a cube, a single string of house lights, an orbit of an electron, etc.)

## SUPPORTED

  - For Teensy 4.x only.  It may be possible to add other boards if they support DMA driven by
    configurable timers, and ability to map digital pins to a data register which DMA can target.
  - LED data color formats: RGB, GRB, BRG, and RGBW


## MAIN BENEFITS

* You can independently configure, control and display multiple LED devices connected to your 
Teensy 4.x, even if the devices use different LED types with different specs.  Combine LED devices
into a single display object, or define separate objects for segments of each device, or just plain 
one display object for each device (segmentation is automatic).  It is also possible to define 2 
display objects to display the same drawing buffer on 2 different LED devices.

* Large LED devices can be driven with parallel output to separate segments of the device.  Physical 
parallelism allows for multi-fold increase in refresh rate.  ObjectFLED automatically breaks your 
drawing buffer into segments and writes to the pins simultaneously in the specified order.  Simply 
define an ordered pin list for your object, in the same order they are connected physically to the 
segments in your LED device.  ObjectFLED performs segmentation "under the covers".

* ObjectFLED works with arrays of CRGB for full compatibility with the rich suite of LED graphics 
functions of FastLED.  But you can use any memory object for drawing buffer, so long as it is 3-
bytes per LED in RGB order.  

* You can tweak the shape of the LED data waveform generated by this driver.  By calling begin() 
with full pulse timing specs, you can tweak the waveform to achieve the highest possible overclock 
(and highest back-to-back refresh rate).  Each display object has it's own begin() function, and 
it's own LED timing.

* The show() function is non-blocking, and returns in just 6% of the time required to write a single 
segment.  However when calling show() back-to-back, each must wait for the prior to complete it's 
latch pulse at the end of writing.  This is also true when calling show() for one object right after 
calling show() for another object.  All display objects share the same DMA-Timer pipeline in hardware.
Therefore, for code which controls a large LED device and a small one, with sequential calls to each 
show(): show the small one first, then show the large one, then update your drawing while the large 
display is still DMA-transmitting 94% of it's LED data.  

* Show() function has built-in handling for serpentine, color order, brightness, and color balance
for each object.  Like in FastLED, these are applied to the frame buffer, not your drawing buffer.
Get and set functions for brightness and colormask are included (the ones in FastLED won't work on
ObjectFLED display objects).

* Accessory functions included to fade a drawing array to a color other than black, and to draw a 
square on an LED plane.


## LED DEVICES

Physical LED devices are connected to Teensy's digital pins via a buffer circuit to step the 3.3v
Teensy squarewave up to 5v for the LEDs.  For buffering, use [OctoWS2811 boards](https://www.pjrc.com/store/octo28_adaptor.html) or SN74AHCT125 or equivalent.  

LED planes can have their data paths physically wired for serpentine (reverse direction on every
other row).  ObjectFLED will automatically perform serpentine output if you specify an LED count
(row size) for your display object.  However, with planes in a cube, ObjectFLED expects the last LED
of a plane to connect to the first LED of the next plane (no plane-level serpentine, only row-
level).

When taking advantage of parallel inputs into an LED device, each pin is assumed to drive the same
number of LEDs (or rows or planes).  When you define your ObjectFLED object, provide an ordered pin 
list which matches the order in which the pins connect to the device.  ObjectFLED will output segments from your display buffer to those pins in the order you specify.  

Individual LEDs come with various formats for RGB color order, data signal clock frequency, and 
pulse timing specs.  ObjectFLED defaults will work for popular RGB-order LEDs with 800KHz clock and
75uS latch delay (Note that for tested WS2812's, which specify 50uS latch, I had to use 72, perhaps
because of added CPU overhead).  Use the begin( params ) function on an ObjectFLED object in order 
to use timing specs from your LED's datasheet.  Specify the color order when you instantiate 
ObjectFLED.


## USAGE 

### ObjectFLED CLASS

ObjectFLED(uint16_t numLEDs, void* drawBuf, uint8_t config, uint8_t numPins, const uint8_t* pinList, 
    uint8_t serpentine = 0);

- numLEDs = total number of LEDs in the device(s) represented by your drawing buffer
- drawBuf = drawing array of CRGB or similar format which contains your graphics  NOTE: multi-dimensional 
drawing arrays must be in row-major order.  That means myCube[ Z ][ Y ][ X ], not myCube[ X ][ Y ][ Z ], 
where Z is plane, Y is row, X is pixel in row.  
- config = predefined value for color order (ie- CORDER_RGB, see above for supported list)
- numPins = number of pins connected to the LED device(s)
- pinList = array of pin numbers in the order which they are connected to successive segments
- serpentine (optional) = number of LEDs in each row for serpentine

**Example:**

    CRGB myCube[16][16][16];
    uint8_t pinList[8] = {1, 8, 14, 17, 24, 3, 4, 5};	//8 pins for 16 planes: 2 planes per pin
    //Pins are attached to each successive pair of planes in the given list order
    ObjectFLED dispCube( 16*16*16, myCube, CORDER_GRB, 8, pinList, 16 );   //serpentine 16 LEDs/row

**Best Practice:**
Use variable names which partially match for creating a drawing array and it's matching display
object.  Ex.-

    CRGB grid4[Y][X];   ObjectFLED dspGrid4( Y*X, grid4, etc. );


### begin() FUNCTION

Called once in setup for the display object

// Default timing 800KHz clock, 75uS latch delay  
begin(void);                

// Overclock default timing by the given factor  
begin(float OCF);           

// Overclock default timing and overrde default latch delay  
begin(float OCF, uint16_t latchDelay);        

// Fully specify output waveform timing.  NOTE: Given period, t0h, t1h are divided by given OCF.  
begin(float OCF, uint16_t period, uint16_t t0h, uint16_t t1h, uint16_t latchDelay);

- OCF = Overclocking factor multiples the clock rate (by dividing the pulse width values below)
- period = nS time for duration of a full LED data pulse (from LED datasheet) NOTE: For 800KHz clock, 
period = 1250nS.  This is the default setting.
- t0h = nS time for duration of high portion of pulse for LED data 0 (from LED datasheet)
- t1h = nS time for duration of high portion of pulse for LED data 1 (from LED datasheet)
- latchDelay = uS time to hold data low for LEDs to latch color data to DACs (from LED datasheet) 
NOTE: In test I had to use 72uS latch delay for LEDs with a spec of 50uS, in order to stop
 a severe display glitch.  I've read at least one LED model specifies 300uS latch.  If you 
 get display glitches on a new LED type, latch time should be the first thing to check.        

**Example:**

    dispCube.begin(1.5, 72);        //overclocks by factor 1.5 (1200KHz) and sets 72uS latch delay
    dispVid.begin(1.68, 1250, 420, 840, 72);   //420nS, 840nS are 1/3, 2/3 of period


### show() FUNCTION

See MAIN BENEFITS above for details.

**Example:**

    dispCube.show();    //dispCube has far fewer LEDs than dispVid (it is a small cube)
    dispVid.show();	    //by calling show on the large object last, code can re-draw while 
                        //~94% of dispVid's LED data is being written by DMA under the covers.
                        //if you call dispCube.show() last, it would wait for all that 94% of
                        //dispVid data to finish, then wait for latch cycle, before it ran.


### setBrightness(), getBrightness() FUNCTIONS

Brightness values 0-255.  Brightness is applied by show() to frame buffer, not your 
drawing buffer.

* setBrightness(uint8_t);

* getBrightness() { return uint8_t brightness; }


### setBalance(), getBalance() FUNCTIONS

Color Balance is 3-byte number in RGB order.  Each byte is a brightness value for that color.
Like brightness, this is applied by show() to frame buffer, not to your drawing buffer.

* setBalance(uint32_t);

* getBalance() { return uint32_t colorBalance; }


## ACCESSORY FUNCTIONS

These are not part of display objects, call them without object specifier.  Unlike brightness and 
balance, these functions operate on your drawing buffer.

### fadeToColorBy(void* leds, uint16_t count, uint32_t color, uint8_t fadeAmt);

Fades drawing array towards the background color by amount.  It is used just like FastLED's 
fadeToBlackBy().

**Example:**

    fadeToColorBy( myCube, 16*16*16, 0xFF8000, 20 );      //fades towards orange by 20/255ths

### drawSquare(void* leds, uint16_t planeY, uint16_t planeX, int yCorner, int xCorner, uint size, uint32_t color);

Safely draws box in given RGB color on LED plane. cornerY and cornerX specify the lower left 
corner of the box.  It is safe to specify -cornerY, -cornerX, and safe to draw a box which only 
partially fits on LED plane.

**Example:**

    //draws a red 10x10 square on the 3rd 16x16 plane of myCube, lower left corner anchored at 
    //col = row = 4.
    drawSquare( &myCube[2][0][0], 16, 16, 4, 4, 10, 0xFF0000 );
