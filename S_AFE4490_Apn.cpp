#include "S_AFE4490_Apn.h"

#include <SPI.h>

	bool S_AFE4490_Apn::begin(uint8_t spiste, uint8_t adc_rdy)
	{
		_SPISTE = spiste;
		_ADC_RDY = adc_rdy;
		//_ADC_RST = adc_rst;

		pinMode (_SPISTE,    OUTPUT);
		pinMode (_ADC_RDY,   INPUT);
    	//pinMode (_ADC_RST, INPUT);

    	SPI.begin();
    	SPI.beginTransaction(settingsSpO2);


    	S_AFE4490_Apn::AFE4490_Write(CONTROL0,      8);
    	delay(200);
	    S_AFE4490_Apn::AFE4490_Write(CONTROL2,      0);
	    S_AFE4490_Apn::AFE4490_Write(PRPCOUNT,      7999);

		// ADC Reset count cycle
	    S_AFE4490_Apn::AFE4490_Write(ADCRSTCNT0,    0);
	    S_AFE4490_Apn::AFE4490_Write(ADCRSTENDCT0,  3);
	    S_AFE4490_Apn::AFE4490_Write(ADCRSTCNT1,    2000);
	    S_AFE4490_Apn::AFE4490_Write(ADCRSTENDCT1,  2003);
	    S_AFE4490_Apn::AFE4490_Write(ADCRSTCNT2,    4000);
	    S_AFE4490_Apn::AFE4490_Write(ADCRSTENDCT2,  4003);
	    S_AFE4490_Apn::AFE4490_Write(ADCRSTCNT3,    6000);
	    S_AFE4490_Apn::AFE4490_Write(ADCRSTENDCT3,  6003);

	    // ADC Conversion count cycle
	    S_AFE4490_Apn::AFE4490_Write(LED2CONVST,    4);
	    S_AFE4490_Apn::AFE4490_Write(LED2CONVEND,   1999);
	    S_AFE4490_Apn::AFE4490_Write(ALED2CONVST,   2004);
	    S_AFE4490_Apn::AFE4490_Write(ALED2CONVEND,  3999);
	    S_AFE4490_Apn::AFE4490_Write(LED1CONVST,    4004);
	    S_AFE4490_Apn::AFE4490_Write(LED1CONVEND,   5999);
	    S_AFE4490_Apn::AFE4490_Write(ALED1CONVST,   6004);
	    S_AFE4490_Apn::AFE4490_Write(ALED1CONVEND,  7999);

	    // ALED2 sample count cycle
	    S_AFE4490_Apn::AFE4490_Write(ALED2STC,      50);
	    S_AFE4490_Apn::AFE4490_Write(ALED2ENDC,     1998);

	    S_AFE4490_Apn::AFE4490_Write(LED1LEDSTC,    2000); // LED1 turn on
	    S_AFE4490_Apn::AFE4490_Write(LED1STC,       2050); // LED1 sample
	    S_AFE4490_Apn::AFE4490_Write(LED1ENDC,      3998); // LED1 stop sample
	    S_AFE4490_Apn::AFE4490_Write(LED1LEDENDC,   3999); // LED1 turn off

	    // ALED1 sample count cycle
	    S_AFE4490_Apn::AFE4490_Write(ALED1STC,      4050);
	    S_AFE4490_Apn::AFE4490_Write(ALED1ENDC,     5998);

	    S_AFE4490_Apn::AFE4490_Write(LED2LEDSTC,    6000); // LED2 turn on
	    S_AFE4490_Apn::AFE4490_Write(LED2STC,       6050); // LED2 sample
	    S_AFE4490_Apn::AFE4490_Write(LED2ENDC,      7998); // LED2 stop sample
	    S_AFE4490_Apn::AFE4490_Write(LED2LEDENDC,   7999); // LED2 turn off

	    S_AFE4490_Apn::AFE4490_Write(CONTROL1, CONTROL1_TIMEREN_SET | (CONTROL1_AVERAGE_MARK & 15));
	    S_AFE4490_Apn::AFE4490_Write(TIAGAIN,       0);
	    S_AFE4490_Apn::AFE4490_Write(TIA_AMB_GAIN,  0);
	    S_AFE4490_Apn::AFE4490_Write(LEDCNTRL,      0x00013030); // LED Bightness config

	    S_AFE4490_Apn::AFE4490_Write(CONTROL0, CONTROL0_SPI_READ); // switch to SPI_READ mode

	    delay(1000);
	    uint32_t testData = S_AFE4490_Apn::AFE4490_Read(LEDCNTRL);
	    Serial.println(testData,BIN);
		SPI.endTransaction();
	    if(testData == 0x00013030){
	    	return true;
	    }else{
			return false;
		}
	}

	void S_AFE4490_Apn::read(afe4490Package* DATA)
	{
		if (cnt == 0){
			DATA->temp_IR   = S_AFE4490_Apn::AFE4490_Read(LED1ABSVAL);
			DATA->temp_RED  = S_AFE4490_Apn::AFE4490_Read(LED2ABSVAL);
			cnt = 1;
		}
		else if(cnt == 1){
			DATA->data_IR     = ((DATA->temp_IR)+S_AFE4490_Apn::AFE4490_Read(LED1ABSVAL))/2;
			DATA->data_RED    = ((DATA->temp_RED)+S_AFE4490_Apn::AFE4490_Read(LED2ABSVAL))/2;
			cnt = 2;
		}
	}

	void S_AFE4490_Apn::printData(afe4490Package* DATA)
	{
	    Serial.print(DATA->data_RED);
	    Serial.print("\t");
	    Serial.print(DATA->data_IR);
	    Serial.print("\t");
	}

	void S_AFE4490_Apn::AFE4490_Write (uint8_t address, uint32_t data)
	{
		/*if (digitalRead (PIN_MISO) == LOW){
	        Serial.println ("SOMI is not connected!!");
	    }*/
		SPI.beginTransaction(settingsSpO2);
		digitalWrite (_SPISTE, LOW);
	    SPI.transfer (address);
	    SPI.transfer ((data >> 16) & 0xFF);
	    SPI.transfer ((data >>  8) & 0xFF);
	    SPI.transfer (data & 0xFF);
	    digitalWrite (_SPISTE, HIGH);
		SPI.endTransaction();
	}

	uint32_t S_AFE4490_Apn::AFE4490_Read (uint8_t address)
	{
		SPI.beginTransaction(settingsSpO2);
		uint32_t data = 0;
	    digitalWrite (_SPISTE, LOW);
	    SPI.transfer (address);
	    data |= ( SPI.transfer (0) << 16 );
	    data |= ( SPI.transfer (0) <<  8 );
	    data |=   SPI.transfer (0);
	    digitalWrite (_SPISTE, HIGH);
		SPI.endTransaction();
	    return data;
	}
