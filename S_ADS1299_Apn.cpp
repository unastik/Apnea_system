#include "S_ADS1299_Apn.h"

bool S_ADS1299_Apn::begin(uint8_t CS, uint8_t DRDY)
{
	_CS = CS;
	_DRDY = DRDY;
	//_PWDN = PWDN;
	//_RESET = RESET;
	//_START = START;

	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV128);
	SPI.setDataMode(SPI_MODE1);
	pinMode(_CS,    OUTPUT);
	pinMode(_DRDY,  INPUT);
	//pinMode(_PWDN,  OUTPUT);
	//pinMode(_RESET, OUTPUT);
	//pinMode(_START, OUTPUT);

	//digitalWrite(_PWDN,  HIGH);
	//digitalWrite(_RESET,  HIGH);
	digitalWrite(_CS,LOW);
	delay(1000);

	S_ADS1299_Apn::commandSend(ADS1299_COMM_RESET);
	//delay(1000);
	S_ADS1299_Apn::commandSend(ADS1299_COMM_SDATAC); //delay(1);

	S_ADS1299_Apn::WREG(ADS1299_REGIS_CONFIG3,0b01100000 | ADS1299_CONFIG3_PD_REFBUF_DIS
									| ADS1299_CONFIG3_BIAS_MEAS_OPEN
									| ADS1299_CONFIG3_BIASREF_INT_INTERNAL
									| ADS1299_CONFIG3_PD_BIAS_ENABLE
									| ADS1299_CONFIG3_PD_BIAS_LOFF_SENS_DISABLE
									| ADS1299_CONFIG3_PD_BIAS_STAT_CONNECTED);
	//delay(200);

	S_ADS1299_Apn::WREG(ADS1299_REGIS_CONFIG1,0b10010000 | ADS1299_CONFIG1_DAISY_EN_DIS
												| ADS1299_CONFIG1_CLK_EN_DIS
												| ADS1299_CONFIG1_SAMPLING_RATE_250);
	//delay(200);

	S_ADS1299_Apn::WREG(ADS1299_REGIS_CONFIG2,0b11000000 | ADS1299_CONFIG2_INT_CAL_INTERNAL
												| ADS1299_CONFIG2_CAL_AMP0_ONE_TIME
												| ADS1299_CONFIG2_CAL_FREQ_POWER_21);
	//delay(200);

	byte configCH1 = ADS1299_CHSET_PD_NORMAL| ADS1299_CHSET_PGA_GAIN_TWENTYFOUR
											| ADS1299_CHSET_SRB2_OPEN
											| ADS1299_CHSET_MUX_NORMAL;

	S_ADS1299_Apn::WREG(ADS1299_REGIS_CH1SET, configCH1);
	//delay(200);
	S_ADS1299_Apn::WREG(ADS1299_REGIS_CH2SET, ADS1299_CHSET_PD_NORMAL
											| ADS1299_CHSET_PGA_GAIN_TWENTYFOUR
											| ADS1299_CHSET_SRB2_OPEN
											| ADS1299_CHSET_MUX_NORMAL);
	//delay(200);
	S_ADS1299_Apn::WREG(ADS1299_REGIS_CH3SET, ADS1299_CHSET_PD_NORMAL
											| ADS1299_CHSET_PGA_GAIN_TWENTYFOUR
											| ADS1299_CHSET_SRB2_OPEN
											| ADS1299_CHSET_MUX_NORMAL);
	//delay(200);
	S_ADS1299_Apn::WREG(ADS1299_REGIS_CH4SET, ADS1299_CHSET_PD_NORMAL
											| ADS1299_CHSET_PGA_GAIN_TWENTYFOUR
											| ADS1299_CHSET_SRB2_OPEN
											| ADS1299_CHSET_MUX_NORMAL);
	//delay(200);
	S_ADS1299_Apn::WREG(ADS1299_REGIS_CH5SET, ADS1299_CHSET_PD_NORMAL
											| ADS1299_CHSET_PGA_GAIN_TWENTYFOUR
											| ADS1299_CHSET_SRB2_OPEN
											| ADS1299_CHSET_MUX_NORMAL);
	//delay(200);
	S_ADS1299_Apn::WREG(ADS1299_REGIS_CH6SET, ADS1299_CHSET_PD_NORMAL
											| ADS1299_CHSET_PGA_GAIN_TWENTYFOUR
											| ADS1299_CHSET_SRB2_OPEN
											| ADS1299_CHSET_MUX_NORMAL);
	//delay(200);
	S_ADS1299_Apn::WREG(ADS1299_REGIS_CH7SET, ADS1299_CHSET_PD_NORMAL
											| ADS1299_CHSET_PGA_GAIN_TWENTYFOUR
											| ADS1299_CHSET_SRB2_OPEN
											| ADS1299_CHSET_MUX_NORMAL);
	//delay(200);
	S_ADS1299_Apn::WREG(ADS1299_REGIS_CH8SET, ADS1299_CHSET_PD_NORMAL
											| ADS1299_CHSET_PGA_GAIN_TWENTYFOUR
											| ADS1299_CHSET_SRB2_OPEN
											| ADS1299_CHSET_MUX_NORMAL);
	//delay(200);

	byte readB = S_ADS1299_Apn::RREG(ADS1299_REGIS_CH1SET);
	Serial.println(readB,BIN);
	//digitalWrite(_START, HIGH);//delayMicroseconds(100);

	S_ADS1299_Apn::commandSend(ADS1299_COMM_START);delayMicroseconds(100);
	S_ADS1299_Apn::commandSend(ADS1299_COMM_RDATAC);

	delay(1000);

	if(readB != configCH1)
		return false;
	else
		return true;
}

