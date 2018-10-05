/*  connections for the Teensy 3.5
      euclidean sequencer usbMIDI
connected to - pin ---- pin - connected to
------------------------------------------
all enc /etc - Gnd -----VIN - all enc /etc
Save fr MIDI - 0 -----A-GND -
Save fr MIDI - 1 ------3.3v -
rot1 left ---- 2 --------23 - button4
rot1 switch -- 3 --------22 - button3
rot1 right --- 4 --------21 - button2
             - 5 --------20 - button1
rot1 blue ---- 6 --------19 - Screen SCL i2c
rot1 green --- 7 --------18 - Screen SDA i2c
rot1 red ----- 8 --------17 - rot2 switch
             - 9 --------16 - rot2 blue
neopix rings - 10 -------15 - rot2 green
rot2 left ---- 11 -------14 - rot2 red
rot2 right --- 12 -------13 - 
             - 3.3v ----GND -
rot3 right --- 24 -----DAC1 - (analogue only) save for CV out
rot3 left ---- 25 -----DAC0 - (analogue only) save for CV out
rot3 red------ 26 -------39 - rot4 switch
rot3 green --- 27 -------38 - rot4 blue
rot3 blue ---- 28 -------37 - rot4 green
save(timer3)-- 29 -------36 - rot4 red
save(timer3)-- 30 -------35 - rot4 right
rot3 switch -- 31 -------34 - rot4 left
             - 32 -------33 -

//--------note on display driver--------//
// using (C)2015 Laurentiu Badea for display as easy extension for <Adafruit_SSD1306.h>

//---------pins note------------//
//save teensy pins 18 and 19 for i2c ssd1306 OLED
//save pins for timer3 = pins 29 and 30

//------------neopixel note---------------------//
//note, as neopixel rings are wired in anticlockwise fashion, need to reverse the call to leds to move the pixel along in
// a clockwise fashion. Therefore starting led = 15 and not zero
// therefore arrange rings to have pixel 15 at top.
//The euclid funciton gives a set of sixteen bits for on or off for each step
//this needs to be read in reverse, 


//---------------clock divider values ---------------------// 
/* a "beat" in terms of tempo is a quarter note 
* a quarter note is always 1 beat when talking about tempo, regardless of the time signature.
* there are 24 MIDI Clocks in every quarter note
* therefore a divider of 24 would give the correct beats tempo per minute
* 12 is twice as fast, so a beat plays each 1/8th note
* 48 plays each 1/2 note
* 24 pulses per 1/4 note = 24*4 = 96 pulses for a note
*/
*/
