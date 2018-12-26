#include <PriUint64.h>
#include "LowPower.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Addresses
const uint64_t addressSetup[2] = { 0x100000000ALL, 0x1000000000LL }; //read pipe then write pipe
const uint64_t address1[2] = { 0x200000000ALL, 0x2000000000LL };
const uint64_t address2[2] = { 0x300000000ALL, 0x3000000000LL };
const uint64_t address3[2] = { 0x400000000ALL, 0x4000000000LL };
const uint64_t address4[2] = { 0x500000000ALL, 0x5000000000LL };
const uint64_t address5[2] = { 0x600000000ALL, 0x6000000000LL };
const uint64_t address6[2] = { 0x700000000ALL, 0x7000000000LL };
const uint64_t address7[2] = { 0x800000000ALL, 0x8000000000LL };
const uint64_t address8[2] = { 0x900000000ALL, 0x9000000000LL };
const uint64_t address9[2] = { 0xA00000000ALL, 0xA000000000LL };
const uint64_t address10[2] = { 0xB00000000ALL, 0xB000000000LL };

//Channels (2mhz apart, minimum @1mbps)
const int chanSetup = 102;
const int chan1 = 114;
const int chan2 = 106;
const int chan3 = 108;
const int chan4 = 110;
const int chan5 = 112;
const int chan6 = 114;
const int chan7 = 116;
const int chan8 = 118;
const int chan9 = 120;
const int chan10 = 124;

long sleepTimer = 1000;
int wakeTimer = 50;
int wakeBreak = 0;
bool paired=false;
bool initPair=false;

RF24 radio(9, 10); // CE, CSN
int radioSerialr[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //third from last number is power LED status, second from last indicates controller gone to sleep
int radioSerialw[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //third from last number is power LED status, second from last indicates controller gone to sleep
unsigned int rdr=0;
int voltsense=0;
float volts=0.0;

uint64_t rAddress=address1[0];
uint64_t wAddress=address1[1];
int channel=chan1; 

void wakeUp(){
  wakeTimer = 5000;
  wakeBreak = 0;
  delay(20); //Give MCU time to properly come out of sleep to avoid errors
  while (digitalRead(2)==LOW && wakeBreak == 0){
    wakeTimer--;
    Serial.println(wakeTimer); //for debugging
    if (wakeTimer==0) //time to wake up, run below before resuming normal operation
    {
      radioSerialw[13]=0; //reset sleep mode bit to zero
      sleepTimer=50000; //set sleeptimer before going back to normal operation
      radio.powerUp();
      detachInterrupt(0); //in the case of interrupts, only pins 2 and 3 can be used, (1) is pin 3
      while (digitalRead(2)==LOW); //Hold code here to prevent power button pressing game button, start normal operation/break loop on button release
      wakeBreak=1;
    }
    if (wakeTimer < 0){
      wakeTimer=0;
    }
  }
}

void goSleep(){
  radioSerialw[13]=1; //tell reciever the pad had gone to sleep
  radio.stopListening();
  radio.openWritingPipe(wAddress);    
  radio.write(&radioSerialw, sizeof(radioSerialw));
  radio.powerDown();
  attachInterrupt(digitalPinToInterrupt(2), wakeUp, LOW); //Has to be pin d2 on nano
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); //max power save
}

void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.setChannel(channel);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(wAddress);
  radio.openReadingPipe(1,rAddress);
  radio.stopListening();
  pinMode(2, INPUT_PULLUP); //start
  pinMode(3, INPUT_PULLUP); //down
  pinMode(4, INPUT_PULLUP); //left
  pinMode(5, INPUT_PULLUP); //right
  pinMode(6, INPUT_PULLUP); //up
  pinMode(7, INPUT_PULLUP); //A
  pinMode(8, INPUT_PULLUP); //B
  pinMode(14, INPUT_PULLUP); //C
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);

}
void loop() {
//    char test[15];
//sprintf(test,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",radioSerialr[0],radioSerialr[1],radioSerialr[2],radioSerialr[3],radioSerialr[4],radioSerialr[5],radioSerialr[6],radioSerialr[7],radioSerialr[8],radioSerialr[9],radioSerialr[10],radioSerialr[11],radioSerialr[12],radioSerialr[13],radioSerialr[14]);
//Serial.println(test);
//char test[15];
//sprintf(test,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",radioSerialw[0],radioSerialw[1],radioSerialw[2],radioSerialw[3],radioSerialw[4],radioSerialw[5],radioSerialw[6],radioSerialw[7],radioSerialw[8],radioSerialw[9],radioSerialw[10],radioSerialw[11],radioSerialw[12],radioSerialw[13],radioSerialw[14]);
//Serial.println(test); 
//
//char test[15];
//sprintf(test,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",radioSerialw[0],radioSerialw[1],radioSerialw[2],radioSerialw[3],radioSerialw[4],radioSerialw[5],radioSerialw[6],radioSerialw[7],radioSerialw[8],radioSerialw[9],radioSerialw[10],radioSerialw[11],radioSerialw[12],radioSerialw[13],radioSerialw[14]);
//Serial.println(test); 

  
  voltsense=analogRead(7);
  volts = voltsense * (5.0 / 1023.0);
  if (volts < 5.5 && volts > 3.0){
  radioSerialw[12]=1;
  }else{
  radioSerialw[12]=0;
  }
  
  for (int i = 2; i < 9; i++ ){
   if (digitalRead(i) == 1){
    rdr = 0;
   }
   if (digitalRead(i) == 0){
    rdr = 1;
   }
   
   radioSerialw[i-2] = rdr; //Iterate starting at position 2
  }
  for (int i = 14; i < 19; i++ ){
   if (digitalRead(i) == 1){
    rdr = 0;
   }
   if (digitalRead(i) == 0){
    rdr = 1;
   }
   
   radioSerialw[i-7] = rdr; //Iterate starting at position 7
  }
  
  radio.write(&radioSerialw, sizeof(radioSerialw));

  if ((radioSerialw[0]+radioSerialw[1]+radioSerialw[2]+radioSerialw[3]+radioSerialw[4]+radioSerialw[5]+radioSerialw[6]+radioSerialw[7]+radioSerialw[8]+radioSerialw[9]+radioSerialw[10]+radioSerialw[11])==0){
    sleepTimer--;
  }else{
    sleepTimer=50000;
  }

  if (sleepTimer==0){
    goSleep();
  }
  
 //delay(5);
}

