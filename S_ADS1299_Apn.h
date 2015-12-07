#ifndef S_ADS1299_H
#define S_ADS1299_H

#include <Arduino.h>
#include <SPI.h>

//SPI COMMAND address
#define ADS1299_COMM_WAKEUP 			0x02 // Wake-up from standby mode
#define ADS1299_COMM_STANDBY 			0x04 // Enter Standby mode
#define ADS1299_COMM_RESET 				0x06 // Reset the device registers to default
#define ADS1299_COMM_START 				0x08 // Start and restart (synchronize) conversions
#define ADS1299_COMM_STOP 				0x0A // Stop conversion
#define ADS1299_COMM_RDATAC 			0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define ADS1299_COMM_SDATAC 			0x11 // Stop Read Data Continuous mode
#define ADS1299_COMM_RDATA 				0x12 // Read data by command; supports multiple read back

#define ADS1299_COMM_RREG				0x20 // Write at some register (for example ADS1299_COMM_RREG + ADS1299_REGIS_CONFIG2) follow by the number of byte to be read 0b000nnnnn
#define ADS1299_COMM_WREG				0x40 // Read at some register (for example ADS1299_COMM_WREG + ADS1299_REGIS_CONFIG2) follow by the number of byte to be read


//Register Addresses
#define ADS1299_REGIS_ID 				0x00
#define ADS1299_REGIS_CONFIG1 			0x01
#define ADS1299_REGIS_CONFIG2 			0x02
#define ADS1299_REGIS_CONFIG3 			0x03
#define ADS1299_REGIS_LOFF 				0x04
#define ADS1299_REGIS_CH1SET 			0x05
#define ADS1299_REGIS_CH2SET 			0x06
#define ADS1299_REGIS_CH3SET 			0x07
#define ADS1299_REGIS_CH4SET 			0x08
#define ADS1299_REGIS_CH5SET 			0x09
#define ADS1299_REGIS_CH6SET 			0x0A
#define ADS1299_REGIS_CH7SET 			0x0B
#define ADS1299_REGIS_CH8SET 			0x0C
#define ADS1299_REGIS_BIAS_SENSP		0x0D
#define ADS1299_REGIS_BIAS_SENSN 		0x0E
#define ADS1299_REGIS_LOFF_SENSP 		0x0F
#define ADS1299_REGIS_LOFF_SENSN 		0x10
#define ADS1299_REGIS_LOFF_FLIP 		0x11
#define ADS1299_REGIS_LOFF_STATP 		0x12
#define ADS1299_REGIS_LOFF_STATN 		0x13
#define ADS1299_REGIS_GPIO 				0x14
#define ADS1299_REGIS_MISC1 			0x15
#define ADS1299_REGIS_MISC2 			0x16
#define ADS1299_REGIS_CONFIG4 			0x17

//CONFIG Address
#define ADS1299_CONFIG1_DAISY_EN_ENA					0x00	// DEFAULT
#define ADS1299_CONFIG1_DAISY_EN_DIS					0x40	// multiple readback mode
#define ADS1299_CONFIG1_CLK_EN_DIS						0x00	// DEFAULT
#define ADS1299_CONFIG1_CLK_EN_ENA						0x20	// oscillator clock enable
#define ADS1299_CONFIG1_SAMPLING_RATE_16K				0x00
#define ADS1299_CONFIG1_SAMPLING_RATE_8K				0x01
#define ADS1299_CONFIG1_SAMPLING_RATE_4K				0x02
#define ADS1299_CONFIG1_SAMPLING_RATE_2K				0x03
#define ADS1299_CONFIG1_SAMPLING_RATE_16K				0x04
#define ADS1299_CONFIG1_SAMPLING_RATE_500				0x05
#define ADS1299_CONFIG1_SAMPLING_RATE_250				0x06

