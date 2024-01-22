#include "UTLN_CMPS12.h"

CMPS12Config cmpsConfig[MAX_CMPS_DEVICE];
unsigned long clockFrequency;
void wait_ms(unsigned short ms)
{
    if(clockFrequency!=0)
        __delay32((clockFrequency/1000)*ms);
}
void wait_us(unsigned short us)
{
    if(clockFrequency!=0)
        __delay32((clockFrequency/1000000)*us);
}
void CMPS12RegisterUserSystemClock(unsigned long ticksPerSec)
{
    clockFrequency=ticksPerSec;
}

void CMPS12RegisterModePin(unsigned char deviceID,unsigned int * pPort, unsigned char pinNum)
{
    cmpsConfig[deviceID].pModePort=pPort;
    cmpsConfig[deviceID].modePin=pinNum;
}

void CMPS12RegisterI2CDevice(unsigned char deviceID,unsigned char deviceAddress, unsigned char mode)
{
    cmpsConfig[deviceID].i2cAddress=deviceAddress;
    cmpsConfig[deviceID].mode=mode;
}

//==================I2C Func==================================================//
CMPS12USER_I2C_Func i2cWrFunc;
void CMPS12RegisterUserI2CWriteFunc(CMPS12USER_I2C_Func func)
{
    if(func != NULL)
        i2cWrFunc = func;
}
CMPS12USER_I2C_Func i2cRdFunc;
void CMPS12RegisterUserI2CReadFunc(CMPS12USER_I2C_Func func)
{
    if(func != NULL)
        i2cRdFunc = func;
}

void CMPS12WriteNRegister( unsigned char deviceID, unsigned char registerAddress, unsigned char* data, unsigned int length )
{
    if(cmpsConfig[deviceID].mode)
    {
        if(i2cWrFunc!=NULL)
            (*i2cWrFunc)(cmpsConfig[deviceID].i2cAddress,registerAddress,data,length);
    }
    else
    {
//        if(serialFunc!=NULL)
//            (*serialFunc)()
    }
}

void CMPS12ReadNRegister( unsigned char deviceID, unsigned char registerAddress, unsigned char* data, unsigned int length )
{
    if(cmpsConfig[deviceID].mode)
    {
        if(i2cRdFunc!=NULL)
            (*i2cRdFunc)(cmpsConfig[deviceID].i2cAddress,registerAddress,data,length);
    }
}

void CMPS12ChangeI2CAddress(unsigned char deviceID,unsigned char newAddress)
{
    unsigned char dat=0xA0;
    CMPS12WriteNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1);
    wait_ms(20);
    dat=0xAA;
    CMPS12WriteNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1);
    wait_ms(20);
    dat=0xA5;
    CMPS12WriteNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1);
    wait_ms(20);
    dat=newAddress;
    CMPS12WriteNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1);
}

unsigned char CMPS12ReadSoftwareVersion(unsigned char deviceID)
{
    unsigned char dat;
    CMPS12ReadNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1 );
    return dat;
}


unsigned char CMPS12GetAngle8bitBearing(unsigned char deviceID)
{
    unsigned char angle8;
    CMPS12ReadNRegister(deviceID, CMPS12_COMPASS_BEARING, &angle8, 1 );
    return angle8;
}

unsigned short CMPS12GetAngle16bitBearing(unsigned char deviceID)
{
    unsigned char data[2];
    CMPS12ReadNRegister(deviceID, CMPS12_COMPASS_BEARING_MSB, data, 2 );
    return ((unsigned short)data[0]<<8)|data[1];
}

unsigned char CMPS12GetPitch8bit(unsigned char deviceID)
{
    unsigned char pitch8;
    CMPS12ReadNRegister(deviceID, CMPS12_PITCH_ANGLE, &pitch8, 1 );
    return pitch8;
}

unsigned char CMPS12GetRoll8bit(unsigned char deviceID)
{
    unsigned char roll8;
    CMPS12ReadNRegister(deviceID, CMPS12_ROLL_ANGLE, &roll8, 1 );
    return roll8;
}

