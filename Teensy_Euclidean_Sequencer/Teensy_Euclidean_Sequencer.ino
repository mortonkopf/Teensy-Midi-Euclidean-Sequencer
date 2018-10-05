
#include "TimerThree.h"
#include "RGB_Rot_Enc.h"
#include <MIDI.h>
#include "display.h"
#include <Bounce2.h>
#include <WS2812Serial.h>//use nonblocking ws2812 serial lib to avoid blocking of MIDI in/out?
#include "memory.h" //this brings in the sd card for storing patches


//----------------------for on/off buttons--------------//
int buttonPin[4] ={23,22,21,20};
boolean currentState[4] ={LOW,LOW,LOW,LOW};
boolean lastState[4] ={LOW,LOW,LOW,LOW};
boolean switchState[4] ={LOW,LOW,LOW,LOW};

// Instantiate a Bounce objects for buttons
Bounce debouncer5 = Bounce(); 
Bounce debouncer6 = Bounce(); 
Bounce debouncer7 = Bounce(); 
Bounce debouncer8 = Bounce(); 

// Instantiate a Bounce objects for push switchs
Bounce debouncer9 = Bounce(); 
Bounce debouncer10 = Bounce(); 
Bounce debouncer11 = Bounce(); 
Bounce debouncer12 = Bounce(); 


//------------------------for leds------------------------//
int positionLed [4] ={15,31,47,63};
const int numled = 64;
const int pin = 10;
byte drawingMemory[numled*3];         //  3 bytes per LED
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED

WS2812Serial leds(numled, displayMemory, drawingMemory, pin, WS2812_GRB);
#define RED    0x160000
#define GREEN  0x001600
#define BLUE   0x000016
#define YELLOW 0x101400
#define PINK   0x120009
#define ORANGE 0x100400
#define WHITE2  0x00001 //so that it does not clash with oled WHITE
#define ALLOFF 0x000000

//-------------------for ssd1306 display--------------//
int screenx[4] ={0,32,65,100};
int screeny[3] ={0,25,50}; //row1=0=steps, row2=25=beats, row3=50(yellow)=offset

void displaySplash(){
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(5,20);
    display.print(F("M0rk0 SQ_E"));
    display.display();   
    delay(3000);
    display.clearDisplay();
}

void writeOut(String myText, int y, int x){   
  display.setTextSize(2);
  display.setCursor(x,y);
  display.print((myText));
 }

//------------------------------for midi and beat timing-------------------------------//
String screenDIV[6] ={"1/1","1/2","1/4","1/8","1/16","1/32"};
int divRatio[6] = {96,48,24,12,6,3};//note multipliers
int channelDiv[5] = {3,3,3,3,3};//initialise each channel at 12 div

//---------Clock-------------//
//clock tick byte (at 24 pulses per quarter note) = 248; start byte = 250; continue byte = 251; stop byte = 252;
//byte counter; byte CLOCK = 248; 
byte START = 250; byte CONTINUE = 251; byte STOP = 252;
byte commandByte;
byte noteByte;
byte velocityByte;
byte inClock;
int clockState =0;
int bpm =120;
const byte CLOCK = 0xF8; //=midi beat clock message (or 248 decimal)

boolean masterClock = true; //true for using teensy usbmidi clock

volatile float midIntervall;// = (1000.0/(float(bpm)/60.0))/24.0;
unsigned long pastTime = micros();
unsigned long currentTime = micros();
int tickCount[4]= {0,0,0,0};//use to count steps for each rotary
int tickCounter;
 int stepNum=48;
byte incomingType;

boolean BPMExternal = 0;
//TOTO / implement
//SHIFT loop start point . change which step is the first = not offset, 
//but maintain pattern where it is, just start in a different place.
 
