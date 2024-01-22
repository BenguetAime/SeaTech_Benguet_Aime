/*
 * File:   UTLN_ICM20948.c
 * Author: Barchasz
 *
 * Created on 16 janvier 2019, 15:55
 */

#include "xc.h"
#include "UTLN_ICM20948.h"
#include "UTLN_ICM20948_REG.h"
#include "Define.h"

#ifndef Nop
    #define Nop ;
#endif

ICM20948USER_SPI_Func spiFunc;
unsigned long clockFrequency;
void wait_ms(unsigned short ms)
{
    unsigned long i;
    if(clockFrequency!=0)
        for(i=0;i<(clockFrequency/1000)*ms;i++)
            Nop();
        //__delay32((clockFrequency/1000)*ms);
}
void wait_us(unsigned short us)
{
    unsigned long i;
    if(clockFrequency!=0)
        for(i=0;i<(clockFrequency/1000000)*us;i++)
            Nop();
        //__delay32((clockFrequency/1000000)*us);
}
void ICM20948RegisterUserSystemClock(unsigned long ticksPerSec)
{
    clockFrequency=ticksPerSec;
}

//SPI Related Functions
void ICM20948RegisterUserSPIFunc(ICM20948USER_SPI_Func func)
{
    if(func != NULL)
        spiFunc = func;
}
void ICM20948SPIWriteSingleMultipleRead(unsigned char command, unsigned char results[], int nbRead, unsigned char csID)
{
    unsigned char comm[1];
    comm[0]=ICM20948_READ_MASK(command);
    if(spiFunc!=NULL)
        (*spiFunc)(comm,1,results,nbRead, csID);
}
void ICM20948SPIWriteMultipleMultipleRead(unsigned char command[],int nbWrite, unsigned char results[], int nbRead, unsigned char csID)
{
    if(spiFunc!=NULL)
        (*spiFunc)(command,nbWrite,results,nbRead,csID);
}


//ICM20948 Functions
void ICM20948WriteRegister(unsigned char regAddress,unsigned char value, unsigned char csID)
{
    unsigned char comm[2],result[1];
    comm[0]=ICM20948_WRITE_MASK(regAddress);
    comm[1]=value;
    ICM20948SPIWriteMultipleMultipleRead(comm,2,result,0, csID);
}

void ICM20948WriteRegisterDelay(unsigned char regAddress,unsigned char value, unsigned char csID)
{
    unsigned char comm[2],result[1];
    comm[0]=ICM20948_WRITE_MASK(regAddress);
    comm[1]=value;
    ICM20948SPIWriteMultipleMultipleRead(comm,2,result,0, csID);
    //wait_us(SPI_DELAY);
}

unsigned char ICM20948ReadRegister(unsigned char regAddress, unsigned char csID)
{
    unsigned char comm[2],result[1];
    comm[0]=ICM20948_READ_MASK(regAddress);
    ICM20948SPIWriteMultipleMultipleRead(comm,1,result,1, csID);
    return result[0];
}

unsigned char ICM20948ReadRegisterDelay(unsigned char regAddress, unsigned char csID)
{
    unsigned char comm[2],result[1];
    comm[0]=ICM20948_READ_MASK(regAddress);
    ICM20948SPIWriteMultipleMultipleRead(comm,1,result,1, csID);
    return result[0];
    //wait_us(SPI_DELAY);
}

void ICM20948Reset(unsigned char csID)
{
    PWR_MGMT_1bits pwrMgmt;
    pwrMgmt.DEVICE_RESET=1;
    pwrMgmt.CLKSEL=1;
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    ICM20948WriteRegister(PWR_MGMT_1,pwrMgmt.ALL,csID);
    wait_us(1000);
}

