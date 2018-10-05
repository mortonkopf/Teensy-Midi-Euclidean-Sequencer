#include <Bounce2.h>

// need six pins for each rotary
int rotaryPins[4][6]={ // left rot, right rot, red pin, green pin, blue pin, switch pin
  {2,4,8,7,6,3},         //first rotary
  {11,12,14,15,16,17}, 
  {25,24,26,27,28,31}, //pins 29 and 30 saved for timer3
  {34,35,36,37,38,39}
};

int euclidParams[4][3] = {  //(int totalsteps, int beats or hits, int offset) 
  {16,1,0},
  {16,1,0},
  {16,1,0},
  {16,1,0}
};
unsigned int euclid(int totalSteps, int beats, int offset1);
int euclidOut[4] = {0,0,0,0};  //used to store the output of the euclidean function

//for blink without delay for avoiding while() in sw process (rotary button push)
unsigned long previousMillis = 0;
unsigned long pushInterval = 300;
unsigned long currentMillis;


// ROT-ENC CONSTANTS
#define  ROT_MAX 16
#define  ROT_MIN 1
//#define  ROT_d   1
#define  ROT_center 1
#define ROT_start 16

#define OFFSET_MIN 0
#define OFFSET_MAX 16
#define OFFSET_START 0

#define DIV_MIN 0
#define DIV_MAX 5
#define DIV_START 3

#define BPM_MIN 40
#define BPM_MAX 200
#define BPM_START 120

#define SLOT_MIN 0
#define SLOT_MAX 32
#define SLOT_START 1


int ROT_d[7] ={1,1,1,1,1,1,1};
int ROT_STAY[7] ={3,3,3,3,3,3,3};
int ROT_LEFT[7] ={1,1,1,1,1,1,1};
int ROT_RIGHT[7] ={2,2,2,2,2,2,2};

int val1a[5] = {0,0,0,0,0};
int val1b[5] = {0,0,0,0,0};
int oldVal1[5] ={-1,-1,-1,-1,-1};
int valRot1[5] ={ROT_start,ROT_start,ROT_start,ROT_start,ROT_start};
int dirRot1[5] ={3,3,3,3,3};

int val2a[5] = {0,0,0,0,0};
int val2b[5] = {0,0,0,0,0};
int oldVal2[5] ={-1,-1,-1,-1,-1};
int valRot2[5] ={ROT_center,ROT_center,ROT_center,ROT_center,ROT_center};
int dirRot2[5] ={3,3,3,3,3};

int val3a[5] = {0,0,0,0,0};
int val3b[5] = {0,0,0,0,0};
int oldVal3[5] ={-1,-1,-1,-1,-1};
int valRot3[5] ={OFFSET_START,OFFSET_START,OFFSET_START,OFFSET_START,OFFSET_START};
int dirRot3[5] ={3,3,3,3,3};

int val4a[5] = {0,0,0,0,0};
int val4b[5] = {0,0,0,0,0};
int oldVal4[5] ={-1,-1,-1,-1,-1};
int valRot4[5] ={DIV_START,DIV_START,DIV_START,DIV_START,DIV_START};
int dirRot4[5] ={3,3,3,3,3};

int val5a[6] = {0,0,0,0,0,0};
int val5b[6] = {0,0,0,0,0,0};
int oldVal5[6] ={-1,-1,-1,-1,-1,-1};
int valRot5[6] ={BPM_START,BPM_START,BPM_START,BPM_START,BPM_START,BPM_START};
int dirRot5[6] ={3,3,3,3,3,3};

int val6a[5] = {0,0,0,0,0};
int val6b[5] = {0,0,0,0,0};
int oldVal6[5] ={-1,-1,-1,-1,-1};
int valRot6[5] ={0,0,0,0,0};
int dirRot6[5] ={3,3,3,3,3};

int mode[6] = {1,1,1,1,1,1}; //four switches, each can be 0,1,2, or 3
int oldMode[6] = {-1,-1,-1,-1,-1};

String extIntClock[2] = {"ext","int"};


// Instantiate a Bounce object
Bounce debouncer1 = Bounce(); 
Bounce debouncer2 = Bounce(); 
Bounce debouncer3 = Bounce(); 
Bounce debouncer4 = Bounce(); 