void CMPS12GetMagRawValues(unsigned char deviceID, unsigned char* data)
{
    CMPS12ReadNRegister( deviceID, CMPS12_MAG_X_RAW_MSB,data, 6 );
}

XYZValues CMPS12GetMagRawXYZValues(unsigned char deviceID)
{
    XYZValues value;
    unsigned char data[6];
    CMPS12ReadNRegister( deviceID, CMPS12_MAG_X_RAW_MSB,data, 6 );
    value.x=((short)data[0]<<8)| data[1];
    value.y=((short)data[2]<<8)| data[3];
    value.z=((short)data[4]<<8)| data[5];
    return value;
}

void CMPS12GetAccRawValues(unsigned char deviceID, unsigned char* data)
{
    CMPS12ReadNRegister( deviceID, CMPS12_ACC_X_RAW_MSB,data, 6 );
}

XYZValues CMPS12GetAccRawXYZValues(unsigned char deviceID)
{
    XYZValues value;
    unsigned char data[6];
    CMPS12ReadNRegister( deviceID, CMPS12_ACC_X_RAW_MSB,data, 6 );
    value.x=((short)data[0]<<8)| data[1];
    value.y=((short)data[2]<<8)| data[3];
    value.z=((short)data[4]<<8)| data[5];
    return value;
}

void CMPS12GetGyrRawValues(unsigned char deviceID, unsigned char* data)
{
    CMPS12ReadNRegister( deviceID, CMPS12_GYR_X_RAW_MSB,data, 6 );
}

XYZValues CMPS12GetGyrRawXYZValues(unsigned char deviceID)
{
    XYZValues value;
    unsigned char data[6];
    CMPS12ReadNRegister( deviceID, CMPS12_GYR_X_RAW_MSB,data, 6 );
    value.x=((short)data[0]<<8)| data[1];
    value.y=((short)data[2]<<8)| data[3];
    value.z=((short)data[4]<<8)| data[5];
    return value;
}

short CMPS12GetBNO55Temperature(unsigned char deviceID)
{
    unsigned char data[2];
    CMPS12ReadNRegister( deviceID, CMPS12_GYR_X_RAW_MSB,data, 2 );
    return ((short)data[0]<<8)|data[1];
}

unsigned short CMPS12GetCompassBNO55Bearing(unsigned char deviceID)
{
    unsigned char data[2];
    CMPS12ReadNRegister( deviceID, CMPS12_COMPASS_BEARING_16BIT_BNO55_MSB,data, 2 );
    return ((unsigned short)data[0]<<8)|data[1];
}

short CMPS12GetCompassPitch180(unsigned char deviceID)
{
    unsigned char data[2];
    CMPS12ReadNRegister( deviceID, CMPS12_COMPASS_PITCH_ANGLE_MSB,data, 2 );
    return ((short)data[0]<<8)|data[1];
}

unsigned char CMPS12GetCalibrationState(unsigned char deviceID)
{
    unsigned char data;
    CMPS12ReadNRegister( deviceID, CMPS12_CALIBRATION_STATE,&data, 1 );
    return data;
}

void CMPS12StoreCalibrationProfile(unsigned char deviceID)
{
    unsigned char dat=0xF0;
    CMPS12WriteNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1);
    wait_ms(20);
    dat=0xF5;
    CMPS12WriteNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1);
    wait_ms(20);
    dat=0xF6;
    CMPS12WriteNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1);
    wait_ms(20);
}

void CMPS12DeleteCalibrationProfile(unsigned char deviceID)
{
    unsigned char dat=0xE0;
    CMPS12WriteNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1);
    wait_ms(20);
    dat=0xE5;
    CMPS12WriteNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1);
    wait_ms(20);
    dat=0xE2;
    CMPS12WriteNRegister( deviceID, CMPS12_COMMAND_REG, &dat, 1);
    wait_ms(20);
}
