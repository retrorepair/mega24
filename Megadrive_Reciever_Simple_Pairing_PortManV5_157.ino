#include <PriUint64.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//Addresses
const uint64_t addressSetup[2] = { 0x1000000000LL, 0x100000000ALL }; //read pipe then write pipe
const uint64_t address1[2] = { 0x2000000000LL, 0x200000000ALL };
const uint64_t address2[2] = { 0x3000000000LL, 0x300000000ALL };

//Channels (2mhz apart, minimum @1mbps)
const int chanSetup = 102;
const int chan1 = 114;
const int chan2 = 106;

RF24 radio(9, 10); // CE, CSN
volatile int radioSerialr[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //third from last number is power LED status, last two are address/channel (01 etc)
int radioSerialw[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //third from last number is power LED status, last two are address/channel (01 etc)
int led1=2;
int led2=3;
int led1State=LOW;
int led2State=LOW;
int a9999counter=0;
unsigned long previousMillis = 0;
const long setupBlinkPeriod = 1000;
const long initBlinkPeriod = 50;
const long pairedBlinkPeriod = 5;
uint64_t rAddress=address1[0];
uint64_t wAddress=address1[1];
int channel=chan1;

void LEDcontrol() {
  if (radioSerialr[0]==1 || radioSerialr[1]==1 || radioSerialr[2]==1 || radioSerialr[3]==1 || radioSerialr[4]==1 || radioSerialr[5]==1 || radioSerialr[6]==1 || radioSerialr[7]==1){
    PORTD |= (1<<PORTD3);
    PORTC &=~(1<<PORTC6);
  }else{
    PORTD &=~(1<<PORTD3);
    PORTC &=~(1<<PORTC6);     
  }
}
//    unsigned long currentMillis = millis();
//    if (currentMillis - previousMillis >= setupBlinkPeriod){
//      previousMillis = currentMillis;
//      if (led1State == LOW){
//        led1State = HIGH;
//        led2State = LOW;
//      }else{
//        led1State = LOW;
//        led2State = LOW;
//      }
//      digitalWrite(led1, led1State);
//      digitalWrite(led2, led2State);
//    }
//}


void setup() {

  // Initialize NRF24L01
//  Serial.begin(2000000); //for testing, can be removed later
  radio.begin();
  radio.setChannel(channel);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(wAddress);
  radio.openReadingPipe(1, rAddress);
  radio.startListening(); 
  DDRD = B01011100; //PD2+3 OUTPUTS, PD4-PD7 INPUTS
  DDRC = B00111111; //PC0-PC5 OUTPUTS, PD6+7 INPUTS
  PORTC = B11111111;
  PORTD = B11111111;
}

void loop() {
 if (radio.available()) {
    radio.read(&radioSerialr, sizeof(radioSerialr));
 }  
  LEDcontrol();

//char test[15];
//sprintf(test,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",radioSerialr[0],radioSerialr[1],radioSerialr[2],radioSerialr[3],radioSerialr[4],radioSerialr[5],radioSerialr[6],radioSerialr[7],radioSerialr[8],radioSerialr[9],radioSerialr[10],radioSerialr[11],radioSerialr[12],radioSerialr[13],radioSerialr[14]);
//Serial.println(test);
//char test2[15];
//sprintf(test2,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",radioSerialw[0],radioSerialw[1],radioSerialw[2],radioSerialw[3],radioSerialw[4],radioSerialw[5],radioSerialw[6],radioSerialw[7],radioSerialw[8],radioSerialw[9],radioSerialw[10],radioSerialw[11],radioSerialw[12],radioSerialw[13],radioSerialw[14]);
//Serial.println(test2);

if (radioSerialr[1] == 1){
    PORTD &=~(1<<PORTD6);
  }else{
    PORTD |= (1<<PORTD6);
  }
if (radioSerialr[4] == 1){
    PORTD &=~(1<<PORTD4);
  }else{
    PORTD |= (1<<PORTD4);
  }
if (radioSerialr[5] == 1){
    PORTC &=~(1<<PORTC4);
  }else{
    PORTC |= (1<<PORTC4);
  }
if (radioSerialr[0] == 1){
    PORTC &=~(1<<PORTC2);
  }else{
    PORTC |= (1<<PORTC2);
  }
if (radioSerialr[2] == 1){
    PORTC &=~(1<<PORTC0);
  }else{
    PORTC |= (1<<PORTC0);
  }
if (radioSerialr[3] == 1){
    PORTC &=~(1<<PORTC1);
  }else{
    PORTC |= (1<<PORTC1);
  }
if (radioSerialr[6] == 1){
    PORTC &=~(1<<PORTC5);
  }else{
    PORTC |= (1<<PORTC5);
  }
if (radioSerialr[7] == 1){
    PORTC &=~(1<<PORTC3);
  }else{
    PORTC |= (1<<PORTC3);
  }
//delay(1); 
}