//----------------------------used in setup, just here to clean the main page up-------------//
//------for rotary encoders------------//     
void setEncoders(){
   pinMode(rotaryPins[0][0],INPUT);
   pinMode(rotaryPins[0][1],INPUT);
   pinMode(rotaryPins[0][5],INPUT);
   digitalWrite(rotaryPins[0][0],HIGH);
   digitalWrite(rotaryPins[0][1],HIGH);
   pinMode(rotaryPins[1][0],INPUT);
   pinMode(rotaryPins[1][1],INPUT);
   pinMode(rotaryPins[1][5],INPUT);
   digitalWrite(rotaryPins[1][0],HIGH);
   digitalWrite(rotaryPins[1][1],HIGH); 

   pinMode(rotaryPins[2][0],INPUT);
   pinMode(rotaryPins[2][1],INPUT);
   pinMode(rotaryPins[2][5],INPUT);
   digitalWrite(rotaryPins[2][0],HIGH);
   digitalWrite(rotaryPins[2][1],HIGH);
   pinMode(rotaryPins[3][0],INPUT);
   pinMode(rotaryPins[3][1],INPUT);
   pinMode(rotaryPins[3][5],INPUT);
   digitalWrite(rotaryPins[3][0],HIGH);
   digitalWrite(rotaryPins[3][1],HIGH); 
    
  debouncer1.attach(2);
  debouncer1.interval(15); // interval in ms
   debouncer2.attach(4);
  debouncer2.interval(15); // interval in ms
    debouncer3.attach(11);
  debouncer3.interval(15); // interval in ms
    debouncer4.attach(12);
  debouncer4.interval(15); // interval in ms

}



//---------------method to flash the rotary encoder when playing a beat-------------//
void startRot() {
for(int x=0; x<4;x++){
 //RED
          analogWrite(rotaryPins[x][2],0);
          analogWrite(rotaryPins[x][3],255);
          analogWrite(rotaryPins[x][4],255);
delay(200); //GREEN
          analogWrite(rotaryPins[x][2],255);
          analogWrite(rotaryPins[x][3],0);
          analogWrite(rotaryPins[x][4],255);
delay(200);  //BLUE
          analogWrite(rotaryPins[x][2],225);
          analogWrite(rotaryPins[x][3],225);
          analogWrite(rotaryPins[x][4],0);
delay(200);  //BLUE

}
}

void ledProcess(int x){

      switch(mode[x]){
         case 0: //GREEN
          analogWrite(rotaryPins[x][2],255);
          analogWrite(rotaryPins[x][3],0);
          analogWrite(rotaryPins[x][4],255);
          break;
        case 1: //BLUE
          analogWrite(rotaryPins[x][2],255);
          analogWrite(rotaryPins[x][3],255);
          analogWrite(rotaryPins[x][4],0);
          break;         
          case 2: //RED
          analogWrite(rotaryPins[x][2],0);
          analogWrite(rotaryPins[x][3],255);
          analogWrite(rotaryPins[x][4],255);
          break;
        case 3: //YELLOW
          analogWrite(rotaryPins[x][2],0);
          analogWrite(rotaryPins[x][3],0);
          analogWrite(rotaryPins[x][4],255);
          break;
        case 4: //YELLOW
          analogWrite(rotaryPins[x][2],0);
          analogWrite(rotaryPins[x][3],255);
          analogWrite(rotaryPins[x][4],0);
          break;
      }      
}


//--------for steps-------//
void rotEncProcess(int x)
{
  val1a[x] = digitalRead( rotaryPins[x][0] );
  val1b[x] = digitalRead( rotaryPins[x][1]);
  delay(1);

// Rotary Encoder 1
  if( val1a[x] == HIGH && val1b[x] == HIGH ){
    if( dirRot1[x] == ROT_LEFT[x] ){
        valRot1[x]+=ROT_d[x];
        if(valRot1[x]>ROT_MAX){valRot1[x]=ROT_MAX;}
    }else if( dirRot1[x] == ROT_RIGHT[x] ){
        valRot1[x] -=ROT_d[x];
        if(valRot1[x]<ROT_MIN){valRot1[x]=ROT_MIN;}
    }
    dirRot1[x] = ROT_STAY[x];
  } else {
    if( val1a[x] == LOW ){
      dirRot1[x] = ROT_LEFT[x];
    }
    if( val1b[x] == HIGH ){
      dirRot1[x] = ROT_RIGHT[x];
    }
  }
}
//----------------------for beats/hits-----------//
void rotEncProcess2(int x)
{
  val2a[x] = digitalRead( rotaryPins[x][0] );
  val2b[x] = digitalRead( rotaryPins[x][1] );
  delay(1);

// Rotary Encoder 2
  if( val2a[x] == HIGH && val2b[x] == HIGH ){
    if( dirRot2[x] == ROT_LEFT[x] ){
        valRot2[x]+=ROT_d[x];
        if(valRot2[x]>ROT_MAX){valRot2[x]=ROT_MAX;}
    }else if( dirRot2[x] == ROT_RIGHT[x] ){
        valRot2[x] -=ROT_d[x];
        if(valRot2[x]<ROT_MIN){valRot2[x]=ROT_MIN;}
    }
    dirRot2[x] = ROT_STAY[x];
  } else {
    if( val2a[x] == LOW ){
      dirRot2[x] = ROT_LEFT[x];
    }
    if( val2b[x] == HIGH ){
      dirRot2[x] = ROT_RIGHT[x];
    }
  }
}

