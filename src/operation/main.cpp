//from Thomas's repo: 

#include <math.h>
#include <Arduino.h>

// pin definitions
const byte LEFTPIN = 0;
const byte RIGHTPIN = 2;
const byte BACKPIN = 3;
const byte ENTERPIN = 4;
const byte LEADPIN = 5;
const byte LEADDIRPIN = 6;
const byte CHUCKPIN = 7;
const byte CHUCKDIRPIN = 8;
const byte MS1 = 9;
const byte MS2 = 10;
const byte MS3 = 11;



const int DELAY = 500;
const double WLX = 2.0;

byte move_state = 0;
byte move_state_last = 1;
byte ent_state = 0;
byte ent_state_last = 0;
byte back_state = 0;
byte back_state_last = 0;

byte program_state = 0;
byte program_state_last = 0;

int rawCount = 0;
int totalCount = 0;
int cSpeed = 300; //ms of step pulse for chuck
int lSpeed = 100; //ms of step pulse for lead
int cOffset = 1200 - 2*cSpeed;
int lOffset = 400 - 2*lSpeed;
byte followerStepIndex = 0;
byte wireGaugeRatio = 5;


double l, P0, d, n, wrwl, NT, lT, nl;

struct calcOut {
    double wrwl;
    double NT;
    double lT;
    double nl;
};

struct calcOut calcPreamble(double l, double P0, double d, int n)
{
  
  double wrwl, NT, lT, nl;
  double pi = 3.14159;
  struct calcOut result;
  
  result.nl = l/d;
  result.NT = n/nl;
  
  int NT_floor = floor(NT);
  
  double sum, partial;
  sum = double(NT_floor)*(P0 + pi*d*(1.0 + sqrt(3.0)/2.0)*((double(NT_floor)+1.0)/2.0 - 1));
  partial = (NT - NT_floor)*nl*(P0 + pi*d*(1 +  sqrt(3.0)/2.0*NT));        
  
  result.lT = sum + partial;
  
  result.wrwl = 1.0/(WLX*d);
  
  return result;
}
  
void step(byte pin, int ms, int offset){
  digitalWrite(pin,HIGH);
  delayMicroseconds(ms);
  digitalWrite(pin,LOW);
  delayMicroseconds(ms);
  delayMicroseconds(offset);
}

void setup()
{
  pinMode(LEFTPIN, INPUT);
  pinMode(RIGHTPIN, INPUT);
  pinMode(BACKPIN, INPUT);
  pinMode(ENTERPIN, INPUT);
  pinMode(CHUCKPIN, OUTPUT);
  pinMode(CHUCKDIRPIN,OUTPUT);
  pinMode(LEADPIN, OUTPUT);
  pinMode(LEADDIRPIN, OUTPUT);
  pinMode(MS1,OUTPUT);
  pinMode(MS2,OUTPUT);
  pinMode(MS3,OUTPUT);
  

  l = 400; //millimeters
  P0 = 650;
  d = 0.914;
  n = 10;

  Serial.begin(9600);

  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,HIGH);
}

void loop()
{
  move_state = (1-digitalRead(LEFTPIN))+(1-digitalRead(RIGHTPIN))*2; // 1 = left button press, 2 = right button press
  ent_state = 1-digitalRead(ENTERPIN); 
  back_state = 1-digitalRead(BACKPIN);
  switch (program_state)
  {
    ///==========================================STATE 0: JOG=======================================================================
    case 0:
    digitalWrite(CHUCKDIRPIN,HIGH);

    if(move_state){
      if(move_state == 1) digitalWrite(LEADDIRPIN,LOW);
      if(move_state == 2) digitalWrite(LEADDIRPIN,HIGH);

      for(int i = 0; i < 50*wireGaugeRatio; i++){
        if(followerStepIndex >= wireGaugeRatio){
          step(LEADPIN,lSpeed, lOffset);
          if(!ent_state) step(CHUCKPIN,cSpeed, cOffset);
          followerStepIndex = 0;
          
        }
        else{
          step(LEADPIN,lSpeed, lOffset);
        }
        followerStepIndex++;
      }
      followerStepIndex = 0;
      if(!ent_state) rawCount++;
    }
      
    if(back_state){
      rawCount = rawCount - 4;
      delay(500);
    }

    Serial.print((float)rawCount / 4.0);
    Serial.println(" turns");
    
    break;

    ///====================================STATE 1: Test buttons~~~=============================================
    case 1:


    if(move_state){
      if(move_state == 1) digitalWrite(LEADDIRPIN,LOW);
      if(move_state == 2) digitalWrite(LEADDIRPIN,HIGH);

      for(int i = 0; i < 40; i++){
        step(LEADPIN,lSpeed, lOffset);
      }
      
    }


    /*
    if(move_state != move_state_last && move_state == 0){
      move_state_last = move_state;
    }
    */

    /*
    byte left = digitalRead(LEFTPIN);
    byte right = digitalRead(RIGHTPIN);
    byte back = digitalRead(BACKPIN);
    byte enter = digitalRead(ENTERPIN);
    
    Serial.print("Left Button: ");
    Serial.println(left);
    Serial.print("Right Button: ");
    Serial.println(right);
    Serial.print("Back Button: ");
    Serial.println(back);
    Serial.print("Enter Button: ");
    Serial.println(enter);

    Serial.print("move_state: ");
    Serial.println(move_state);

    Serial.print("ent_state: ");
    Serial.println(ent_state);
    delay(1000);
    */
    break;

    ///===================================CASE 2: Jog Lead===============================
    case 2:

    if(move_state){
      if(move_state == 1) digitalWrite(CHUCKDIRPIN,LOW);
      if(move_state == 2) digitalWrite(CHUCKDIRPIN,HIGH);

      for(int i = 0; i < 40; i++){
        step(CHUCKPIN,cSpeed,cOffset);
      }
      
    }

    if(ent_state) {
      if(cSpeed > 300) cSpeed--;
      delay(10);
    }
    else if(back_state){
      cSpeed++;
      delay(10);
    }
    Serial.println(cSpeed);

    break;

  //============================= CASE 3: chuck step / rotation test ==============================
    case 3:
    if(move_state){
      if(move_state == 1){

      }
      for(int i = 0; i < 200; i++){
        step(LEADPIN,lSpeed,lOffset);
      }
      delay(1000);
    }


    break;
  }

  
}