//-----------------------------setup section-----------------------------------//
void setup() {  
//------------setup the midi-------------//
 Serial.begin(31250);
      usbMIDI.begin();
      usbMIDI.sendNoteOn(60, 127, 1);    // Send a Note (pitch 42, velo 127 on channel 1)

  Timer3.initialize(100); //ties-up Teensy 3.5 pins 29 and 30
  Timer3.attachInterrupt(MidiClock);

//---------setup the leds----------------//
//for ws28 serial
  leds.begin();
  
//------------setup the display--------//
  delay(100);  // give time for display to init
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
    display.setRotation(2);
    displaySplash();
    display.display();
    display.clearDisplay();

//----------------setup the encoders and check working--------//
    setEncoders();

//---------------for channel on/off push buttons----------------//
  pinMode(buttonPin[0], INPUT_PULLUP);
  pinMode(buttonPin[1], INPUT_PULLUP);
  pinMode(buttonPin[2], INPUT_PULLUP);
  pinMode(buttonPin[3], INPUT_PULLUP);

  debouncer5.attach(buttonPin[0]);
  debouncer5.interval(25); // interval in ms
  debouncer6.attach(buttonPin[1]);
  debouncer6.interval(25); // interval in ms
  debouncer7.attach(buttonPin[2]);
  debouncer7.interval(25); // interval in ms
  debouncer8.attach(buttonPin[3]);
  debouncer8.interval(25); // interval in ms

//commented out as still working on using software only debounce
 /* debouncer5.attach(3);
  debouncer5.interval(5); // interval in ms
  debouncer6.attach(17);
  debouncer6.interval(5); // interval in ms
  debouncer7.attach(31);
  debouncer7.interval(5); // interval in ms
  debouncer8.attach(39);
  debouncer8.interval(5); // interval in ms
  */

//********************check that SD card is functioning***************//
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;}
    delay(100);
  Serial.println("SD OK!");
//*******************end of SD check******************************//
  
startRot();
updateScreen1();

}
//--------------end of setup-----------------//


//-------------------- loop-----------------------------//
void loop(){
  
//check to see if using external clock - do all the time
  if (usbMIDI.read()) {   // Is there a MIDI message incoming ?
    incomingType = usbMIDI.getType();
      
      if (incomingType ==250){//if 250 (a start byte) is received, reset all step counters to zero
        tickCounter=0;
        for(int allCounters=0;allCounters<4;allCounters++){
          tickCount[allCounters]=15;} 
          }     
    if (incomingType ==248){  //if we are receiving clocks from external, stop internal clock
      Timer3.detachInterrupt();
      masterClock =false; //set flag for inernal or external clock
      MidiClock();
      BPMExternal=0;
     }else{ BPMExternal=1;}

      if (incomingType ==252){}  //not used yet
   }

//check on off all the time
checkOnOff();

//read all the rotaries all the time
//this does swprocess(), ledProcess() and updateScreen1() and rotEncProcess**(enc); 
encoderProcess(); 

//load all euclid info into one number for each rotary
 for (int seq=0;seq<4;seq++){
      euclidOut[seq] = euclid(euclidParams[seq][0],euclidParams[seq][1],euclidParams[seq][2]);
    }
   
//update the beat locations all the time = does Led.Show() all the time, 
//needed as need to update changes to beat locations. todo alt - do this only of beat locations change?
doLedBeats();

}
//-------------------------end of loop----------------------//


//------------------for on/off buttons--------------//
void checkOnOff(){
  for (int onoff=0;onoff<4;onoff++){
  currentState[onoff] = digitalRead(buttonPin[onoff]);
  if (currentState[onoff] == LOW && lastState[onoff] == HIGH){//if button has just been pressed
    
    //toggle the state of the LED
    if (switchState[onoff] == LOW){
      switchState[onoff] = HIGH;
      
    } else {
      switchState[onoff] = LOW;  
          }
  }//end of if
  lastState[onoff] = currentState[onoff];
    }//end of for
}
//----------------end of on/off buttons-----------------//



//-------------------------midi clock action for internal and external clock----------------------------//
void  MidiClock(){  //do this every time the Timer3 interval timer is triggered
  
  if (masterClock){   //allows clock to be switched off  
     currentTime = micros();
     midIntervall = (1000/(bpm/60.0))/24.0;//set the midi clock using ref to bpm
      
     if ((currentTime - pastTime) > midIntervall*1000) {
      usbMIDI.sendRealTime(CLOCK);

    //---use per channel midi note send-----//   
    sendVoice1();    
    sendVoice2();
    sendVoice3();
    sendVoice4();

      pastTime = currentTime;
   tickCounter++;
   if(tickCounter >=stepNum){tickCounter=0;}
      
     }//end if intervl time hit
    }//end if masterclock

    else{
    sendVoice1();    
    sendVoice2();
    sendVoice3();
    sendVoice4();

   pastTime = currentTime;
   tickCounter++;
   if(tickCounter >=stepNum){tickCounter=0;}
     //   }
    }//end of if not masterclock
}
//-------------------end of midiclock()-----------------------//

