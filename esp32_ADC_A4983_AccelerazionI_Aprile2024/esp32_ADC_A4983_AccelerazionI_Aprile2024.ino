#include <AccelStepper.h>
#include <MultiStepper.h>

// Stepper con riduttore, rapporto di riduzione 1:5.18

 // Peso bottiglie
 //Test 10KOHm
 // 1: ADC 200, 521 g 
 // 2: ADC 700, 217 g

 //Test 2.2Kohm //
 // 1: ADC 990, 521 g  //1160
 // 2: ADC 1450, 416 g //1250
 // 1: ADC 1740, 321 g //1600
 // 2: ADC 1780, 217 g //1900
 // 1: ADC 3500, 116 g //2800
 
  //Test 2.2Kohm con rondella centrale
 // 1: 521 g  //ADC 1160
 // 2: 416 g //ADC 1250
 // 1: 321 g //ADC 1600
 // 2: 217 g //ADC 1900
 // 1: 116 g //ADC 2800


 #include "mpr121.h"
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define MPR121_POWER_PIN 27 //su scheda TELOS
#define MPR121_GND_PIN 5 //su scheda cora S2
#define MPR121_ADDR 0x5A

#define EN_STEP_PIN 19 
#define STEP_PIN 18
#define DIR_PIN 23

AccelStepper stepperX(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

int i;
int t0,t1;
int step_ref;
int step_time_us;
int steps_cnt = 0;
#define STEP_PULSE_TIME_US 2

//int irqpin = 2;  // Digital 2
boolean touchStates[16]; //to keep track of the previous touch states
uint16_t rawSignals[16];

int adc_pin[4] = {35, 34, 36, 39}; // {36, 34, 39, 35}; //{35, 34, 39, 36};
int adc_sig[4];

float maxSpeed = 500.0;
float acceleration = 500.0;
int pos_ref = 0;
int cnt = 0;

int log_cnt = 0;

void setup(){
  //pinMode(irqpin, INPUT);
  //digitalWrite(irqpin, HIGH); //enable pullup resistor
  //pinMode(MPR121_GND_PIN, OUTPUT);
  //digitalWrite(MPR121_GND_PIN,LOW);
  delay(1000);
  Serial.begin(115200);

  delay(250);
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(250);
  Wire.setClock(400000);   

  pinMode(EN_STEP_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  
  digitalWrite(EN_STEP_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);

  stepperX.setMaxSpeed(maxSpeed);
  stepperX.setAcceleration(acceleration);

  //mpr121_setup(); //DEBUG

  for(i=0;i<4;i++)
    pinMode(adc_pin[i], INPUT);

}

void loop()
{

  if(cnt<=0)
  {
    cnt = 5000;

    t0 = micros();
    //readTouchInputs();
    readRawSignals();

    for(i=0;i<4;i++)
      adc_sig[i] = analogRead(adc_pin[i]);

    t1 = micros();
    //delay(3);
    SerialRead();
    //Serial.printf("%d,%d,%d,%d\n", touchStates[0], touchStates[1], touchStates[2], touchStates[3])
    log_cnt++;
    //Serial.printf("Cnt: %d \tSensors: %d,\t%d,\t%d,\t%d\n", log_cnt, rawSignals[0], rawSignals[2], rawSignals[4], rawSignals[3]);  
    //Serial.printf("%d,%d,%d,%d,%d,%d\n", log_cnt, steps_cnt, rawSignals[0], rawSignals[2], rawSignals[4], rawSignals[3]);  
    
    Serial.printf("%d,%d,%d,%d,%f,%f,%d\n\r",adc_sig[0],adc_sig[1],adc_sig[2],adc_sig[3], stepperX.acceleration(), stepperX.speed(), stepperX.distanceToGo());
    //Serial.printf("acc: %f, speed: %f, distance: %d\n",stepperX.acceleration(), stepperX.speed(), stepperX.distanceToGo());
    //pollice quarto -     

  }
  cnt--;

  stepperX.run();
  delayMicroseconds(20); 

}

void readRawSignals()
{
  int byte_count =  26;
  byte addr = MPR121_ADDR;
  byte *pbyte;
  
  Wire.beginTransmission(addr);
  Wire.write(0x04);
  Wire.endTransmission(false);

  Wire.requestFrom(addr, byte_count); //12 elettrodi + proximity channel
  pbyte = (byte*) &rawSignals[0];
  for (int i=0; i<byte_count; ++i)
  {
    *pbyte = Wire.read();
    pbyte++;
  }
}
void readTouchInputs()
{    
    //read the touch state from the MPR121
    Wire.requestFrom(MPR121_ADDR,2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states
    for (int i=0; i < 12; i++)
    {
      if(touched & (1<<i)){touchStates[i] = 1;}
      else{touchStates[i] = 0;}
    }

    
}




void mpr121_setup(void){

  set_register(0x5A, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, TOU_THRESH);
  set_register(0x5A, ELE0_R, REL_THRESH);
 
  set_register(0x5A, ELE1_T, TOU_THRESH);
  set_register(0x5A, ELE1_R, REL_THRESH);
  
  set_register(0x5A, ELE2_T, TOU_THRESH);
  set_register(0x5A, ELE2_R, REL_THRESH);
  
  set_register(0x5A, ELE3_T, TOU_THRESH);
  set_register(0x5A, ELE3_R, REL_THRESH);
  
  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);
  
  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);
  
  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);
  
  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);
  
  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);
  
  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);
  
  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);
  
  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, 0x5C, 0x18); //Daniele mod: 0x10 default
  set_register(0x5A, FIL_CFG, 0x41); //Daniele mod: era 0x04, 0x24 default
  

  
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(0x5A, ATO_CFG0, 0x0B);
  set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(0x5A, ELE_CFG, 0x0C);
  
}


//boolean checkInterrupt(void){
//  return digitalRead(irqpin);
//}


void set_register(uint8_t address, uint8_t r, uint8_t v)
{
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}
