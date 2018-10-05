# Teensy-Midi-Euclidean-Sequencer
A usbMidi euclidean sequencer on a Teensy 3.5 programmed using Arduino IDE

this is a four channel usb MIDI euclidean sequencer. the sequencer is running on a Teensy 3.5. The setup consist of four rgb rotary encoders from Sparkfun, a Teensy, an ssd1306 lcd, four momentary buttons, and four 16led neopixel rings. The housing is an old case from a bottle of port, for now, and a piece of spare translucent perspex.

https://www.youtube.com/embed/aqOsPZUo860
https://youtu.be/aqOsPZUo860

Euclidean rhythms are explained by White Noises at the link below. there are three parts,
1. the total number of steps in the sequence
2. the number of hits within those steps
3. how far the sequence is offset by

The euclidean algorithm is used to divide up the number of hits across the number of steps as evenly as possible. So, for example, if we had four hits happening over 16 steps, there would be one hit every four steps. Interesting things happen when the number of beats can not be divided evenly over the number of steps, and also when the number of total steps varies between different channels.

discussion on euclidean sequences by White Noises
https://youtu.be/OHS3lN6snrE

Inspiration for this comes from:
Euclidean Circles modular sequencer - https://vpme.de/euclidean-circles/
https://www.youtube.com/watch?time_c...&v=LcVSN1q02Mg

Current Features:
4 tracks, 16 steps
4 channel Midi over USB
midi clock divider per channel
- "1/1","1/2","1/4","1/8","1/16","1/32" note 
internal or external clock
automatic switch to external clock if present at startup
start / stop sync with external DAW midi byte
memory saved to SD card
32 memory slot save and recall
USB powered
adjust step number per channel
adjust hit/beat number per channel
adjust offset value per channel
on/off per channel
step location indicator per channel
setting display on oled
absolutely no deep dive menu

The sequencer automatically uses external midi clock if present when switched on, otherwise uses internal clock that can be controlled by one of the rotary encoders.

all active steps per channel are shown as light blue, with the evenly spaced beats / hits shown as red. the rotary controlled selection options match blue for step number selection and red for beat number selection. 

each channel can be switched off or on, change the clock divider, change the step number and beat number, as well as have an offset start point. this is all done by using the push button of the rotary encoder to select the option and then turn the encoder to select the value for that option. Al the settings are show on the LCD screen.

I wanted to try and use software only debounce rather than hardware debounce as used on previous projects with these rotary encoders. It was not easy, and perhaps I should have gone with hardware as there there is still some small amount of gutter that I am trying for sort out. Also, this would have been a perfect project for using a pcb, as its a true rats nest under the hood.

Connections for the Teensy 3.5 euclidean sequencer usbMIDI
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


BOM
4 x 16led neopixel rings
4 x momentary push button
1 x Teensy 3.5
1 x SD card
4 x Sparkfun RGB rotary Encoders
1 x SSD1306 bi colour LCD 128x64
12 x 330 Ohm resistors (1 per rotary led colour)
connectors, solder, header pins, socket, etc.

References:
GitHub Code for this project is found at:
https://github.com/mortonkopf

display.cpp - Display class with printf() and human-readable units from Tiny Scope for Arduino project - Copyright (C)2015 Laurentiu Badea

Euclid calculation functions are taken directly from http://clsound.com/euclideansequenc.html - acknowledgment to Craig Lee 

https://github.com/TomWhitwell/Euclidean-sequencer-

http://electro-music.com/forum/topic-61011.html - Tombola and further expanded by sneak-thief and Syinsi.

Godfried T. Toussaint*2005 work on Euclidean algorithm. work on computing the greatest common divisor of two integers being linked to world music rhythms - paper “The Euclidean Algorithm Generates Traditional Musical Rhythms”.*
http://cgm.cs.mcgill.ca/~godfried/pu...ions/banff.pdf