void sendVoice1(){
   if (tickCounter%divRatio[channelDiv[0]] == 0) {
 
    tickCount[0]++;    
  if(tickCount[0]>euclidParams[0][0]-1){tickCount[0]=0;}  
   
    for(int sender=0;sender<euclidParams[0][0]+1;sender++){
      if(tickCount[0] == sender-1 && bitRead(euclidOut[0],euclidParams[0][0]-sender)==1 && switchState[0] ==HIGH)
         {
           usbMIDI.sendNoteOn(60,125,1);
           }
    }//end of for
  }   
}

//-----------------
void sendVoice2(){
  if (tickCounter%divRatio[channelDiv[1]] == 0) {

    tickCount[1]++;    
  if(tickCount[1]>euclidParams[1][0]-1){tickCount[1]=0;}  
    
    for(int sender=0;sender<euclidParams[1][0]+1;sender++){
      if(tickCount[1] == sender-1 && bitRead(euclidOut[1],euclidParams[1][0]-sender)==1 && switchState[1] ==HIGH)
         {
           usbMIDI.sendNoteOn(60,125,2);
           }
    }//end of for
  }   
}
//------------------
void sendVoice3(){
   if (tickCounter%divRatio[channelDiv[2]] == 0) {

    tickCount[2]++;
  //reset tickcounter for rotary if over stepcount
  if(tickCount[2]>euclidParams[2][0]-1){tickCount[2]=0;}
    
    for(int sender=0;sender<euclidParams[2][0]+1;sender++){
      if(tickCount[2] == sender-1 && bitRead(euclidOut[2],euclidParams[2][0]-sender)==1 && switchState[2] ==HIGH)
         {
           usbMIDI.sendNoteOn(60,125,3);
           }
    }//end of for

  }   
}
//-----------------
void sendVoice4(){
   if (tickCounter%divRatio[channelDiv[3]] == 0) {

    tickCount[3]++;
  //reset tickcounter for rotary if over stepcount
  if(tickCount[3]>euclidParams[3][0]-1){tickCount[3]=0;}
    
    for(int sender=0;sender<euclidParams[3][0]+1;sender++){
      if(tickCount[3] == sender-1 && bitRead(euclidOut[3],euclidParams[3][0]-sender)==1 && switchState[3] ==HIGH)
         {
           usbMIDI.sendNoteOn(60,125,4);
           }
    }//end of for

  }   
}

//------------------------------------do the leds--------------------//
void doLedBeats(){
//---------set to blue only those steps active, so switch off all first
for(int all=0;all<64;all++){leds.setPixel(all, ALLOFF);}//use ws serial as non blocking

//----------set those leds that are step, to blueish white
for(int rings=0;rings<4;rings++){
    for(int onSteps=0;onSteps<euclidParams[rings][0];onSteps++){leds.setPixel(15-onSteps+(16*rings), WHITE2);}
}


//------------do the leds for the hits
for(int allRot=0;allRot<4;allRot++){
    for(int beats = 0; beats<16; beats++){
      if(bitRead(euclidOut[allRot],beats)==1)
          {leds.setPixel((15+(16*allRot))-(euclidParams[allRot][0]-beats-1), RED);} 
    }
}
//-----------do the leds for the step indicator
for(int xx=0;xx<4;xx++){
     if(switchState[xx]==1){ leds.setPixel (  15 -(tickCount[xx])    +(16*xx)  , BLUE);}
     else{leds.setPixel(  15 -(tickCount[xx])    +(16*xx)  , GREEN );}
      }
         
//----call to show leds only once, reduces overhead     
        leds.show();
}
//---------------------end of doLedBeats-----------------//

//-------------------update the display------------//
void updateScreen1(){
for(int a=0;a<3;a++){
  for(int b=0;b<4;b++){
   writeOut(euclidParams[b][a],screeny[a],screenx[b]);     
}}

display.display();
display.clearDisplay();
}

//-------------------update screen for clock divider changes----------//
void updateScreenDIV(){

  for(int b=0;b<4;b++){
  display.setTextSize(1);
  display.setCursor(screenx[b],0);
  display.print(screenDIV[channelDiv[b]]);
 }
  display.setTextSize(2);
  display.setCursor(0,30);
  display.print("Clck = ");
  display.print(extIntClock[BPMExternal]);
 
  display.setTextSize(1);
  display.setCursor(0,50);
  display.print("INT BPM= ");
  display.setTextSize(2);
  display.print(bpm);

display.display();
display.clearDisplay();
}