unsigned char readVal;
unsigned char ICM20948Init_default(unsigned char csID)
{
    ICM20948Reset(csID);
    
    //CONFIG USER BANK 0
    PWR_MGMT_1bits pwrMgmt;
    pwrMgmt.ALL=0x00;
    pwrMgmt.CLKSEL=0b001;    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    ICM20948WriteRegister(PWR_MGMT_1,pwrMgmt.ALL,csID);
    //readVal=ICM20948ReadRegister(PWR_MGMT_1,csID);
    readVal=ICM20948ReadRegisterDelay(WHO_AM_I, csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    if(readVal!=ICM20948_ID)
        return 0;
    //wait_us(SPI_DELAY);
        
    USER_CTRLbits userCtrl;
    userCtrl.I2C_IF_DIS=1;
    //userCtrl.DMP_EN=0;      //1:Activate DMP Feature
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    ICM20948WriteRegisterDelay(USER_CTRL,userCtrl.ALL,csID);
    //readVal=ICM20948ReadRegister(USER_CTRL,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    
    LP_CONFIGbits lpConfig;
    lpConfig.ALL=0x00;
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    ICM20948WriteRegisterDelay(LP_CONFIG,lpConfig.ALL,csID);
    //readVal=ICM20948ReadRegister(LP_CONFIG,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);  

    INT_PIN_CFGbits intPinConfig;
    intPinConfig.INT1_ACTL=0;
    intPinConfig.INT1_OPEN=1;
    intPinConfig.INT1_LATCH_INT_EN=1;
    intPinConfig.INT_ANYRD_2CLEAR=1;
    intPinConfig.BYPASS_EN=0;       /* I2C master pins bypass mode if disabled */
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    ICM20948WriteRegisterDelay(INT_PIN_CFG,intPinConfig.ALL,csID);
    //readVal=ICM20948ReadRegister(INT_PIN_CFG,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    ICM20948WriteRegisterDelay(INT_ENABLE_1,0x01,csID);      //Activate DataRdy Interrupt
    //readVal=ICM20948ReadRegister(INT_ENABLE_1,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    
    //CONFIG USER BANK 2    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    ICM20948SetGyroSMPLRTDiv(10, csID);  //10: 100Hz
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
   /// __delay32(SPI_DELAY);
    //readVal=ICM20948ReadRegister(GYRO_SMPLRT_DIV,csID);

    //Config des filtres LP du gyro
    GYRO_CFG_1bits gyroConfig1;
    gyroConfig1.GYRO_FCHOICE=1;    
    gyroConfig1.GYRO_DLPFCFG=0b000;     //0b000 LPCFG 196.6
                                        //0b001 LPCFG 151.8
                                        //0b010 LPCFG 119.5
                                        //0b011 LPCFG 51.2
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    ICM20948WriteRegisterDelay(GYRO_CONFIG_1,gyroConfig1.ALL,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
        
    //Config de la résolution du gyro
    ICM20948SetGyroResolution(GYRO_FULLSCALE_500DPS,csID);   
   
    //readVal=ICM20948ReadRegister(GYRO_CONFIG_1,csID);
    wait_us(SPI_DELAY);
    GYRO_CFG_2bits gyroConfig2;
    gyroConfig2.GYRO_AVGCFG=0b100;          //0b100 Average 16X
                                            //0b101 Average 32X
                                            //0b110 Average 64X
                                            //0b110 Average 128X
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    ICM20948WriteRegisterDelay(GYRO_CONFIG_2,gyroConfig2.ALL,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    
    //Mise à 0 des offsets du gyro
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    ICM20948WriteRegisterDelay(XG_OFFS_USRH,0,csID);
    ICM20948WriteRegisterDelay(XG_OFFS_USRL,0,csID);
    ICM20948WriteRegisterDelay(YG_OFFS_USRH,0,csID);
    ICM20948WriteRegisterDelay(YG_OFFS_USRL,0,csID);
    ICM20948WriteRegisterDelay(ZG_OFFS_USRH,0,csID);
    ICM20948WriteRegisterDelay(ZG_OFFS_USRL,0,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);

    //TODO : le réglage de l'ODR ne semble pas avoir d'effet
    ICM20948SetAccelSMPLRTDiv(1,csID); //ODR is computed as follows : 1.125 kHz/(1+ACCEL_SMPLRT_DIV[11:0])
    
//    wait_us(SPI_DELAY);
//    readVal=ICM20948ReadRegister(ACCEL_SMPLRT_DIV_2,csID);
    
    //Config du low pass filter
    wait_us(SPI_DELAY);
    ACCEL_CONFIGbits accelConfig;
    accelConfig.ACCEL_FCHOICE=1;   //1 : activate Low Pass filter
    //accelConfig.ACCEL_DLPFCFG=0b011;    // 011 : LP FC = 50.4Hz
    accelConfig.ACCEL_DLPFCFG=0b101;    // 101 : LP FC = 11.5Hz  
    //accelConfig.ACCEL_DLPFCFG=0b111;    // 101 : LP FC = 11.5Hz 
    ICM20948WriteRegisterDelay(ACCEL_CONFIG,accelConfig.ALL,csID); 
    
    //Config de la résolution de l'accéléromètre
    ICM20948SetAccelResolution(ACCEL_FULLSCALE_2G,csID);
        
    //Mise à 0 des offsets du gyro
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    ICM20948WriteRegisterDelay(XA_OFFS_H,0,csID);
    ICM20948WriteRegisterDelay(XA_OFFS_L,0,csID);
    ICM20948WriteRegisterDelay(YA_OFFS_H,0,csID);
    ICM20948WriteRegisterDelay(YA_OFFS_L,0,csID);
    ICM20948WriteRegisterDelay(ZA_OFFS_H,0,csID);
    ICM20948WriteRegisterDelay(ZA_OFFS_L,0,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    
    
    //===================SetSampleRate========================================//
    //ICM20948SetSampleRate((float)50.0,csID);  
    
    //COnfig de ?????  TODO comment
    ACCEL_CONFIG_2bits accelConfig2;
    accelConfig2.DEC3_CFG=0b11;
    //accelConfig2.DEC3_CFG=0b11;
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    ICM20948WriteRegisterDelay(ACCEL_CONFIG_2,accelConfig2.ALL,csID);    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);  
    

    return 1;
}

void ICM20948SetGyroSMPLRTDiv(unsigned char div,unsigned char csID)
{
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    ICM20948WriteRegisterDelay(GYRO_SMPLRT_DIV,div,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}



void ICM20948SetAccelSMPLRTDiv(unsigned short div,unsigned char csID)
{
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    ICM20948WriteRegisterDelay(ACCEL_SMPLRT_DIV_1,(unsigned char)MSB_UINT16(div),csID);
    //readVal=ICM20948ReadRegister(ACCEL_SMPLRT_DIV_1,csID);
    ICM20948WriteRegisterDelay(ACCEL_SMPLRT_DIV_2,(unsigned char)LSB_UINT16(div),csID);
    //readVal=ICM20948ReadRegister(ACCEL_SMPLRT_DIV_2,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}



unsigned char* ICM20948GetAccelValues(unsigned char csID)
{
    static unsigned char dataRead[6];
    ICM20948SPIWriteSingleMultipleRead(ACCEL_XOUT_H, dataRead, 6, csID);
    return dataRead;
}

void ICM20948GetAccelValues_(unsigned char* data,unsigned char csID)
{
    ICM20948SPIWriteSingleMultipleRead(ACCEL_XOUT_H, data, 6, csID);
}

unsigned char* ICM20948GetGyroValues(unsigned char csID)
{
    static unsigned char dataRead[6];
    ICM20948SPIWriteSingleMultipleRead(GYRO_XOUT_H, dataRead, 6, csID);
    return dataRead;
}

unsigned char* ICM20948GetTempValues(unsigned char csID)
{
    static unsigned char dataRead[2];
    ICM20948SPIWriteSingleMultipleRead(TEMP_OUT_H, dataRead, 2, csID);
    return dataRead;
}


//========================ADVANCED FUNCTION===================================//
float ICM20948SetGyroSampleRate(float sampleRate, unsigned char csID)
{
    uint8_t gyroDiv;
    float gyroSampleRate;
 
    /* Calculate the sample rate divider */
    gyroSampleRate = (1125.0 / sampleRate) - 1.0;
 
    /* Check if it fits in the divider register */
    if ( gyroSampleRate > 255.0 ) {
        gyroSampleRate = 255.0;
    }
 
    if ( gyroSampleRate < 0 ) {
        gyroSampleRate = 0.0;
    }
 
    /* Write the value to the register */
    gyroDiv = (uint8_t) gyroSampleRate;
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    ICM20948WriteRegisterDelay(GYRO_SMPLRT_DIV, gyroDiv,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    //wait_us(SPI_DELAY);
    /* Calculate the actual sample rate from the divider value */
    gyroSampleRate = 1125.0 / (gyroDiv + 1);
 
    return gyroSampleRate;
}

float ICM20948SetAccelSampleRate(float sampleRate, unsigned char csID)
{
    uint16_t accelDiv;
    float accelSampleRate;
 
    /* Calculate the sample rate divider */
    accelSampleRate = (1125.0 / sampleRate) - 1.0;
 
    /* Check if it fits in the divider registers */
    if ( accelSampleRate > 4095.0 ) {
        accelSampleRate = 4095.0;
    }
 
    if ( accelSampleRate < 0 ) {
        accelSampleRate = 0.0;
    }
 
    /* Write the value to the registers */
    accelDiv = (uint16_t) accelSampleRate;
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    ICM20948WriteRegisterDelay(ACCEL_SMPLRT_DIV_1, (uint8_t) (accelDiv >> 8), csID );
    ICM20948WriteRegisterDelay(ACCEL_SMPLRT_DIV_2, (uint8_t) (accelDiv & 0xFF), csID );
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
 
    /* Calculate the actual sample rate from the divider value */
    accelSampleRate = 1125.0 / (accelDiv + 1);
 
    return accelSampleRate;
}

/***************************************************************************//**
 * @brief
 *    Sets the sample rate both of the accelerometer and the gyroscope.
 *
 * @param[in] sampleRate
 *    The desired sample rate in Hz. Since the resolution of the sample rate
 *    divider is different in the accel and gyro stages it is possible that
 *    the two sensor will have different sample rate set.
 *
 ******************************************************************************/
void ICM20948SetSampleRate(float sampleRate,unsigned char csID)
{
    ICM20948SetGyroSampleRate(sampleRate,csID);
    ICM20948SetAccelSampleRate(sampleRate,csID);
}
/***************************************************************************//**
 * @brief
 *    Sets the bandwidth of the gyroscope
 *
 * @param[in] gyroBw
 *    The desired bandwidth value. Use the ICM20648_GYRO_BW_xHZ macros, which
 *    are defined in the icm20648.h file. The value of x can be
 *    6, 12, 24, 51, 120, 150, 200, 360 or 12100.
 *
 ******************************************************************************/
void ICM20948SetGyroBandwidth(uint8_t gyroBw, unsigned char csID)
{
    GYRO_CFG_1bits reg;
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    /* Read the GYRO_CONFIG_1 register */
    reg.ALL=ICM20948ReadRegisterDelay(GYRO_CONFIG_1, csID);
 
    /* Write the new bandwidth value to the gyro config register */
    reg.GYRO_DLPFCFG= gyroBw;
    if(gyroBw!=GYRO_BW_12100HZ)
    {
        reg.GYRO_FCHOICE=1;
    }
    else
    {
        reg.GYRO_DLPFCFG= 0;
    }
    ICM20948WriteRegisterDelay(GYRO_CONFIG_1, reg.ALL, csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}
 
/***************************************************************************//**
 * @brief
 *    Sets the bandwidth of the accelerometer
 *
 * @param[in] accelBw
 *    The desired bandwidth value. Use the ICM20648_ACCEL_BW_yHZ macros, which
 *    are defined in the icm20648.h file. The value of y can be
 *    6, 12, 24, 50, 111, 246, 470 or 1210.
 *
 ******************************************************************************/
void ICM20948SetAccelBandwidth(uint8_t accelBw, unsigned char csID)
{
    ACCEL_CONFIGbits reg;
 
    /* Read the GYRO_CONFIG_1 register */
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    reg.ALL=ICM20948ReadRegisterDelay(ACCEL_CONFIG, csID);
    
    /* Write the new bandwidth value to the gyro config register */
    reg.ACCEL_DLPFCFG= accelBw ;
    if(accelBw!=ACCEL_BW_1210HZ)
    {
        reg.ACCEL_FCHOICE=1;
    }
    else
    {
        reg.ACCEL_DLPFCFG= 0;
    }
    ICM20948WriteRegisterDelay(ACCEL_CONFIG, reg.ALL,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}

float ICM20948AccelResolutionMultiplier = 0; //Variable "privée" mise à jour lors du SetAccel
/***************************************************************************//**
 * @brief
 *    Sets the full scale value of the accelerometer
 *
 * @param[in] accelFs
 *    The desired full scale value. Use the ICM20648_ACCEL_FULLSCALE_xG
 *    macros, which are defined in the icm20648.h file. The value of x can be
 *    2, 4, 8 or 16.
 *    En même temps, le coefficient multiplicateur de la sortie (en m.s-2)
 *    est stocké pour éviter d'avoir à la calculer ensuite.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
void ICM20948SetAccelResolution(uint8_t accelResolution, unsigned char csID)
{
    ACCEL_CONFIGbits reg;
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    reg.ALL=ICM20948ReadRegisterDelay(ACCEL_CONFIG, csID);
    reg.ACCEL_FS_SEL= accelResolution;
    ICM20948WriteRegisterDelay(ACCEL_CONFIG, reg.ALL, csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID); 
    
    //On relis le registre pour utiliser la valeur réelle pour ajuster la 
    //constante de multiplication
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    reg.ALL=ICM20948ReadRegisterDelay(ACCEL_CONFIG, csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
     
    /* Calculate the resolution */
    switch ( reg.ACCEL_FS_SEL ) {
        case ACCEL_FULLSCALE_2G:
            ICM20948AccelResolutionMultiplier = 2.0 / 32768.0 * 9.81;
            break;
 
        case ACCEL_FULLSCALE_4G:
            ICM20948AccelResolutionMultiplier = 4.0 / 32768.0 * 9.81;
            break;
 
        case ACCEL_FULLSCALE_8G:
            ICM20948AccelResolutionMultiplier = 8.0 / 32768.0 * 9.81;
            break;
 
        case ACCEL_FULLSCALE_16G:
            ICM20948AccelResolutionMultiplier = 16.0 / 32768.0 * 9.81;
            break;
    }
}

float ICM20948GetAccelResolutionConstant()
{
    return ICM20948AccelResolutionMultiplier;
}
 

float ICM20948GyroResolutionMultiplier = 0; //Variable "privée" mise à jour lors du SetAccel
/***************************************************************************//**
 * @brief
 *    Sets the full scale value of the gyroscope
 *
 * @param[in] gyroFs
 *    The desired full scale value. Use the ICM20648_GYRO_FULLSCALE_yDPS
 *    macros, which are defined in the icm20648.h file. The value of y can be
 *    250, 500, 1000 or 2000.
 *
 ******************************************************************************/

//void ICM20948GetGyroResolution(float *accelRes, unsigned char csID);
void ICM20948SetGyroResolution(uint8_t gyroFs, unsigned char csID)
{
    GYRO_CFG_1bits reg;
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    reg.ALL=ICM20948ReadRegisterDelay(GYRO_CONFIG_1, csID);
    reg.GYRO_FS_SEL= gyroFs;
    ICM20948WriteRegisterDelay(GYRO_CONFIG_1, reg.ALL, csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    
    //On relis le registre pour utiliser la valeur réelle pour ajuster la 
    //constante de multiplication
    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
    reg.ALL=ICM20948ReadRegisterDelay(GYRO_CONFIG_1, csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
     
    /* Calculate the resolution */
    switch ( reg.GYRO_FS_SEL ) {
        case GYRO_FULLSCALE_250DPS:
            ICM20948GyroResolutionMultiplier = 250.0*(M_PI/180.0)/32768.0;
            break;
 
        case GYRO_FULLSCALE_500DPS:
            ICM20948GyroResolutionMultiplier = 500.0*(M_PI/180.0)/32768.0;
            break;
 
        case GYRO_FULLSCALE_1000DPS:
            ICM20948GyroResolutionMultiplier = 1000.0*(M_PI/180.0)/32768.0;
            break;
 
        case GYRO_FULLSCALE_2000DPS:
            ICM20948GyroResolutionMultiplier = 2000.0*(M_PI/180.0)/32768.0;
            break;
    }
}

float ICM20948GetGyroResolutionConstant()
{
    return ICM20948GyroResolutionMultiplier;
}

///***************************************************************************//**
// * @brief
// *    Gets the actual resolution of the accelerometer
// *
// * @param[out] accelRes
// *    The resolution in g/bit units
// *
// ******************************************************************************/
//void ICM20948GetAccelResolution(float *accelRes, unsigned char csID)
//{
//    ACCEL_CONFIGbits reg;
// 
//    /* Read the actual acceleration full scale setting */
//    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
//    reg.ALL=ICM20948ReadRegister(ACCEL_CONFIG, csID);
//    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
//    /* Calculate the resolution */
//    switch ( reg.ACCEL_FS_SEL ) {
//        case ACCEL_FULLSCALE_2G:
//            *accelRes = 2.0 / 32768.0;
//            break;
// 
//        case ACCEL_FULLSCALE_4G:
//            *accelRes = 4.0 / 32768.0;
//            break;
// 
//        case ACCEL_FULLSCALE_8G:
//            *accelRes = 8.0 / 32768.0;
//            break;
// 
//        case ACCEL_FULLSCALE_16G:
//            *accelRes = 16.0 / 32768.0;
//            break;
//    }
//}
 
/***************************************************************************//**
 * @brief
 *    Gets the actual resolution of the gyroscope
 *
 * @param[out] gyroRes
 *    The actual resolution in (deg/sec)/bit units
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
//void ICM20948GetGyroResolution(float *gyroRes, unsigned char csID)
//{
//    GYRO_CFG_1bits reg;
// 
//    /* Read the actual gyroscope full scale setting */
//    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
//    reg.ALL=ICM20948ReadRegister(GYRO_CONFIG_1, csID);
//    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
//    /* Calculate the resolution */
//    switch ( reg.GYRO_FS_SEL ) {
//        case GYRO_FULLSCALE_250DPS:
//            *gyroRes = 250.0 / 32768.0;
//            break;
// 
//        case GYRO_FULLSCALE_500DPS:
//            *gyroRes = 500.0 / 32768.0;
//            break;
// 
//        case GYRO_FULLSCALE_1000DPS:
//            *gyroRes = 1000.0 / 32768.0;
//            break;
// 
//        case GYRO_FULLSCALE_2000DPS:
//            *gyroRes = 2000.0 / 32768.0;
//            break;
//    }
//}

/***************************************************************************//**
 * @brief
 *    Enables or disables the sleep mode of the device
 *
 * @param[in] enable
 *    If true, sleep mode is enabled. Set to false to disable sleep mode.
 *
 ******************************************************************************/
void ICM20648EnableSleepmode(bool enable, unsigned char csID)
{
    PWR_MGMT_1bits reg;
 
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    reg.ALL=ICM20948ReadRegisterDelay(PWR_MGMT_1, csID); 
    if ( enable ) {
        /* Sleep: set the SLEEP bit */
        reg.SLEEP=1;
    } else {
        /* Wake up: clear the SLEEP bit */
        reg.SLEEP=0;;
    } 
    ICM20948WriteRegisterDelay(PWR_MGMT_1, reg.ALL,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}

 
/***************************************************************************//**
 * @brief
 *    Enables or disables the cycle mode operation of the accel and gyro
 *
 * @param[in] enable
 *    If true both the accel and gyro sensors will operate in cycle mode. If
 *    false the senors working in continuous mode.
 *
 ******************************************************************************/
void ICM20648EnableCyclemode(bool enable, unsigned char csID)
{
    LP_CONFIGbits reg;
 
    reg.ALL = 0x00;
 
    if ( enable ) {
        reg.ACCEL_CYCLE=1;
        reg.GYRO_CYCLE=1;
    }
 
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    ICM20948WriteRegisterDelay(LP_CONFIG, reg.ALL, csID);    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}
 
/***************************************************************************//**
 * @brief
 *    Enables or disables the sensors in the ICM20648 chip
 *
 * @param[in] accel
 *    If true enables the acceleration sensor
 *
 * @param[in] gyro
 *    If true enables the gyroscope sensor
 *
 * @param[in] temp
 *    If true enables the temperature sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
void ICM20948EnableSensor(bool accel, bool gyro, bool temp, unsigned char csID)
{
    PWR_MGMT_1bits pwrManagement1;
    PWR_MGMT_2bits pwrManagement2;
 
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    pwrManagement1.ALL=ICM20948ReadRegisterDelay(PWR_MGMT_1, csID);
    pwrManagement2.ALL = 0;
 
    /* To enable the accelerometer clear the DISABLE_ACCEL bits in PWR_MGMT_2 */
    if ( accel ) {
        pwrManagement2.DISABLE_ACCEL=0;
    } else {
        pwrManagement2.DISABLE_ACCEL=1;
    }
 
    /* To enable gyro clear the DISABLE_GYRO bits in PWR_MGMT_2 */
    if ( gyro ) {
        pwrManagement2.DISABLE_GYRO=0;
    } else {
        pwrManagement2.DISABLE_GYRO=1;
    }
 
    /* To enable the temperature sensor clear the TEMP_DIS bit in PWR_MGMT_1 */
    if ( temp ) {
        pwrManagement1.TEMP_DIS=0;
    } else {
        pwrManagement1.TEMP_DIS=1;
    }
 
    /* Write back the modified values */
    ICM20948WriteRegisterDelay(PWR_MGMT_1, pwrManagement1.ALL, csID);
    ICM20948WriteRegisterDelay(PWR_MGMT_2, pwrManagement2.ALL, csID);    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);

}
 
/***************************************************************************//**
 * @brief
 *    Enables or disables the sensors in low power mode in the ICM20648 chip
 *
 * @param[in] enAccel
 *    If true enables the acceleration sensor in low power mode
 *
 * @param[in] enGyro
 *    If true enables the gyroscope sensor in low power mode
 *
 * @param[in] enTemp
 *    If true enables the temperature sensor in low power mode
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
void ICM20948EnterLowPowerMode(bool enAccel, bool enGyro, bool enTemp, unsigned char csID)
{
    PWR_MGMT_1bits data;
 
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    data.ALL=ICM20948ReadRegisterDelay(PWR_MGMT_1, csID);
 
    if ( enAccel || enGyro || enTemp ) {
        /* Make sure that the chip is not in sleep */
        ICM20648EnableSleepmode(false, csID);
 
        /* And in continuous mode */
        ICM20648EnableCyclemode(false, csID);
 
        /* Enable the accelerometer and the gyroscope*/
        ICM20948EnableSensor(enAccel, enGyro, enTemp, csID);
        wait_ms(50);
 
        /* Enable cycle mode */
        ICM20648EnableCyclemode(true, csID);
 
        /* Set the LP_EN bit to enable low power mode */
        data.LP_EN=1;
    } else {
        /* Enable continuous mode */
        ICM20648EnableSleepmode(false, csID);
 
        /* Clear the LP_EN bit to disable low power mode */
        data.LP_EN= 0;
    }
 
    /* Write the updated value to the PWR_MGNT_1 register */
    ICM20948WriteRegisterDelay(PWR_MGMT_1, data.ALL, csID);    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}
 
/***************************************************************************//**
 * @brief
 *    Enables or disables the interrupts in the ICM20648 chip
 *
 * @param[in] dataReadyEnable
 *    If true enables the Raw Data Ready interrupt, otherwise disables.
 *
 * @param[in] womEnable
 *    If true enables the Wake-up On Motion interrupt, otherwise disables.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
void ICM20948EnableIrq(bool dataReadyEnable, bool womEnable, unsigned char csID)
{
    INT_ENABLEbits intEnable;
 
    /* All interrupts disabled by default */
    intEnable.ALL = 0;
 
    /* Enable one or both of the interrupt sources if required */
    if ( womEnable ) {
        intEnable.WOM_INT_EN=1;
    }
    /* Write value to register */    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
    ICM20948WriteRegisterDelay(INT_ENABLE, intEnable.ALL, csID);
 
    /* All interrupts disabled by default */
    intEnable.ALL = 0;
 
    if ( dataReadyEnable ) {
        intEnable.ALL=0x01;
    }
 
    /* Write value to register */
    ICM20948WriteRegister(INT_ENABLE_1, intEnable.ALL,csID);    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}

/***************************************************************************//**
 * @brief
 *    Sets up and enables the Wake-up On Motion feature
 *
 * @param[in] enable
 *    If true enables the WOM feature, disables otherwise
 *
 * @param[in] womThreshold
 *    Threshold value for the Wake on Motion Interrupt for ACCEL x/y/z axes.
 *    LSB = 4mg. Range is 0mg to 1020mg
 *
 * @param[in] sampleRate
 *    The desired sample rate of the accel sensor in Hz
 *
 ******************************************************************************/
void ICM20948EnableWakeOnMotion(bool enable, uint8_t womThreshold, float sampleRate, unsigned char csID)
{
    if ( enable ) {
        /* Make sure that the chip is not in sleep */
        ICM20648EnableSleepmode(false,csID);
 
        /* And in continuous mode */
        ICM20648EnableCyclemode(false,csID);
 
        /* Enable only the accelerometer */
        ICM20948EnableSensor(true, false, false,csID);
 
        /* Set sample rate */
        ICM20948SetSampleRate(sampleRate,csID);
 
        /* Set the bandwidth to 1210Hz */
        ICM20948SetAccelBandwidth(ACCEL_BW_1210HZ,csID);
 
        /* Accel: 2G full scale */
        ICM20948SetAccelResolution(ACCEL_FULLSCALE_2G,csID);
        //ICM20948SetAccelFullscale(ACCEL_FULLSCALE_2G,csID);
 
        /* Enable the Wake On Motion interrupt */
        ICM20948EnableIrq(false, true,csID);
        wait_ms(50);
 
        /* Enable Wake On Motion feature */
        ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
        ICM20948WriteRegisterDelay(ACCEL_INTEL_CTRL, BIT_ACCEL_INTEL_EN | BIT_ACCEL_INTEL_MODE, csID);
 
        /* Set the wake on motion threshold value */
        ICM20948WriteRegisterDelay(ACCEL_WOM_THR, womThreshold, csID);
        ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
 
        /* Enable low power mode */
        ICM20948EnterLowPowerMode(true, false, false, csID);
    } else {
        /* Disable Wake On Motion feature */
        ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
        ICM20948WriteRegisterDelay(ACCEL_INTEL_CTRL, 0x00, csID);
        ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
 
        /* Disable the Wake On Motion interrupt */
        ICM20948EnableIrq(false, false,csID);
 
        /* Disable cycle mode */
        ICM20648EnableCyclemode(false,csID);
    }
}
 
///***************************************************************************//**
// * @brief
// *    Accelerometer and gyroscope calibration function. Reads the gyroscope
// *    and accelerometer values while the device is at rest and in level. The
// *    resulting values are loaded to the accel and gyro bias registers to cancel
// *    the static offset error.
// *
// * @param[out] accelBiasScaled
// *    The mesured acceleration sensor bias in mg
// *
// * @param[out] gyroBiasScaled
// *    The mesured gyro sensor bias in deg/sec
// *
// * @return
// *    Returns zero on OK, non-zero otherwise
// ******************************************************************************/
//void ICM20948Calibrate(float *accelBiasScaled, float *gyroBiasScaled,unsigned char csID)
//{
//    uint8_t data[12];
//    uint16_t i, packetCount, fifoCount;
//    int32_t gyroBias[3] = { 0, 0, 0 };
//    int32_t accelBias[3] = { 0, 0, 0 };
//    int32_t accelTemp[3];
//    int32_t gyroTemp[3];
//    int32_t accelBiasFactory[3];
//    int32_t gyroBiasStored[3];
//    float gyroRes; //, accelRes;
// 
//    /* Enable the accelerometer and the gyro */
//    ICM20948EnableSensor(true, true, false, csID);
// 
//    /* Set 1kHz sample rate */
//    ICM20948SetSampleRate(1100.0,csID);
// 
//    /* 246Hz BW for the accelerometer and 200Hz for the gyroscope */
//    ICM20948SetAccelBandwidth(ACCEL_BW_246HZ, csID);
//    ICM20948SetGyroBandwidth(GYRO_BW_12HZ,csID);
// 
//    /* Set the most sensitive range: 2G full scale and 250dps full scale */
//    ICM20948SetAccelResolution(ACCEL_FULLSCALE_2G,csID);
//    //ICM20948SetAccelFullscale(ACCEL_FULLSCALE_2G,csID);
//    ICM20948SetGyroFullscale(GYRO_FULLSCALE_250DPS,csID);
// 
//    /* Retrieve the resolution per bit */
//    //ICM20948GetAccelResolution(&accelRes, csID);
//    ICM20948GetGyroResolution(&gyroRes,csID);
// 
//    /* The accel sensor needs max 30ms, the gyro max 35ms to fully start */
//    /* Experiments show that the gyro needs more time to get reliable results */
//    wait_ms(50);
// 
//    /* Disable the FIFO */
//    ICM20948WriteRegister(USER_CTRL, 0x00,csID);
//    ICM20948WriteRegister(FIFO_MODE, 0x0F, csID);
// 
//    /* Enable accelerometer and gyro to store the data in FIFO */
//    FIFO_EN_2bits reg;
//    reg.ACCEL_FIFO_EN=1;
//    reg.GYRO_X_FIFO_EN=1;
//    reg.GYRO_Y_FIFO_EN=1;
//    reg.GYRO_Z_FIFO_EN=1;
//    ICM20948WriteRegister(FIFO_EN_2, reg.ALL, csID);
// 
//    /* Reset the FIFO */
//    ICM20948WriteRegister(FIFO_RST, 0x0F, csID);
//    ICM20948WriteRegister(FIFO_RST, 0x00, csID);
// 
//    /* Enable the FIFO */
//    USER_CTRLbits user;
//    user.FIFO_EN=1;
//    ICM20948WriteRegister(USER_CTRL, user.ALL, csID);
// 
//    /* The max FIFO size is 4096 bytes, one set of measurements takes 12 bytes */
//    /* (3 axes, 2 sensors, 2 bytes each value ) 340 samples use 4080 bytes of FIFO */
//    /* Loop until at least 4080 samples gathered */
//    fifoCount = 0;
//    while ( fifoCount < 4080 ) {
//        wait_ms(5);
//        /* Read FIFO sample count */
//        ICM20948SPIWriteSingleMultipleRead(FIFO_COUNTH, &data[0], 2, csID);
//        /* Convert to a 16 bit value */
//        fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);
//    }
// 
//    /* Disable accelerometer and gyro to store the data in FIFO */
//    ICM20948WriteRegister(FIFO_EN_2, 0x00, csID);
// 
//    /* Read FIFO sample count */
//    ICM20948SPIWriteSingleMultipleRead(FIFO_COUNTH, &data[0], 2, csID);
// 
//    /* Convert to a 16 bit value */
//    fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);
// 
//    /* Calculate the number of data sets (3 axis of accel an gyro, two bytes each = 12 bytes) */
//    packetCount = fifoCount / 12;
// 
//    /* Retrieve the data from the FIFO */
//    for ( i = 0; i < packetCount; i++ ) {
//        ICM20948SPIWriteSingleMultipleRead(FIFO_R_W, &data[0], 12, csID);
//        /* Convert to 16 bit signed accel and gyro x,y and z values */
//        accelTemp[0] = ( (int16_t) (data[0] << 8) | data[1]);
//        accelTemp[1] = ( (int16_t) (data[2] << 8) | data[3]);
//        accelTemp[2] = ( (int16_t) (data[4] << 8) | data[5]);
//        gyroTemp[0] = ( (int16_t) (data[6] << 8) | data[7]);
//        gyroTemp[1] = ( (int16_t) (data[8] << 8) | data[9]);
//        gyroTemp[2] = ( (int16_t) (data[10] << 8) | data[11]);
// 
//        /* Sum the values */
//        accelBias[0] += accelTemp[0];
//        accelBias[1] += accelTemp[1];
//        accelBias[2] += accelTemp[2];
//        gyroBias[0] += gyroTemp[0];
//        gyroBias[1] += gyroTemp[1];
//        gyroBias[2] += gyroTemp[2];
//    }
// 
//    /* Divide by packet count to get the average */
//    accelBias[0] /= packetCount;
//    accelBias[1] /= packetCount;
//    accelBias[2] /= packetCount;
//    gyroBias[0] /= packetCount;
//    gyroBias[1] /= packetCount;
//    gyroBias[2] /= packetCount;
// 
//    /* Acceleormeter: add or remove (depending on the orientation of the chip) 1G (gravity) from the Z axis value */
//    if ( accelBias[2] > 0L ) {
//        accelBias[2] -= (int32_t) (1.0 / accelRes);
//    } else {
//        accelBias[2] += (int32_t) (1.0 / accelRes);
//    }
// 
//    /* Convert the values to degrees per sec for displaying */
//    gyroBiasScaled[0] = (float) gyroBias[0] * gyroRes;
//    gyroBiasScaled[1] = (float) gyroBias[1] * gyroRes;
//    gyroBiasScaled[2] = (float) gyroBias[2] * gyroRes;
// 
//    /* Read stored gyro trim values. After reset these values are all 0 */
//    ICM20948WriteRegister(REG_BANK_SEL,BANK_2,csID);
//    ICM20948SPIWriteSingleMultipleRead(XG_OFFS_USRH, &data[0], 2, csID);
//    gyroBiasStored[0] = ( (int16_t) (data[0] << 8) | data[1]);
//    ICM20948SPIWriteSingleMultipleRead(YG_OFFS_USRH, &data[0], 2, csID);
//    gyroBiasStored[1] = ( (int16_t) (data[0] << 8) | data[1]);
//    ICM20948SPIWriteSingleMultipleRead(ZG_OFFS_USRH, &data[0], 2, csID);
//    gyroBiasStored[2] = ( (int16_t) (data[0] << 8) | data[1]);
// 
//    /* The gyro bias should be stored in 1000dps full scaled format. We measured in 250dps to get */
//    /* the best sensitivity, so need to divide by 4 */
//    /* Substract from the stored calibration value */
//    gyroBiasStored[0] -= gyroBias[0] / 4;
//    gyroBiasStored[1] -= gyroBias[1] / 4;
//    gyroBiasStored[2] -= gyroBias[2] / 4;
// 
//    /* Split the values into two bytes */
//    data[0] = (gyroBiasStored[0] >> 8) & 0xFF;
//    data[1] = (gyroBiasStored[0]) & 0xFF;
//    data[2] = (gyroBiasStored[1] >> 8) & 0xFF;
//    data[3] = (gyroBiasStored[1]) & 0xFF;
//    data[4] = (gyroBiasStored[2] >> 8) & 0xFF;
//    data[5] = (gyroBiasStored[2]) & 0xFF;
// 
//    /* Write the  gyro bias values to the chip */
//    ICM20948WriteRegister(XG_OFFS_USRH, data[0], csID);
//    ICM20948WriteRegister(XG_OFFS_USRL, data[1], csID);
//    ICM20948WriteRegister(YG_OFFS_USRH, data[2], csID);
//    ICM20948WriteRegister(YG_OFFS_USRL, data[3], csID);
//    ICM20948WriteRegister(ZG_OFFS_USRH, data[4], csID);
//    ICM20948WriteRegister(ZG_OFFS_USRL, data[5], csID);
// 
//    /* Calculate the accelerometer bias values to store in the hardware accelerometer bias registers. These registers contain */
//    /* factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold */
//    /* non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature */
//    /* compensation calculations(? the datasheet is not clear). Accelerometer bias registers expect bias input */
//    /* as 2048 LSB per g, so that the accelerometer biases calculated above must be divided by 8. */
// 
//    /* Read factory accelerometer trim values */
//    ICM20948WriteRegister(REG_BANK_SEL,BANK_1,csID);
//    ICM20948SPIWriteSingleMultipleRead(XA_OFFS_H, &data[0], 2, csID);
//    accelBiasFactory[0] = ( (int16_t) (data[0] << 8) | data[1]);
//    ICM20948SPIWriteSingleMultipleRead(YA_OFFS_H, &data[0], 2, csID);
//    accelBiasFactory[1] = ( (int16_t) (data[0] << 8) | data[1]);
//    ICM20948SPIWriteSingleMultipleRead(ZA_OFFS_H, &data[0], 2, csID);
//    accelBiasFactory[2] = ( (int16_t) (data[0] << 8) | data[1]);
// 
//    /* Construct total accelerometer bias, including calculated average accelerometer bias from above */
//    /* Scale the 2g full scale (most sensitive range) results to 16g full scale - divide by 8 */
//    /* Clear the last bit (temperature compensation? - the datasheet is not clear) */
//    /* Substract from the factory calibration value */
// 
//    accelBiasFactory[0] -= ( (accelBias[0] / 8) & ~1);
//    accelBiasFactory[1] -= ( (accelBias[1] / 8) & ~1);
//    accelBiasFactory[2] -= ( (accelBias[2] / 8) & ~1);
// 
//    /* Split the values into two bytes */
//    data[0] = (accelBiasFactory[0] >> 8) & 0xFF;
//    data[1] = (accelBiasFactory[0]) & 0xFF;
//    data[2] = (accelBiasFactory[1] >> 8) & 0xFF;
//    data[3] = (accelBiasFactory[1]) & 0xFF;
//    data[4] = (accelBiasFactory[2] >> 8) & 0xFF;
//    data[5] = (accelBiasFactory[2]) & 0xFF;
// 
//    /* Store them in the accelerometer offset registers */
//    ICM20948WriteRegister(XA_OFFS_H, data[0],csID);
//    ICM20948WriteRegister(XA_OFFS_L, data[1],csID);
//    ICM20948WriteRegister(YA_OFFS_H, data[2],csID);
//    ICM20948WriteRegister(YA_OFFS_L, data[3],csID);
//    ICM20948WriteRegister(ZA_OFFS_H, data[4],csID);
//    ICM20948WriteRegister(ZA_OFFS_L, data[5],csID);
//    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
//    /* Convert the values to G for displaying */
//    accelBiasScaled[0] = (float) accelBias[0] * accelRes;
//    accelBiasScaled[1] = (float) accelBias[1] * accelRes;
//    accelBiasScaled[2] = (float) accelBias[2] * accelRes;
// 
//    /* Turn off FIFO */
//    ICM20948WriteRegister(USER_CTRL, 0x00,csID);
// 
//    /* Disable all sensors */
//    ICM20948EnableSensor(false, false, false,csID);
//}
 
/***************************************************************************//**
 * @brief
 *    Gyroscope calibration function. Reads the gyroscope
 *    values while the device is at rest and in level. The
 *    resulting values are loaded to the gyro bias registers to cancel
 *    the static offset error.
 *
 * @param[out] gyroBiasScaled
 *    The mesured gyro sensor bias in deg/sec
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
//uint32_t ICM20948CalibrateGyro(float *gyroBiasScaled, unsigned char csID)
//{
//    uint8_t data[12];
//    uint16_t i, packetCount, fifoCount;
//    int32_t gyroBias[3] = { 0, 0, 0 };
//    int32_t gyroTemp[3];
//    int32_t gyroBiasStored[3];
//    float gyroRes;
// 
//    /* Enable the accelerometer and the gyro */
//    enable_sensor(true, true, false);
// 
//    /* Set 1kHz sample rate */
//    ICM20948SetSampleRate(1100.0,csID);
// 
//    /* Configure bandwidth for gyroscope to 12Hz */
//    set_gyro_bandwidth(ICM20648_GYRO_BW_12HZ);
// 
//    /* Configure sensitivity to 250dps full scale */
//    set_gyro_fullscale(ICM20648_GYRO_FULLSCALE_250DPS);
// 
//    /* Retrieve the resolution per bit */
//    get_gyro_resolution(&gyroRes);
// 
//    /* The accel sensor needs max 30ms, the gyro max 35ms to fully start */
//    /* Experiments show that the gyro needs more time to get reliable results */
//    wait_ms(50);
// 
//    /* Disable the FIFO */
//    write_register(ICM20648_REG_USER_CTRL, ICM20648_BIT_FIFO_EN);
//    write_register(ICM20648_REG_FIFO_MODE, 0x0F);
// 
//    /* Enable accelerometer and gyro to store the data in FIFO */
//    write_register(ICM20648_REG_FIFO_EN_2, ICM20648_BITS_GYRO_FIFO_EN);
// 
//    /* Reset the FIFO */
//    write_register(ICM20648_REG_FIFO_RST, 0x0F);
//    write_register(ICM20648_REG_FIFO_RST, 0x00);
// 
//    /* Enable the FIFO */
//    write_register(ICM20648_REG_USER_CTRL, ICM20648_BIT_FIFO_EN);
// 
//    /* The max FIFO size is 4096 bytes, one set of measurements takes 12 bytes */
//    /* (3 axes, 2 sensors, 2 bytes each value ) 340 samples use 4080 bytes of FIFO */
//    /* Loop until at least 4080 samples gathered */
//    fifoCount = 0;
//    while ( fifoCount < 4080 ) {
//        wait_ms(5);
// 
//        /* Read FIFO sample count */
//        read_register(ICM20648_REG_FIFO_COUNT_H, 2, &data[0]);
// 
//        /* Convert to a 16 bit value */
//        fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);
//    }
// 
//    /* Disable accelerometer and gyro to store the data in FIFO */
//    write_register(ICM20648_REG_FIFO_EN_2, 0x00);
// 
//    /* Read FIFO sample count */
//    read_register(ICM20648_REG_FIFO_COUNT_H, 2, &data[0]);
// 
//    /* Convert to a 16 bit value */
//    fifoCount = ( (uint16_t) (data[0] << 8) | data[1]);
// 
//    /* Calculate the number of data sets (3 axis of accel an gyro, two bytes each = 12 bytes) */
//    packetCount = fifoCount / 12;
// 
//    /* Retrieve the data from the FIFO */
//    for ( i = 0; i < packetCount; i++ ) {
//        read_register(ICM20648_REG_FIFO_R_W, 12, &data[0]);
//        /* Convert to 16 bit signed accel and gyro x,y and z values */
//        gyroTemp[0] = ( (int16_t) (data[6] << 8) | data[7]);
//        gyroTemp[1] = ( (int16_t) (data[8] << 8) | data[9]);
//        gyroTemp[2] = ( (int16_t) (data[10] << 8) | data[11]);
// 
//        /* Sum the values */
//        gyroBias[0] += gyroTemp[0];
//        gyroBias[1] += gyroTemp[1];
//        gyroBias[2] += gyroTemp[2];
//    }
// 
//    /* Divide by packet count to get the average */
//    gyroBias[0] /= packetCount;
//    gyroBias[1] /= packetCount;
//    gyroBias[2] /= packetCount;
// 
//    /* Convert the values to degrees per sec for displaying */
//    gyroBiasScaled[0] = (float) gyroBias[0] * gyroRes;
//    gyroBiasScaled[1] = (float) gyroBias[1] * gyroRes;
//    gyroBiasScaled[2] = (float) gyroBias[2] * gyroRes;
// 
//    /* Read stored gyro trim values. After reset these values are all 0 */
//    read_register(ICM20648_REG_XG_OFFS_USRH, 2, &data[0]);
//    gyroBiasStored[0] = ( (int16_t) (data[0] << 8) | data[1]);
// 
//    read_register(ICM20648_REG_YG_OFFS_USRH, 2, &data[0]);
//    gyroBiasStored[1] = ( (int16_t) (data[0] << 8) | data[1]);
// 
//    read_register(ICM20648_REG_ZG_OFFS_USRH, 2, &data[0]);
//    gyroBiasStored[2] = ( (int16_t) (data[0] << 8) | data[1]);
// 
//    /* The gyro bias should be stored in 1000dps full scaled format. We measured in 250dps to get */
//    /* the best sensitivity, so need to divide by 4 */
//    /* Substract from the stored calibration value */
//    gyroBiasStored[0] -= gyroBias[0] / 4;
//    gyroBiasStored[1] -= gyroBias[1] / 4;
//    gyroBiasStored[2] -= gyroBias[2] / 4;
// 
//    /* Split the values into two bytes */
//    data[0] = (gyroBiasStored[0] >> 8) & 0xFF;
//    data[1] = (gyroBiasStored[0]) & 0xFF;
//    data[2] = (gyroBiasStored[1] >> 8) & 0xFF;
//    data[3] = (gyroBiasStored[1]) & 0xFF;
//    data[4] = (gyroBiasStored[2] >> 8) & 0xFF;
//    data[5] = (gyroBiasStored[2]) & 0xFF;
// 
//    /* Write the  gyro bias values to the chip */
//    write_register(ICM20648_REG_XG_OFFS_USRH, data[0]);
//    write_register(ICM20648_REG_XG_OFFS_USRL, data[1]);
//    write_register(ICM20648_REG_YG_OFFS_USRH, data[2]);
//    write_register(ICM20648_REG_YG_OFFS_USRL, data[3]);
//    write_register(ICM20648_REG_ZG_OFFS_USRH, data[4]);
//    write_register(ICM20648_REG_ZG_OFFS_USRL, data[5]);
// 
//    /* Turn off FIFO */
//    write_register(ICM20648_REG_USER_CTRL, 0x00);
// 
//    /* Disable all sensors */
//    enable_sensor(false, false, false);
// 
//    return ICM20648_OK;
//}
 
/***************************************************************************//**
 * @brief
 *    Reads the temperature sensor raw value and converts to Celsius.
 *
 * @param[out] temperature
 *    The mesured temperature in Celsius
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
void ICM20948ReadTemperature(float *temperature, unsigned char csID)
{
    uint8_t data[2];
    int16_t raw_temp;
 
    /* Read temperature registers */
    ICM20948SPIWriteSingleMultipleRead(TEMP_OUT_H, data, 2, csID);
 
    /* Convert to int16 */
    raw_temp = (int16_t) ( (data[0] << 8) + data[1]);
 
    /* Calculate the Celsius value from the raw reading */
    *temperature = ( (float) raw_temp / 333.87) + 21.0;
}


//=====================MAGNETOMETER FUNCTIONS=================================//
void ICM20948WriteToAK09916(unsigned char reg,unsigned char value, unsigned char csID)
{
    ICM20948WriteRegister(REG_BANK_SEL,BANK_3,csID);
    wait_us(30);
    /* Configure SLAVE0 ADDR For Write */
    ICM20948WriteRegisterDelay(I2C_SLV0_ADDR, /*AK09916_WRITE_MASK(*/ICM20948_WRITE_MASK(AK09916_ADDRESS),csID);
    
    wait_us(30);
    /* Configure SLV0_REG to CONFIG2 registers address */
    ICM20948WriteRegisterDelay(I2C_SLV0_REG, reg,csID);
    
    wait_us(30);
    /* Configure SLV0_DO with content value of AK09916 CONTROL 2  */
    ICM20948WriteRegisterDelay(I2C_SLV0_DO, value,csID);
    
    wait_us(30);
    /* Configure SLV0_CTRL  */
    I2C_SLVx_CTRLbits slvCtrl;
    slvCtrl.I2C_SLVx_EN=1;      //1 ? Enable reading data from this slave at the sample rate and storing data at the first available EXT_SENS_DATA register, which is always EXT_SENS_DATA_00 for I2C slave 0.
                                //0 ? Function is disabled for this slave.
    slvCtrl.I2C_SLVx_LENG=1;    //Number of bytes to be read from I2C slave 0.
    ICM20948WriteRegisterDelay(I2C_SLV0_CTRL, slvCtrl.ALL,csID);
    
    wait_us(30);
    /* Configure SLAVE0 ADDR For Read */
    ICM20948WriteRegisterDelay(I2C_SLV0_ADDR, /*AK09916_WRITE_MASK(*/ICM20948_READ_MASK(AK09916_ADDRESS),csID);
    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}
unsigned char valAK;
void ICM20948ReadFromAK09916(unsigned char reg,unsigned char * buf,unsigned char count, unsigned char csID)
{
    ICM20948WriteRegister(REG_BANK_SEL,BANK_3,csID);
    wait_us(30);
    /* Configure SLAVE0 ADDR For READ */
    ICM20948WriteRegisterDelay(I2C_SLV0_ADDR, /*AK09916_WRITE_MASK(*/ICM20948_READ_MASK(AK09916_ADDRESS),csID);
    
    ICM20948WriteRegisterDelay(REG_BANK_SEL,BANK_0,csID);
    wait_us(30);
    valAK=ICM20948ReadRegisterDelay(INT_STATUS,csID);
    
    ICM20948WriteRegisterDelay(REG_BANK_SEL,BANK_3,csID);
    
    /* Configure SLV0_REG to reg registers address */
    ICM20948WriteRegisterDelay(I2C_SLV0_REG, reg,csID);
    
    wait_us(30);
    /* Configure SLV0_CTRL  */
    I2C_SLVx_CTRLbits slvCtrl;
    slvCtrl.I2C_SLVx_EN=1;      //1 ? Enable reading data from this slave at the sample rate and storing data at the first available EXT_SENS_DATA register, which is always EXT_SENS_DATA_00 for I2C slave 0.
                                //0 ? Function is disabled for this slave.
    slvCtrl.I2C_SLVx_LENG=1;    //Number of bytes to be read from I2C slave 0.
    ICM20948WriteRegisterDelay(I2C_SLV0_CTRL, slvCtrl.ALL,csID);
    
    ICM20948WriteRegisterDelay(REG_BANK_SEL,BANK_0,csID);
//    do{
//        wait_us(30);
//        valAK=ICM20948ReadRegister(INT_STATUS,csID);
//        if(valAK & 0x01==0)
//            break;
//    }while(1);
    
    valAK=ICM20948ReadRegisterDelay(EXT_SLV_SENS_DATA_00,csID);
    wait_us(30);
    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}

void ICM20948ConfigureMstI2CBusForAK09916(unsigned char csID)
{ 
    ICM20948WriteRegister(REG_BANK_SEL,BANK_3,csID);
    
    wait_us(30);
    /* Configure I2C MST ODR registers */
    ICM20948WriteRegisterDelay(I2C_MST_ODR_CONFIG, 0x00,csID);   //ODR configuration for external sensor when gyroscope and accelerometer are disabled. ODR is computed as follows:
                                                            //1.1 kHz/(2^((odr_config[3:0])) )
                                                            //When gyroscope is enabled, all sensors (including I2C_MASTER) use the gyroscope ODR. 
                                                            //If gyroscope is disabled, then all sensors (including I2C_MASTER) use the accelerometer ODR.
    wait_us(30);
    /* Configure I2C MST CTRL registers */
    I2C_MST_CTRLbits mstCtrl;
    mstCtrl.I2C_MST_CLK=7;
    mstCtrl.MULT_MST_EN=0;          //Disable Multi master
    mstCtrl.I2C_MST_P_NSR=0;        //This bit controls the I2C Master?s transition from one slave read to the next slave read.
                                    //0 - There is a restart between reads.
                                    //1 - There is a stop between reads.
    ICM20948WriteRegisterDelay(I2C_MST_ODR_CONFIG, mstCtrl.ALL,csID);
    
    wait_us(30);
    /* Configure I2C MST DELAY_CTRL registers */
    I2C_MST_DELAY_CTRLbits mstDlyCtrl;
    mstDlyCtrl.ALL=0;
    ICM20948WriteRegisterDelay(I2C_MST_DELAY_CTRL, mstDlyCtrl.ALL,csID);
    
    ICM20948WriteRegisterDelay(REG_BANK_SEL,BANK_0,csID);
    
    wait_us(30);
    /* Configure USER_CTRL register */
    USER_CTRLbits userCtrl;
    userCtrl.ALL=ICM20948ReadRegister(USER_CTRL,csID);
    userCtrl.I2C_MST_EN=1;                      //1 ? Enable the I2C Master I/F module; pins ES_DA and ES_SCL are isolated from pins SDA/SDI and SCL/ SCLK.
    ICM20948WriteRegisterDelay(USER_CTRL,userCtrl.ALL,csID);
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}

void ICM20948ConfigureAK09916(unsigned char csID)
{
    
    ICM20948WriteToAK09916(AK09916_CONTROL_2,AK09916_ODR_100HZ, csID);
    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_3,csID);
    
    wait_us(30);
    /* Configure SLV0_REG to AK09916_OUT_X_L registers address */
    ICM20948WriteRegisterDelay(I2C_SLV0_REG, /*AK09916_OUT_X_L*/ AK09916_STATUS_1,csID);
    
    wait_us(30);
    /* Configure SLV0_CTRL  */
    I2C_SLVx_CTRLbits slvCtrl;
    slvCtrl.ALL=0;
    slvCtrl.I2C_SLVx_EN=1;      //1 ? Enable reading data from this slave at the sample rate and storing data at the first available EXT_SENS_DATA register, which is always EXT_SENS_DATA_00 for I2C slave 0.
                                //0 ? Function is disabled for this slave.
    slvCtrl.I2C_SLVx_LENG=8;    //Number of bytes to be read from I2C slave 0.
    ICM20948WriteRegisterDelay(I2C_SLV0_CTRL, slvCtrl.ALL,csID);
    
    wait_us(30);
    /* Configure SLAVE0 ADDR For Read */
    ICM20948WriteRegisterDelay(I2C_SLV0_ADDR, /*AK09916_WRITE_MASK(*/ICM20948_READ_MASK(AK09916_ADDRESS),csID);
    
    
    ICM20948WriteRegister(REG_BANK_SEL,BANK_0,csID);
}

unsigned char* ICM20948GetMagValues(unsigned char csID)
{
    static unsigned char dataRead[6];
    ICM20948SPIWriteSingleMultipleRead(EXT_SLV_SENS_DATA_00, dataRead, 6, csID);
    return dataRead;
}