#include <string.h>
#include <SPI.h>
#include "AFE4490_REGISTER.h"

#define PIN_MISO        12 //data out connect to PIN_MISO
#define PIN_MOSI        11 //data in connect to PIN_MOSI 
#define PIN_SCLK        13 //butuh logic level converter
#define PIN_SPISTE      10 //butuh logic level converter

#define PIN_ADC_RDY     9
#define PIN_ADC_RESET   7

void        AFE4490_Write (uint8_t address, uint32_t data);
uint32_t    AFE4490_Read  (uint8_t address);
void        AFE4490_Init  ();

bool        flagDataReady     = false;
uint16_t    cnt   = 0;
uint32_t    data_RED;
uint32_t    data_IR;

void setup() 
{
    Serial.begin(115200);
    Serial.println(">> Boot.."); 
  
    pinMode (PIN_MISO,      INPUT);
    pinMode (PIN_SPISTE,    OUTPUT);
    pinMode (PIN_SCLK,      OUTPUT);
    pinMode (PIN_MOSI,      OUTPUT);

    pinMode (PIN_ADC_RDY,   INPUT);
    pinMode (PIN_ADC_RESET, INPUT);

    SPI.begin();
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

    delay(1000);
    
    Serial.println(">> AFE4490 init.."); 
    AFE4490_Init();

    Serial.println(">> AttachInterrupt on DUE PIN 9.."); 
    attachInterrupt(9, setFlag, FALLING);

    Serial.println(">> Start #");
}

void setFlag()
{
    flagDataReady = true;
}

void loop() 
{
  if (flagDataReady){
    getData();
    printData();
    flagDataReady = false;
  }
}

//.................................... Function .....................................// 
void getData()
{
  static uint32_t temp_IR = 0;
  static uint32_t temp_RED = 0;
  if (cnt == 0){
    temp_IR   = AFE4490_Read(LED1ABSVAL);
    temp_RED  = AFE4490_Read(LED2ABSVAL); 
    cnt = 1;
  }
  else if(cnt == 1){
    data_IR     = (temp_IR+AFE4490_Read(LED1ABSVAL))/2;
    data_RED    = (temp_RED+AFE4490_Read(LED2ABSVAL))/2;
    cnt = 2;
  }
}

void printData()
{
  if (cnt == 2){
    Serial.write('#');
    Serial.write('S');
    write4byte(data_RED);
    write4byte(data_IR);
    Serial.write('\n');
    cnt = 0;
  }
//    Serial.print(data_RED); 
////    Serial.print(",");
//    Serial.print("\t");
//    Serial.println(data_IR);  
}


void AFE4490_Write (uint8_t address, uint32_t data)
{
    // if (digitalRead (PIN_MISO) == LOW){
    //     Serial.println ("SOMI is not connected!!");
    // }      
    digitalWrite (PIN_SPISTE, LOW);
    SPI.transfer (address);
    SPI.transfer ((data >> 16) & 0xFF);
    SPI.transfer ((data >>  8) & 0xFF);
    SPI.transfer (data & 0xFF);
    digitalWrite (PIN_SPISTE, HIGH); 
}

uint32_t AFE4490_Read (uint8_t address)
{       
    uint32_t data = 0;
    digitalWrite (PIN_SPISTE, LOW);
    SPI.transfer (address);
    data |= ( SPI.transfer (0) << 16 );
    data |= ( SPI.transfer (0) <<  8 );
    data |=   SPI.transfer (0);
    digitalWrite (PIN_SPISTE, HIGH);
    return data;
}

void AFE4490_Init() 
{
    // According to Example Clock Edge Mapping to SPI Registers
    // AFE4490 datasheet December, 2012 Revised October 2014 Page.36
    // This configuration set PRF 500Hz and duty cycle 25%
    // The sysclk of AFE4490 = 4MHz;
    // PRF 500Hz = 8000 cycles
    // Note: ALED2 = Ambient LED2; LED2 = RED LED
    //       ALED1 = Ambient LED1; LED1 = IR LED
    // the pulse cycle start from: ALED2 > LED1  > ALED1 > LED2
    // the conversion start from:  LED2  > ALED2 > LED1  > ALED1
    AFE4490_Write(CONTROL0,      0);
    AFE4490_Write(CONTROL2,      0);
    AFE4490_Write(PRPCOUNT,      7999);

    // ADC Reset count cycle
    AFE4490_Write(ADCRSTCNT0,    0);
    AFE4490_Write(ADCRSTENDCT0,  3);
    AFE4490_Write(ADCRSTCNT1,    2000);
    AFE4490_Write(ADCRSTENDCT1,  2003);
    AFE4490_Write(ADCRSTCNT2,    4000);
    AFE4490_Write(ADCRSTENDCT2,  4003);
    AFE4490_Write(ADCRSTCNT3,    6000);
    AFE4490_Write(ADCRSTENDCT3,  6003);

    // ADC Conversion count cycle
    AFE4490_Write(LED2CONVST,    4);
    AFE4490_Write(LED2CONVEND,   1999);
    AFE4490_Write(ALED2CONVST,   2004);
    AFE4490_Write(ALED2CONVEND,  3999);
    AFE4490_Write(LED1CONVST,    4004);
    AFE4490_Write(LED1CONVEND,   5999);
    AFE4490_Write(ALED1CONVST,   6004);
    AFE4490_Write(ALED1CONVEND,  7999);

    // ALED2 sample count cycle
    AFE4490_Write(ALED2STC,      50);
    AFE4490_Write(ALED2ENDC,     1998);

    AFE4490_Write(LED1LEDSTC,    2000); // LED1 turn on
    AFE4490_Write(LED1STC,       2050); // LED1 sample
    AFE4490_Write(LED1ENDC,      3998); // LED1 stop sample
    AFE4490_Write(LED1LEDENDC,   3999); // LED1 turn off

    // ALED1 sample count cycle
    AFE4490_Write(ALED1STC,      4050);
    AFE4490_Write(ALED1ENDC,     5998);

    AFE4490_Write(LED2LEDSTC,    6000); // LED2 turn on
    AFE4490_Write(LED2STC,       6050); // LED2 sample
    AFE4490_Write(LED2ENDC,      7998); // LED2 stop sample
    AFE4490_Write(LED2LEDENDC,   7999); // LED2 turn off

    AFE4490_Write(CONTROL1, CONTROL1_TIMEREN_SET | (CONTROL1_AVERAGE_MARK & 15));
    AFE4490_Write(TIAGAIN,       0);
    AFE4490_Write(TIA_AMB_GAIN,  0);
    AFE4490_Write(LEDCNTRL,      0x00013030); // LED Bightness config

    AFE4490_Write(CONTROL0, CONTROL0_SPI_READ); // switch to SPI_READ mode

    delay(1000);
}
void write4byte(uint32_t data){
  Serial.write((byte)(data>>24));
  Serial.write((byte)(data>>16));
  Serial.write((byte)(data>>8));
  Serial.write((byte)(data));
}

void write3byte(int32_t data) {
//  Serial.write((byte)(data));
  Serial.write((byte)(data >> 24));
  Serial.write((byte)(data >> 16));
  Serial.write((byte)(data >> 8));
}

void write2byte(int16_t data) {
  Serial.write((byte)(data >> 8));
  Serial.write((byte)(data));
}