//-------------------update screen for memory action ----------//
void updateScreenMem(){

  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("mem slot = ");
  display.setTextSize(2);
  display.setCursor(0,17);
  display.print(memSlot);

  display.setTextSize(1);
  display.setCursor(0,37);
   display.print("button 1 - write");
  display.setCursor(0,50);
   display.print("button 2 - read");
      
    display.display();
    display.clearDisplay();

}
//---------------read from memory action---------------------//
void updateScreenFromMem(){
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("load mem = ");
  display.setTextSize(2);
  display.setCursor(0,17);
  display.print(memSlot);
display.display();
display.clearDisplay();
}

//------------------------- Euclid calculation functions ------------------------------------//
//--- the three funcitons below are taken directly from http://clsound.com/euclideansequenc.html ---//
//--- acknowledgment to Craig Lee ----------------------------------------------------------//

//------------Function to right rotate n by d bits---------------------------------//
uint16_t rightRotate(int shift, uint16_t value, uint8_t pattern_length) {
  uint16_t mask = ((1 << pattern_length) - 1);
  value &= mask;
  return ((value >> shift) | (value << (pattern_length - shift))) & mask;
}

//----1---------Function to find the binary length of a number by counting bitwise-------//
int findlength(unsigned int bnry) {
  boolean lengthfound = false;
  int length = 1; // no number can have a length of zero - single 0 has a length of one, but no 1s for the sytem to count
  for (int q = 32; q >= 0; q--) {
    int r = bitRead(bnry, q);
    if (r == 1 && lengthfound == false) {
      length = q + 1;
      lengthfound = true;
    }
  }
  return length;
}

//-----2--------Function to concatenate two binary numbers bitwise----------------------//
unsigned int ConcatBin(unsigned int bina, unsigned int binb) {
  int binb_len = findlength(binb);
  unsigned int sum = (bina << binb_len);
  sum = sum | binb;
  return sum;
}

//------3-------------------Euclidean bit sorting funciton-------------------------------//
unsigned int euclid(int n, int k, int o) { // inputs: n=total, k=beats, o = offset
  int pauses = n - k;
  int pulses = k;
  int offset = o;
  int steps = n;
  int per_pulse = pauses / k;
  int remainder = pauses % pulses;
  unsigned int workbeat[n];
  unsigned int outbeat;
  uint16_t outbeat2;
  int workbeat_count = n;
  int a;
  int b;
  int trim_count;

  for (a = 0; a < n; a++) { // Populate workbeat with unsorted pulses and pauses
    if (a < pulses) {
      workbeat[a] = 1;
    }
    else {
      workbeat[a] = 0;
    }
  }

  if (per_pulse > 0 && remainder < 2) { // Handle easy cases where there is no or only one remainer
    for (a = 0; a < pulses; a++) {
      for (b = workbeat_count - 1; b > workbeat_count - per_pulse - 1; b--) {
        workbeat[a] = ConcatBin(workbeat[a], workbeat[b]);
      }
      workbeat_count = workbeat_count - per_pulse;
    }

    outbeat = 0; // Concatenate workbeat into outbeat - according to workbeat_count
    for (a = 0; a < workbeat_count; a++) {
      outbeat = ConcatBin(outbeat, workbeat[a]);
    }

    if (offset > 0) {
      outbeat2 = rightRotate(offset, outbeat, steps); // Add offset to the step pattern
    }
    else {
      outbeat2 = outbeat;
    }

    return outbeat2;
  }

  else {
    if (pulses == 0) {
      pulses = 1;  // Prevent crashes when k=0 and n goes from 0 to 1
    }
    int groupa = pulses;
    int groupb = pauses;
    int iteration = 0;
    if (groupb <= 1) {
    }

    while (groupb > 1) { //main recursive loop
      
      if (groupa > groupb) { // more Group A than Group B
        int a_remainder = groupa - groupb; // what will be left of groupa once groupB is interleaved
        trim_count = 0;
        for (a = 0; a < groupa - a_remainder; a++) { //count through the matching sets of A, ignoring remaindered
          workbeat[a] = ConcatBin(workbeat[a], workbeat[workbeat_count - 1 - a]);
          trim_count++;
        }
        workbeat_count = workbeat_count - trim_count;

        groupa = groupb;
        groupb = a_remainder;
      }

      else if (groupb > groupa) { // More Group B than Group A
        int b_remainder = groupb - groupa; // what will be left of group once group A is interleaved
        trim_count = 0;
        for (a = workbeat_count - 1; a >= groupa + b_remainder; a--) { //count from right back through the Bs
          workbeat[workbeat_count - a - 1] = ConcatBin(workbeat[workbeat_count - a - 1], workbeat[a]);

          trim_count++;
        }
        workbeat_count = workbeat_count - trim_count;
        groupb = b_remainder;
      }

      else if (groupa == groupb) { // groupa = groupb
        trim_count = 0;
        for (a = 0; a < groupa; a++) {
          workbeat[a] = ConcatBin(workbeat[a], workbeat[workbeat_count - 1 - a]);
          trim_count++;
        }
        workbeat_count = workbeat_count - trim_count;
        groupb = 0;
      }

      else {
        //Serial.println("ERROR");
      }
      iteration++;
    }

    outbeat = 0; // Concatenate workbeat into outbeat - according to workbeat_count
    for (a = 0; a < workbeat_count; a++) {
      outbeat = ConcatBin(outbeat, workbeat[a]);
    }

    if (offset > 0) {
      outbeat2 = rightRotate(offset, outbeat, steps); // Add offset to the step pattern
    }
    else {
      outbeat2 = outbeat;
    }

    return outbeat2;
  }
}
//------------------end euclidian math-------------------------------------//

