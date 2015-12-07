#ifndef S_ADS1115_H
#define S_ADS1115_H

#include <Arduino.h>
#include <Wire.h>

#define ADS1115_ADDR_GND 			0x48
#define ADS1115_ADDR_VDD 			0x49
#define ADS1115_ADDR_SDA 			0x4A
#define ADS1115_ADDR_SCL 			0x4B

#define ADS1115_REGIS_CONVER 		0x00
#define ADS1115_REGIS_CONFIG 		0x01
#define ADS1115_REGIS_LO_THRES 		0x02
#define ADS1115_REGIS_HI_THRES 		0x03

/*#define READ_MODE 1
#define WRITE_MODE 0*/

//________________________________________________

#define ADS1115_CONFIG_OS_START				0x8000 		//Always set to 1 to begin conversion
#define ADS1115_CONFIG_OS_STOP 				0x0000

#define ADS1115_CONFIG_MUX_DIFF_0_1 		0x0000  // Diff P = AIN0, N = AIN1 (default)
#define ADS1115_CONFIG_MUX_DIFF_0_3 		0x1000  // Diff P = AIN0, N = AIN3
#define ADS1115_CONFIG_MUX_DIFF_1_3 		0x2000  // Diff P = AIN1, N = AIN3
#define ADS1115_CONFIG_MUX_DIFF_2_3 		0x3000  // Diff P = AIN2, N = AIN3
#define ADS1115_CONFIG_MUX_SINGLE_0 		0x4000  // Single-ended AIN0
#define ADS1115_CONFIG_MUX_SINGLE_1 		0x5000  // Single-ended AIN1
#define ADS1115_CONFIG_MUX_SINGLE_2 		0x6000  // Single-ended AIN2
#define ADS1115_CONFIG_MUX_SINGLE_3 		0x7000  // Single-ended AIN3

#define ADS1115_CONFIG_PGA_GAIN_TWOTHIRDS  	0x0000  // +/-6.144V range = Gain 2/3
#define ADS1115_CONFIG_PGA_GAIN_ONE        	0x0200  // +/-4.096V range = Gain 1
#define ADS1115_CONFIG_PGA_GAIN_TWO        	0x0400  // +/-2.048V range = Gain 2 (default)
#define ADS1115_CONFIG_PGA_GAIN_FOUR       	0x0600  // +/-1.024V range = Gain 4
#define ADS1115_CONFIG_PGA_GAIN_EIGHT      	0x0800  // +/-0.512V range = Gain 8
#define ADS1115_CONFIG_PGA_GAIN_SIXTEEN    	0x0A00  // +/-0.256V range = Gain 16

#define ADS1115_CONFIG_MODE_CONTIN  		0x0000  // Continuous conversion mode
#define ADS1115_CONFIG_MODE_SINGLE  		0x0100  // Power-down single-shot mode (default)

#define ADS1115_CONFIG_DR_8SPS    			0x0000  
#define ADS1115_CONFIG_DR_16SPS    			0x0020  
#define ADS1115_CONFIG_DR_32SPS    			0x0040  
#define ADS1115_CONFIG_DR_64SPS   			0x0060  
#define ADS1115_CONFIG_DR_128SPS  			0x0080  
#define ADS1115_CONFIG_DR_250SPS   			0x00A0 
#define ADS1115_CONFIG_DR_475SPS   			0x00C0  
#define ADS1115_CONFIG_DR_860SPS			0x00E0

#define ADS1115_CONFIG_CMODE_TRAD   		0x0000  // Traditional comparator with hysteresis (default)
#define ADS1115_CONFIG_CMODE_WINDOW 		0x0010  // Window comparator

#define ADS1115_CONFIG_CPOL_ACTVLOW 		0x0000  // ALERT/RDY pin is low when active (default)
#define ADS1115_CONFIG_CPOL_ACTVHI  		0x0008  // ALERT/RDY pin is high when active

#define ADS1115_CONFIG_CLAT_NONLAT  		0x0000  // Non-latching comparator (default)
#define ADS1115_CONFIG_CLAT_LATCH   		0x0004  // Latching comparator

#define ADS1115_CONFIG_CQUE_1CONV   		0x0000  // Assert ALERT/RDY after one conversions
#define ADS1115_CONFIG_CQUE_2CONV   		0x0001  // Assert ALERT/RDY after two conversions
#define ADS1115_CONFIG_CQUE_4CONV   		0x0002  // Assert ALERT/RDY after four conversions
#define ADS1115_CONFIG_CQUE_NONE    		0x0003  // Disable the comparator and put ALERT/RDY in high state (default)


class S_ADS1115_Apn
{
public:
	bool begin(uint8_t address);
	uint16_t read(uint8_t channel);
	void ADS1115_Write(uint8_t regis, uint16_t data);
	uint16_t ADS1115_Read(uint8_t reg);

protected:
	uint8_t _ads1115Address;

private:
	
};

#endif