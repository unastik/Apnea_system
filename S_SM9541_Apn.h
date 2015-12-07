#ifndef S_SM9541_Apn_h
#define S_SM9541_Apn_h

#include <Arduino.h>

class S_SM9541_Apn
{
    public:
        bool begin();
        int32_t read();

    private:
    	byte b;
    	int32_t d;
        //long bcd2dec(long val);


};

#endif