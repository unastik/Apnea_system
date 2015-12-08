/*=======================================================================
    Project             :       Sleep Apnea Detection Device System
    Developer           :       Nitikorn Srisrisawang
    Version             :       xxxxxxxxxx
    Last modified date  :       xx/xx/xxxx
=======================================================================*/

    ////********************************************Basic Implementation********************************************************
        //---------------------------------Library inclusion----------------------------------
            #include <Wire.h>
            #include <SPI.h>

            #include "Node_Apn.h"
            #include "DueTimer.h"
            #include "S_SM9541_Apn.h"
            #include "s_ADS1115_Apn.h"
            #include "S_ADS1299_Apn.h"
            #include "S_AFE4490_Apn.h"
            bool verbose = true;        // change this variable to false to mute the serial ouput
            bool writebyte = true;      //set to true to change to write the data to byte

        //------------------------------------Initialize function implement---------------------------------------------------
            void deviceInit();
            void sensorInit();
            void threadInit();
            void streamingInit();

        //------------------------------------Acquisition function implement---------------------------------------------------
            void nasalAcquire();
            void analogAcquire();

            void spo2Acquire();

            void biosignal1Acquire();
            void biosignal2Acquire();

        //------------------------------------Streaming function implement---------------------------------------------------
            void shiftBuffer();
            void streamingToPC();
            void write2byte(int16_t data);
            void write4byte(int32_t data);

            #define SLOW_FREQUENCY 25
            #define FAST_FREQUENCY 250

            #define BUFFERSIZESLOW 1+1
            #define BUFFERSIZEFAST 10+1

        //-----------------------------Sensor module implement---------------------------------------
            S_SM9541_Apn nasal;

            S_AFE4490_Apn spo2;
            //#define SPO2_CYCLE 500
            #define SPO2_SPISTE_PIN 10
            #define SPO2_ADCRDY_PIN 9
            //#define SPO2_ADCRST_PIN 7
            afe4490Package spo2pack;

            typedef struct _spo2data
            {
                int32_t IRData      = 0;
                int32_t REDData     = 0;
            } dataSpo2;
            dataSpo2 spo2Data[BUFFERSIZEFAST];

            //uint8_t CS, uint8_t DRDY, uint8_t PWDN, uint8_t RESET, uint8_t START
            S_ADS1299_Apn eeg1,eeg2;
            #define EEG1_CS_PIN     42
            #define EEG1_DRDY_PIN   40

            #define EEG2_CS_PIN     43
            #define EEG2_DRDY_PIN   41
            ads1299Package eeg1pack[BUFFERSIZEFAST], eeg2pack[BUFFERSIZEFAST],zeroEEG;

            typedef struct _data
            {
                int32_t nasalData       = 999999999;
                int32_t thermistData    = 999999999;
                int32_t thoraxData      = 999999999;
                int32_t abdoData        = 999999999;
                uint8_t samplingNum     = 0;
            } dataPackage;
            dataPackage rawData[BUFFERSIZESLOW];

            S_ADS1115_Apn analog;
            bool nasalExist = false, spo2Exist = false, analogExist = false, eeg1Exist = false, eeg2Exist = false;

        //-------------------------Thread and interrupt implement------------------------------------------------
            DueTimer nasalTimer = DueTimer(0);
            DueTimer analogTimer =  DueTimer(1);
            DueTimer streamTimer =  DueTimer(2);

            void streamTick(); bool timeToStream = false;

    ////************************************function**********************************************************
        void setup()
        {
            deviceInit();
            sensorInit();
            threadInit();
            streamingInit();
        }

        void loop()
        {
            streamingToPC();
        }


        //------------------------------------Initialize function---------------------------------------------------
            void deviceInit()
            {
                Serial.begin(250000);
                if(verbose){
                    Serial.println("Serial initialized>>");
                    Serial.println("Checking module.");
                }
                Serial1.begin(250000);
                zeroEEG.CH1 = 0;zeroEEG.CH2 = 0;zeroEEG.CH3 = 0;zeroEEG.CH4 = 0;zeroEEG.CH5 = 0;zeroEEG.CH6 = 0;zeroEEG.CH7 = 0;zeroEEG.CH8 = 0;
            }

            void sensorInit()
            {
                nasalExist = false;
                analogExist = false;
                spo2Exist = false;
                eeg1Exist = false;
                eeg2Exist = false;
                if(nasal.begin())
                {
                    nasalExist = true;
                    if(verbose)
                        Serial.println("Nasal cannula module initialized.");
                }

                if(analog.begin(ADS1115_ADDR_GND))
                {
                    analogExist = true;
                    if(verbose)
                        Serial.println("Analog module initialized.");
                }

                if(spo2.begin(SPO2_SPISTE_PIN, SPO2_ADCRDY_PIN))
                {
                    spo2Exist = true;
                    //delay(5000);
                    if(verbose)
                        Serial.println("SpO2 module initialized.");
                }

                if(eeg1.begin(EEG1_CS_PIN, EEG1_DRDY_PIN))
                {
                    eeg1Exist = true;
                    if(verbose)
                        Serial.println("Biosignal module 1 initialized.");
                }

                if(eeg2.begin(EEG2_CS_PIN, EEG2_DRDY_PIN))
                {
                    eeg2Exist = true;
                    if(verbose)
                        Serial.println("Biosignal module 2 initialized.");
                }

            }

            void threadInit()
            {
                if(eeg1Exist){
                    //attachInterrupt(EEG1_DRDY_PIN, biosignal1Acquire, FALLING);
                    if(verbose)
                        Serial.println("Biosignal1 Thread initialized.");
                }
                if(eeg2Exist){
                    //attachInterrupt(EEG2_DRDY_PIN, biosignal2Acquire, FALLING);
                    if(verbose)
                        Serial.println("Biosignal2 Thread initialized.");
                }
                if(spo2Exist){
                    //attachInterrupt(SPO2_ADCRDY_PIN, spo2Acquire, FALLING);
                    if(verbose)
                        Serial.println("SpO2 Thread initialized.");
                }

                if(nasalExist){
                    //nasalTimer.attachInterrupt(nasalAcquire).setFrequency(SLOW_FREQUENCY).start();
                    if(verbose)
                        Serial.println("Nasal Thread initialized.");
                }
                if(analogExist){
                    //delayMicroseconds(1500);
                    //analogTimer.attachInterrupt(analogAcquire).setFrequency(SLOW_FREQUENCY).start();
                    //delayMicroseconds(1500);
                    if(verbose)
                        Serial.println("Analog Thread initialized.");
                }

                attachInterrupt(EEG1_DRDY_PIN, biosignal1Acquire, FALLING);
                attachInterrupt(EEG2_DRDY_PIN, biosignal2Acquire, FALLING);
                attachInterrupt(SPO2_ADCRDY_PIN, spo2Acquire, FALLING);
                SPI.usingInterrupt(EEG1_DRDY_PIN);
                SPI.usingInterrupt(EEG2_DRDY_PIN);
                SPI.usingInterrupt(SPO2_ADCRDY_PIN); // To prevent interrupt usage conflicts
                nasalTimer.attachInterrupt(nasalAcquire).setFrequency(SLOW_FREQUENCY).start();
                analogTimer.attachInterrupt(analogAcquire).setFrequency(SLOW_FREQUENCY).start();
                streamTimer.attachInterrupt(streamTick).setFrequency(SLOW_FREQUENCY).start();
            }

            void streamingInit()
            {
                if(verbose)
                    Serial.println("#READY");
            }

        //---------------------------------Acquisition function---------------------------------------
            void nasalAcquire()
            {
                if(nasalExist){
                    rawData[0].nasalData = nasal.read();
                    rawData[0].samplingNum++;
                    //Serial1.write('a');
                }else{
                    rawData[0].nasalData = 0;
                }
            }
            void analogAcquire()
            {
                if(analogExist){
                    rawData[0].thermistData = (int16_t)analog.read(2);
                    delayMicroseconds(350);
                    rawData[0].thoraxData = (int16_t)analog.read(1);
                    delayMicroseconds(350);
                    rawData[0].abdoData = (int16_t)analog.read(0);
                    //Serial1.write('E');
                }else{
                    rawData[0].thermistData = 0;
                    rawData[0].thoraxData = 0;
                    rawData[0].abdoData = 0;
                }
                //Serial1.write('b');
            }
            void spo2Acquire()
            {
                static int buffernumber;
                if(spo2Exist){
                    spo2.read(&spo2pack);
                    if(spo2.cnt == 2){
                        spo2Data[0].IRData = spo2pack.data_IR;
                        spo2Data[0].REDData = spo2pack.data_RED;
                        spo2.cnt = 0;
                        //Serial1.write('S');
                    }
                }else{
                    spo2Data[0].IRData = 0;
                    spo2Data[0].REDData = 0;
                }//Serial1.write('S');
                for(buffernumber = BUFFERSIZEFAST-2; buffernumber >= 0; buffernumber--){
                    spo2Data[buffernumber+1] = spo2Data[buffernumber];
                    //Serial1.write('S');
                }
                //Serial1.write('E');
            }
            void biosignal1Acquire()
            {
                static int buffernumber;
                if(eeg1Exist){
                    eeg1pack[0] = eeg1.read(8);
                    eeg1pack[0].samplingNum++;
                }else{
                    eeg1pack[0] = zeroEEG;
                }
                for(buffernumber = BUFFERSIZEFAST-2; buffernumber >= 0; buffernumber--){
                    eeg1pack[buffernumber+1] = eeg1pack[buffernumber];
                    //Serial1.write('S');
                }
                //Serial1.write('E');
            }
            void biosignal2Acquire()
            {
                static int buffernumber;
                if(eeg2Exist){
                    eeg2pack[0] = eeg2.read(7);
                    eeg2pack[0].samplingNum++;
                }else{
                    eeg2pack[0] = zeroEEG;
                }//Serial1.write('K');
                for(buffernumber = BUFFERSIZEFAST-2; buffernumber >= 0; buffernumber--){
                    eeg2pack[buffernumber+1] = eeg2pack[buffernumber];
                    //Serial1.println("SHITFT2");
                }
                //Serial1.write('F');
            }

        //------------------------------------Streaming function implement---------------------------------------------------
            void streamTick()
            {
                timeToStream = true;
            }

            void streamingToPC()
            {
                int bufferIteration;
                //bool bufferSlowFulled = false,bufferFastFulled = false;
                if(timeToStream){
                //Profiler : 33 - 36 ms
                //digitalWrite(47,HIGH);
                Serial.write('#');
                Serial.write('S');
                Serial1.write('E');
                for(bufferIteration = BUFFERSIZESLOW-2; bufferIteration >= 0; bufferIteration--){
                    if(writebyte){
                        rawData[bufferIteration].nasalData    *= 65536;
                        rawData[bufferIteration].thermistData *= 65536;
                        rawData[bufferIteration].thoraxData   *= 65536;
                        rawData[bufferIteration].abdoData     *= 65536;

                        Serial.write(rawData[bufferIteration].samplingNum);
                        write2byte(rawData[bufferIteration].nasalData);
                        write2byte(rawData[bufferIteration].thermistData);
                        write2byte(rawData[bufferIteration].thoraxData);
                        write2byte(rawData[bufferIteration].abdoData);
                        //Serial.write((byte)(rawData[bufferIteration].spo2Data));
                    }else{
                        rawData[bufferIteration].nasalData    *= 65536;
                        rawData[bufferIteration].thermistData *= 65536;
                        rawData[bufferIteration].thoraxData   *= 65536;
                        rawData[bufferIteration].abdoData     *= 65536;

                        Serial.print(rawData[bufferIteration].samplingNum);Serial.print(',');
                        Serial.print(rawData[bufferIteration].nasalData);Serial.print(',');
                        Serial.print(rawData[bufferIteration].thermistData);Serial.print(',');
                        Serial.print(rawData[bufferIteration].thoraxData);Serial.print(',');
                        Serial.print(rawData[bufferIteration].abdoData);Serial.print(',');
                        //Serial.print(rawData[bufferIteration].spo2Data);Serial.print(',');
                    }
                }

                for(bufferIteration = BUFFERSIZEFAST-1; bufferIteration >= 1; bufferIteration--){
                    if(writebyte){
                        write4byte(spo2Data[bufferIteration].IRData);
                        write4byte(spo2Data[bufferIteration].REDData);
                        write4byte(eeg1pack[bufferIteration].CH1);
                        write4byte(eeg1pack[bufferIteration].CH2);
                        write4byte(eeg1pack[bufferIteration].CH3);
                        write4byte(eeg1pack[bufferIteration].CH4);
                        write4byte(eeg1pack[bufferIteration].CH5);
                        write4byte(eeg1pack[bufferIteration].CH6);
                        write4byte(eeg1pack[bufferIteration].CH7);
                        write4byte(eeg1pack[bufferIteration].CH8);

                        /*write4byte(eeg2pack[bufferIteration].CH1);
                        write4byte(eeg2pack[bufferIteration].CH2);
                        write4byte(eeg2pack[bufferIteration].CH3);
                        write4byte(eeg2pack[bufferIteration].CH4);
                        write4byte(eeg2pack[bufferIteration].CH5);
                        write4byte(eeg2pack[bufferIteration].CH6);
                        write4byte(eeg2pack[bufferIteration].CH7);*/
                    }else{
                        Serial.print(spo2Data[bufferIteration].IRData);Serial.print(',');
                        Serial.print(spo2Data[bufferIteration].REDData);Serial.print(',');
                        Serial.print(eeg1pack[bufferIteration].CH1);Serial.print(',');
                        Serial.print(eeg1pack[bufferIteration].CH2);Serial.print(',');
                        Serial.print(eeg1pack[bufferIteration].CH3);Serial.print(',');
                        Serial.print(eeg1pack[bufferIteration].CH4);Serial.print(',');
                        Serial.print(eeg1pack[bufferIteration].CH5);Serial.print(',');
                        Serial.print(eeg1pack[bufferIteration].CH6);Serial.print(',');
                        Serial.print(eeg1pack[bufferIteration].CH7);Serial.print(',');
                        Serial.print(eeg1pack[bufferIteration].CH8);
                        if(bufferIteration != 1)
                            Serial.print(',');
                        /*Serial.print(eeg2pack[bufferIteration].CH1);
                        Serial.print(eeg2pack[bufferIteration].CH2);
                        Serial.print(eeg2pack[bufferIteration].CH3);
                        Serial.print(eeg2pack[bufferIteration].CH4);
                        Serial.print(eeg2pack[bufferIteration].CH5);
                        Serial.print(eeg2pack[bufferIteration].CH6);
                        Serial.print(eeg2pack[bufferIteration].CH7);*/
                    }
                }
                //bufferSlowFulled = false;
                //bufferFastFulled = false;
                timeToStream = false;
                //digitalWrite(47,LOW);
                //bufferStringStreamSlow += "\n";
                //Serial.println("EEG ACQUIRED!");//Serial.print(bufferStringStreamSlow);
                //Serial1.write('S');
                Serial.write('\n');
                Serial1.write('A');
                }
            }

            void write2byte(int16_t data)
            {
                Serial.write((byte)((data)>>24));
                Serial.write((byte)((data)>>16));
            }
            void write4byte(int32_t data)
            {
                Serial.write((byte)((data)>>24));
                Serial.write((byte)((data)>>16));
                Serial.write((byte)((data)>>8));
                Serial.write((byte)(0x00));
            }