void S_ADS1299_Apn::WREG(byte address, byte value)
{
	byte opcode1 = address + 0x40; 	//  WREG expects 010rrrrr where rrrrr = _address
	SPI.beginTransaction(settingsEEG);
    digitalWrite(_CS, LOW); 				//  open SPI
    SPI.transfer(opcode1);					//  Send WREG command & address
    SPI.transfer(0x00);						//	Send number of registers to read -1
    SPI.transfer(value);					//  Write the value to the register
    digitalWrite(_CS, HIGH);
	SPI.endTransaction();
}

byte S_ADS1299_Apn::RREG(byte address)
{
	SPI.beginTransaction(settingsEEG);
	digitalWrite(_CS, LOW);
	SPI.transfer(0x20 + address);
	SPI.transfer(0x00);
	byte readB = SPI.transfer(0x00);
	digitalWrite(_CS, HIGH);
	SPI.endTransaction();
	return readB;
}

void S_ADS1299_Apn::commandSend(byte comm)
{
	SPI.beginTransaction(settingsEEG);
	digitalWrite(_CS, LOW);
	SPI.transfer(comm);
	digitalWrite(_CS, HIGH);
	SPI.endTransaction();
}

int32_t S_ADS1299_Apn::twocomCONVERT(byte one, byte two, byte three)
{
	volatile bool flagMinus = false;
	int32_t ANS = 0;

	ANS |= one; ANS = ANS << 8;
	ANS |= two; ANS = ANS << 8;
	ANS |= three; ANS = ANS << 8;

	one = 0; two = 0; three = 0;

	//Serial.println(ANS,BIN);

	if(ANS && 0b10000000000000000000000000000000){
		flagMinus = true;
	}else{
		flagMinus = false;
	//Serial.println("-");
	}

	if(flagMinus == true){
	//Serial.println("-");

		ANS = ~ANS;
		ANS += 0b1;
		//ANS = ANS >> 8 ;
	//ANS = ANS - 0x1000000;
		flagMinus = false;
		return -ANS;//-1*ANS;
	}else{
	//Serial.println("/");
		flagMinus = false;
		return ANS;
	}
}

ads1299Package S_ADS1299_Apn::read(uint8_t chNum)
{
	SPI.beginTransaction(settingsEEG);
	digitalWrite(_CS, LOW);
	SPI.transfer(0x00);//delayMicroseconds(6);
	SPI.transfer(0x00);//delayMicroseconds(6);
	SPI.transfer(0x00);//delayMicroseconds(6);

	if(chNum >= 1){
		holdB1 = SPI.transfer(0x00);
		holdB2 = SPI.transfer(0x00);
		holdB3 = SPI.transfer(0x00);
		DATA.CH1 = twocomCONVERT(holdB1,holdB2,holdB3);
		holdB1 = 0; holdB2 = 0; holdB3 = 0;
	}

	if(chNum >= 2){
		holdB1 = SPI.transfer(0x00);
		holdB2 = SPI.transfer(0x00);
		holdB3 = SPI.transfer(0x00);
		DATA.CH2 = twocomCONVERT(holdB1,holdB2,holdB3);
		holdB1 = 0; holdB2 = 0; holdB3 = 0;
	}

	if(chNum >= 3){
		holdB1 = SPI.transfer(0x00);
		holdB2 = SPI.transfer(0x00);
		holdB3 = SPI.transfer(0x00);
		DATA.CH3 = twocomCONVERT(holdB1,holdB2,holdB3);
		holdB1 = 0; holdB2 = 0; holdB3 = 0;
	}

	if(chNum >= 4){
		holdB1 = SPI.transfer(0x00);
		holdB2 = SPI.transfer(0x00);
		holdB3 = SPI.transfer(0x00);
		DATA.CH4 = twocomCONVERT(holdB1,holdB2,holdB3);
		holdB1 = 0; holdB2 = 0; holdB3 = 0;
	}

	if(chNum >= 5){
		holdB1 = SPI.transfer(0x00);
		holdB2 = SPI.transfer(0x00);
		holdB3 = SPI.transfer(0x00);
		DATA.CH5 = twocomCONVERT(holdB1,holdB2,holdB3);
		holdB1 = 0; holdB2 = 0; holdB3 = 0;
	}

	if(chNum >= 6){
		holdB1 = SPI.transfer(0x00);
		holdB2 = SPI.transfer(0x00);
		holdB3 = SPI.transfer(0x00);
		DATA.CH6 = twocomCONVERT(holdB1,holdB2,holdB3);
		holdB1 = 0; holdB2 = 0; holdB3 = 0;
	}

	if(chNum >= 7){
		holdB1 = SPI.transfer(0x00);
		holdB2 = SPI.transfer(0x00);
		holdB3 = SPI.transfer(0x00);
		DATA.CH7 = twocomCONVERT(holdB1,holdB2,holdB3);
		holdB1 = 0; holdB2 = 0; holdB3 = 0;
	}

	if(chNum == 8)
	{
		holdB1 = SPI.transfer(0x00);
		holdB2 = SPI.transfer(0x00);
		holdB3 = SPI.transfer(0x00);
		DATA.CH8 = twocomCONVERT(holdB1,holdB2,holdB3);
		holdB1 = 0; holdB2 = 0; holdB3 = 0;
	}
	digitalWrite(_CS, HIGH);
	SPI.endTransaction();
	return DATA;
}
