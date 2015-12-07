#include "S_SM9541_Apn.h"

#include <Wire.h>

#define NasalCan_ADDRESS 0x28
#define READ_MODE 0x01
#define WRITE_MODE 0x00

bool S_SM9541_Apn::begin()
{
    Wire.begin();
    Wire.beginTransmission(NasalCan_ADDRESS);
    Wire.write(READ_MODE);
    Wire.endTransmission();
    Wire.requestFrom(NasalCan_ADDRESS, 1);

    if(Wire.read() != 0)
    {
        Wire.endTransmission();
        return true;
    }
    else
    {
        Wire.endTransmission();
        return false;
    }

}

int32_t S_SM9541_Apn::read()
{
    Wire.beginTransmission(NasalCan_ADDRESS);
    Wire.write(READ_MODE);
    Wire.endTransmission();
     
    Wire.requestFrom(NasalCan_ADDRESS, 2);
     
    b = Wire.read();
    //s = b & 0b11000000;//receive the status bit
    d = b & 0b00111111;//receive the first 6 data bit
    d <<= 8;

    b = Wire.read();
    d  = d | b; // receive the last 8 data bit
    
    Wire.endTransmission();

    return d;
}   

/*long S_SM9541_Apn::bcd2dec(long val)
{
  return ((val/16 * 10) + (val % 16));
}*/