//----------------------for offset-----------//
void rotEncProcess3(int x) //for offset
{
  val3a[x]= digitalRead( rotaryPins[x][0] );
  val3b[x] = digitalRead( rotaryPins[x][1] );
  delay(1);

// Rotary Encoder 3
  if( val3a[x] == HIGH && val3b[x] == HIGH ){
    if( dirRot3[x] == ROT_LEFT[x] ){
        valRot3[x]+=ROT_d[x];
        if(valRot3[x]>OFFSET_MAX){valRot3[x]=OFFSET_MAX;}
    }else if( dirRot3[x] == ROT_RIGHT[x] ){
        valRot3[x] -=ROT_d[x];
        if(valRot3[x]<OFFSET_MIN){valRot3[x]=OFFSET_MIN;}
    }
    dirRot3[x] = ROT_STAY[x];
  } else {
    if( val3a[x] == LOW ){
      dirRot3[x] = ROT_LEFT[x];
    }
    if( val3b[x] == HIGH ){
      dirRot3[x] = ROT_RIGHT[x];
    }
  }
}


//----------------------for setting the clock DIV-----------//
void rotEncProcess4(int x) //for offset
{
  val4a[x]= digitalRead( rotaryPins[x][0] );
  val4b[x] = digitalRead( rotaryPins[x][1] );
  delay(1);

// Rotary Encoder 4
  if( val4a[x] == HIGH && val4b[x] == HIGH ){
    if( dirRot4[x] == ROT_LEFT[x] ){
        valRot4[x]+=ROT_d[x];
        if(valRot4[x]>DIV_MAX){valRot4[x]=DIV_MAX;}
    }else if( dirRot4[x] == ROT_RIGHT[x] ){
        valRot4[x] -=ROT_d[x];
        if(valRot4[x]<DIV_MIN){valRot4[x]=DIV_MIN;}
    }
    dirRot4[x] = ROT_STAY[x];
  } else {
    if( val4a[x] == LOW ){
      dirRot4[x] = ROT_LEFT[x];
    }
    if( val4b[x] == HIGH ){
      dirRot4[x] = ROT_RIGHT[x];
    }
  }
}

//----------------------for setting the clock DIV-----------//
void rotEncProcess5() //for offset
{
  val5a[5]= digitalRead( rotaryPins[3][0] );
  val5b[5] = digitalRead( rotaryPins[3][1] );
  delay(1);

// Rotary Encoder 4 -- stored in slot5
  if( val5a[5] == HIGH && val5b[5] == HIGH ){
    if( dirRot5[5] == ROT_LEFT[5] ){
        valRot5[5]+=ROT_d[5];
        if(valRot5[5]>BPM_MAX){valRot5[5]=BPM_MAX;}
    }else if( dirRot5[5] == ROT_RIGHT[5] ){
        valRot5[5] -=ROT_d[5];
        if(valRot5[5]<BPM_MIN){valRot5[5]=BPM_MIN;}
    }
    dirRot5[5] = ROT_STAY[5];
  } else {
    if( val5a[5] == LOW ){
      dirRot5[5] = ROT_LEFT[5];
    }
    if( val5b[5] == HIGH ){
      dirRot5[5] = ROT_RIGHT[5];
    }
  }
}

//----------------------for setting the memory slot-----------//
void rotEncProcessMem() //for offset
{
  val6a[0]= digitalRead( rotaryPins[0][0] );
  val6b[0] = digitalRead( rotaryPins[0][1] );
  delay(1);

// Rotary Encoder 1---stored in slot6
  if( val6a[0] == HIGH && val6b[0] == HIGH ){
    if( dirRot6[0] == ROT_LEFT[6] ){
        valRot6[0]+=ROT_d[6];
        if(valRot6[0]>SLOT_MAX){valRot6[0]=SLOT_MAX;}
    }else if( dirRot6[0] == ROT_RIGHT[6] ){
        valRot6[0] -=ROT_d[6];
        if(valRot6[0]<SLOT_MIN){valRot6[0]=SLOT_MIN;}
    }
    dirRot6[0] = ROT_STAY[6];
  } else {
    if( val6a[0] == LOW ){
      dirRot6[0] = ROT_LEFT[6];
    }
    if( val6b[0] == HIGH ){
      dirRot6[0] = ROT_RIGHT[6];
    }
  }
}


//------------switch process----------//
void swProcess(int x){      
  if( digitalRead(rotaryPins[x][5]) ){
       currentMillis = millis();
         if(currentMillis - previousMillis > pushInterval) {
          // save the last time
            previousMillis = currentMillis;   
      mode[x]++;
      mode[x] %=5;
     }
  }
}