//------------------------------------------encoder process------------//
 void encoderProcess(){

    for(int sw=0;sw<4;sw++){
    swProcess(sw);
      if(oldMode[sw] != mode[sw]){
      oldMode[sw] = mode[sw];
      ledProcess(sw);
      }
    }

    
for(int enc=0;enc<4;enc++){
if(mode[enc]==1){ //1=step number
  rotEncProcess(enc); 
    if(oldVal1[enc] != valRot1[enc]){
      oldVal1[enc] = valRot1[enc];
      euclidParams[enc][0] = oldVal1[enc]; //puts the step number from oldVal1 into the euclid array
    updateScreen1();
    }
} }

for(int enc=0;enc<4;enc++){
if(mode[enc]==2){ //2 = beats
  rotEncProcess2(enc); 
    if(oldVal2[enc] != valRot2[enc]){
      oldVal2[enc] = valRot2[enc];
      euclidParams[enc][1] = oldVal2[enc]; //puts the beat number from oldVal1 into the euclid array
   updateScreen1();
    }
}  } 
for(int enc=0;enc<4;enc++){
if(mode[enc]==3){ //3 = offset amount
  rotEncProcess3(enc); 
    if(oldVal3[enc] != valRot3[enc]){
      oldVal3[enc] = valRot3[enc];
      euclidParams[enc][2] = oldVal3[enc]; //puts the offset number from oldVal1 into the euclid array
     updateScreen1();
        }
     } }

for(int enc=0;enc<4;enc++){
if(mode[enc]==0){ //0 = DIV number
  rotEncProcess4(enc); 
    if(oldVal4[enc] != valRot4[enc]){
      oldVal4[enc] = valRot4[enc];
      channelDiv[enc] = oldVal4[enc]; //puts the div number from oldVal1 into the euclid array
     updateScreenDIV();
        }
     } }   

//for(int enc=0;enc<4;enc++){
if(mode[3]==4){ //4 = BPM number for forth rotary 
  rotEncProcess5(); 
    if(oldVal5[5] != valRot5[5]){
      oldVal5[5] = valRot5[5];
      bpm = oldVal5[5]; //
     updateScreenDIV();
        }
     }
     
if(mode[0]==4) //&& digitalRead(buttonPin[0]==HIGH))
{ //4 = memory action on first rotary 
//copy divs to mem slot
for(int mem=0;mem<4;mem++){
  memChannelDIV[mem]=channelDiv[mem];
  ONOFF[mem]=switchState[mem];

}

   rotEncProcessMem(); //read the rotary pins and update
   
    if(oldVal6[0] != valRot6[0]){
      oldVal6[0] = valRot6[0];
      memSlot = oldVal6[0]; //
    
    updateScreenMem();
     }

//-------------if writing to memory----------//
  static uint8_t lastBtnState1 = HIGH;
  uint8_t state1 = digitalRead(buttonPin[0]);
  if (state1 != lastBtnState1) {
    lastBtnState1 = state1;
    if (state1 == LOW) {
       writeMemory(memSlot);}
    }
//-------------if reading from memory----------//
  static uint8_t lastBtnState2 = HIGH;
  uint8_t state2 = digitalRead(buttonPin[1]);
  if (state2 != lastBtnState2) {
    lastBtnState2 = state2;
    if (state2 == LOW) {
       readMemory(memSlot);}
       //-----put div values and onoff values into current
      for(int mem=0;mem<4;mem++){
        channelDiv[mem]=memChannelDIV[mem];
        switchState[mem]=ONOFF[mem];
      } 
    }
  }
 }
//---------end encoder process----------//
