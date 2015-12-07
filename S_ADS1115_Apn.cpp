#include "S_ADS1115_Apn.h"

#include <Wire.h>

bool S_ADS1115_Apn::begin(uint8_t address)
{
	uint16_t test = 0;
	_ads1115Address = address;
	Wire.begin();

	//S_ADS1115_Apn::ADS1115_Write(ADS1115_REGIS_CONFIG,);

	test = S_ADS1115_Apn::ADS1115_Read(ADS1115_REGIS_CONFIG);
	//Serial.println(test,BIN);
	if(test == 0)
		return false;
	else
		return true;
}
uint16_t S_ADS1115_Apn::read(uint8_t channel)
{
	if(channel > 3)
		return 0;

	uint16_t config = ADS1115_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
					  ADS1115_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
					  ADS1115_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
					  ADS1115_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
					  ADS1115_CONFIG_DR_860SPS    | // 250 samples per second (default)
					  ADS1115_CONFIG_MODE_SINGLE;
					  

	config |= ADS1115_CONFIG_PGA_GAIN_TWO;

	switch (channel)
	{
	case (0):
	  config |= ADS1115_CONFIG_MUX_SINGLE_0;  break;
	case (1):
	  config |= ADS1115_CONFIG_MUX_SINGLE_1;  break;
	case (2):
	  config |= ADS1115_CONFIG_MUX_SINGLE_2;  break;
	case (3):
	  config |= ADS1115_CONFIG_MUX_SINGLE_3;  break;
	}
	//Serial.println(channel);
	// Set 'start single-conversion' bit
	config |= ADS1115_CONFIG_OS_START;

	S_ADS1115_Apn::ADS1115_Write(ADS1115_REGIS_CONFIG, config);
	
	//delay(8);

	return S_ADS1115_Apn::ADS1115_Read(ADS1115_REGIS_CONVER);
}

void S_ADS1115_Apn::ADS1115_Write(uint8_t regis, uint16_t data)
{
	Wire.beginTransmission(_ads1115Address);
   
    Wire.write((uint8_t)regis);

    Wire.write((uint8_t)(data>>8));
    Wire.write((uint8_t)data & 0xFF);

    Wire.endTransmission();
}

uint16_t S_ADS1115_Apn::ADS1115_Read(uint8_t reg)
{
	Wire.beginTransmission(_ads1115Address);
    
    Wire.write((uint8_t)reg);

    Wire.endTransmission();
    Wire.requestFrom(_ads1115Address, (uint8_t)2);
    return (Wire.read() << 8 | Wire.read());
}