#define ADS1299_CONFIG2_INT_CAL_EXTERNAL				0x00
#define ADS1299_CONFIG2_INT_CAL_INTERNAL				0x10	//SELECT THIS ONE!!
#define ADS1299_CONFIG2_CAL_AMP0_ONE_TIME				0x00	//DEFAULT
#define ADS1299_CONFIG2_CAL_AMP0_TWO_TIME				0x04
#define ADS1299_CONFIG2_CAL_FREQ_POWER_21				0x00	// test signal pulse at f(clk)/2^21
#define ADS1299_CONFIG2_CAL_FREQ_POWER_20				0x01	// test signal pulse at f(clk)/2^20
#define ADS1299_CONFIG2_CAL_FREQ_NOT_USED				0x02
#define ADS1299_CONFIG2_CAL_FREQ_AT_DC					0x03

#define ADS1299_CONFIG3_PD_REFBUF_ENA					0x00
#define ADS1299_CONFIG3_PD_REFBUF_DIS					0x80
#define ADS1299_CONFIG3_BIAS_MEAS_OPEN					0x00
#define ADS1299_CONFIG3_BIAS_MEAS_ROUTED				0x10
#define ADS1299_CONFIG3_BIASREF_INT_EXTERNAL			0x00
#define ADS1299_CONFIG3_BIASREF_INT_INTERNAL			0x08
#define ADS1299_CONFIG3_PD_BIAS_DISABLE					0x00
#define ADS1299_CONFIG3_PD_BIAS_ENABLE					0x04
#define ADS1299_CONFIG3_PD_BIAS_LOFF_SENS_DISABLE		0x00
#define ADS1299_CONFIG3_PD_BIAS_LOFF_SENS_ENABLE		0x02
#define ADS1299_CONFIG3_PD_BIAS_STAT_CONNECTED			0x00
#define ADS1299_CONFIG3_PD_BIAS_STAT_NOT_CONNECTED		0x01

#define ADS1299_CHSET_PD_NORMAL							0x00
#define ADS1299_CHSET_PD_POWER_DOWN						0x80
#define ADS1299_CHSET_PGA_GAIN_ONE						0x00
#define ADS1299_CHSET_PGA_GAIN_TWO						0x10
#define ADS1299_CHSET_PGA_GAIN_FOUR						0x20
#define ADS1299_CHSET_PGA_GAIN_SIX						0x30
#define ADS1299_CHSET_PGA_GAIN_EIGHT					0x40
#define ADS1299_CHSET_PGA_GAIN_TWELVE					0x50
#define ADS1299_CHSET_PGA_GAIN_TWENTYFOUR				0x60
#define ADS1299_CHSET_SRB2_OPEN							0x00
#define ADS1299_CHSET_SRB2_CLOSED						0x08
#define ADS1299_CHSET_MUX_NORMAL						0x00
#define ADS1299_CHSET_MUX_SHORTED						0x01
#define ADS1299_CHSET_MUX_BIAS_MEAS						0x02
#define ADS1299_CHSET_MUX_MVDD							0x03
#define ADS1299_CHSET_MUX_TEMPERATURE					0x04
#define ADS1299_CHSET_MUX_TEST_SIGNAL					0x05
#define ADS1299_CHSET_MUX_BIAS_DRP						0x06
#define ADS1299_CHSET_MUX_BIAS_DRN						0x07

typedef struct _ADSdata
{
    int32_t CH1 = 0;
    int32_t CH2 = 0;
    int32_t CH3 = 0;
    int32_t CH4 = 0;
    int32_t CH5 = 0;
    int32_t CH6 = 0;
    int32_t CH7 = 0;
    int32_t CH8 = 0;
    uint8_t samplingNum = 0;
} ads1299Package;

class S_ADS1299_Apn
{
    SPISettings settingsEEG;
	public:
		bool begin(uint8_t CS, uint8_t DRDY);
		ads1299Package read(uint8_t chNum);
		int32_t twocomCONVERT(byte one, byte two, byte three);
		void WREG(byte address, byte value);
		void commandSend(byte comm);
		byte RREG(byte address);
        S_ADS1299_Apn() : settingsEEG(4000000, MSBFIRST, SPI_MODE1){};

	private:
		byte holdB1 = 0, holdB2 = 0, holdB3 = 0;
		ads1299Package DATA;

	protected:
		uint8_t _CS, _DRDY, _PWDN, _RESET, _START;
};

#endif
