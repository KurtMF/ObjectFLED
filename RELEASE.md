### Release 1.1.0
* Changed default LED waveform timing to fix conflict with Teensy Audio library and possibly other DMA-enabled apps. 
More relaxed default timing allows ObjectFLED to work out-of-the-box with more LED chips as well.
* Eliminated using both overclock factor and pulse timing specs in same .begin() function call.  Either specify OC factor, 
or pulse timing values, but not both.  See mouseover help or OctoFLED.h for the updated .begin() signatures.  Only those 
using the full form of begin(OC_Factor, THTL, T0H, T1H, Latch_Delay) will need to update their .begin() call.
* Changed how OC factor is applied to waveform timing.  Originally, OC factor was applied to equally shrink TH_TL, T0H, 
and T1H.  Now, OC factor applies to TH_TL and T0H equally, but only reduces T1H by 1/3 of the amount.  This is because 
LED chips have a fixed threshold for when a H pulse is a 0 or a 1.  This change yeilded slightly better overlockability
in testing with WS2812B chips._

### Release 1.0.3
* Added support for GBR, BGR color formats.
 
### Release 1.0.2
* Set DSE=3 on output pins improved LED overclocking by 7% (boot default DSE=6).

### Release 1.0.1
* Updated includes to replace include Arduino.h.
* Started making releases, added INSTALLATION.md file.
* Tweakage to readme.