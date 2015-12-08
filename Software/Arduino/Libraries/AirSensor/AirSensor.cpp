/*
  AirSensor.cpp - Library for Basic Air Data calculations
  Created by J. Larragueta, December 3, 2015.
  Refer to http:\\www.basicairdata.eu
*/
#include "AirSensor.h"
#include <math.h>
#include <Wire.h>
#include <SPI.h>

AirSensor::AirSensor(int pid)
{
    //Default parameters values
    _pid = pid;
}
void AirSensor::ReadDifferentialPressure(AirDC *out,int sensor)
{
    switch (sensor)
    {
    case 1 :{
         //Sensor one is I2C HLCA12X5
        //http://www.first-sensor.com/cms/upload/datasheets/DS_Standard-HCLA_E_11629.pdf
        //Basic software for HLCA12X5 from Sensortechnics/First
        // Pressure  - Sensor output hex/dec
        //12.5  mBar 1250 Pa - 6CCCx /27852d
        //-12.5 mBar 1250 Pa - 0666x /1638d
        // 0    mBar    0 Pa - 3999x/14745
        //(27852-1638)/2500=10.4856 per Pascal
        int   reading ;//The reading from pressure sensor[Pa]
        float rawdata; //Reading with offset
        Wire.beginTransmission(120); //Initialize correct I2C device number
        Wire.requestFrom(120, 2);
        if(2 <= Wire.available())   //When the two requested bytes are available proceed to data handling
        {
            reading = Wire.read();  //16 Bit reading, 1 byte per time
            reading = reading << 8;
            reading |= Wire.read();
            rawdata=(reading-1638)/10.4856-1250;
        }
        //Update measurement values
        out->_qc=rawdata; //pa
        out->_uqc=10.0;//pa
    }
        break;
    case 2 :
        {
        /*
        Differential Pressure sensor LDES205U by FirstSensor/SensorThecnics
        This sensor uses SPI

        Circuit:
        LDES250U   <-->  ArduinoUno
        MOSI: pin 7      MOSI: pin 11                    GREEN
        MISO: pin 8      MISO: pin 12   //MSB first      RED
        SCLK: pin 6      SCK : pin 13                    YELLOW
        /CS:  pin 9      SS  : pin 10   //Asserted low   BROWN
        Power connections to LDES250U

        pin 2 +5V   RED
        pin 3 GND  BLACK

        DATASHEET
        http://www.first-sensor.com/cms/upload/datasheets/DS_Standard-LDE_E_11815.pdf
        */
        float sensorgain=0.00833333; // Pa/Count
        const byte POLL=0x2D;
        const byte REGISTER=0x14;
        const byte GETMEASURE=0x98;
        int value,result;
        int cs = 10;
        float rawpressure;
//Get the reading
        digitalWrite(cs, LOW);
        SPI.transfer(POLL);
        digitalWrite(cs, HIGH);
        digitalWrite(cs, LOW);
        SPI.transfer(REGISTER);
        digitalWrite(cs, HIGH);
        digitalWrite(cs, LOW);
        SPI.transfer(GETMEASURE);
        digitalWrite(cs, HIGH);
        digitalWrite(cs, LOW);
        byte hb = SPI.transfer(0x00);
        byte lb = SPI.transfer(0x00);
        digitalWrite(cs, HIGH);
        result=word(hb,lb);
        rawpressure=result*sensorgain;
        out->_qc=rawpressure; //pa
        out->_uqc=5.0;//pa
        }
        break; //Sensor two is SPI
    case 3 :
        {
        //MPXV7002 Sensor
        //Wire the Arduino Uno and MPXV7002 (That sensor is mounted on HobbyKing Pitot Board) in this way
        //MPXV7002    <->   Arduino Uno
        //GND                     GND
        //VDD                      5V
        //SIGNAL                   A0
        float offsetv=0;
        int analogPin=0; //Wired on input A0
        int raw;
        float Vread,Pread;
        raw = analogRead(analogPin);
        Vread=5.0/1023.0*(raw);
        Pread=(Vread-2.5-offsetv)*1000;
        Pread=1000;
        out->_qc=Pread; //pa
        out->_uqc=50.0;//pa
        }
        break;
    }
}

void AirSensor::ReadStaticPressure(AirDC *out,int sensor)
{
    switch (sensor)
    {
    case 1 :
    {
        out->_p=90000;
        out->_up=100;
        break;
    }
    }
}
