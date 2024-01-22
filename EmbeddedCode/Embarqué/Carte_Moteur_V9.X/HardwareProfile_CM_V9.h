
/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File: HardwareProfile_CM_V7.h  
 * Author: Valentin BARCHASZ
 * Comments:
 * Revision history: 1.0
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef HARDWAREPROFILE_CM_V6_H
#define	HARDWAREPROFILE_CM_V6_H
#include "UTLN_Typedefs.h"
#include <stdbool.h>
#include "ChipConfig.h"

#include <xc.h> // include processor files - each processor file is guarded.  

#define USE_CM_V10

//#define WORKAROUND_EUROBOT

#define UTLN_LED_COUNT 10
#define UTLN_BUTTON_COUNT 3
#define ICM20948_SPICS_ID 0
#ifdef USE_CM_V10
    #define CMD_DIODE_FORWARD_VOLTAGE 0.67
    #define ACS723_VOLTAGE_PER_A 0.2
    #define ACS723_CURRENT_OFFSET 0.2
#else
    #define CMD_DIODE_FORWARD_VOLTAGE 0
    #define ACS723_VOLTAGE_PER_A 0.05
#endif



//#define TEST_MOTOR
//#define DEBUG_INTERRUPT
#define USE_M3508
#define USE_CANBUS
#ifdef USE_M3508

#endif
//#define USE_DIR_MODE
/*******************************************************************************
 * MODULES TO USE
 ******************************************************************************/

//#define USE_ULTRA_HIGH_CLOCK                    //140MHz option
//#define USE_CRYSTAL_OSCILLATOR
//#define USE_UART1
//#define USE_UART1_CIRCULAR_BUFFER

#define USE_UART2
#define USE_UART2_CIRCULAR_BUFFER

#define USE_UART4
#define USE_UART4_CIRCULAR_BUFFER

#define USE_EXTERNAL_QEI                        //Active l'utilisation de QEI externes
//#define USE_INTERNAL_QEI                        //Active l'utilisation de QEI externes
#define USE_4WHEELS_HOLO
//#define USE_LED_BLINKING                        //Active l'utilisation des fonction de leds avancées
#define USE_OSAL                                //Active l'utilisation de l'OS temps reel
#define USE_RS232
//#define USE_BD63150                             //Utilise hacheur BD63150
#define USE_BD63150_5                             //Utilise hacheur BD63150 sur le hacheur 5
#define USE_BD63150_6                             //Utilise hacheur BD63150 sur le hacheur 6

//#define MOTOR_OFF                               //Desactive les moteurs
#define USE_ODOMETRIE                           //Permet d'activer l'utilisation de l'odometrie
#define USE_IMU
//#define USE_JACK
//#define USE_LCD_SSD1306
//#define USE_POLOLU_SERVO
#define USE_OUTPUTCOMPARE34
#define USE_USB
#define USE_USB_CIRCULAR_BUFFER



//CONTROL MACRO, AUTO SET
#if defined( USE_RS232)
    #define USE_UART3
    #define USE_UART3_CIRCULAR_BUFFER
#endif
#if defined(USE_LCD_SSD1306)
    #define USE_PERIPH_I2C1
    #define USE_I2C1
#endif
#ifdef USE_ODOMETRIE
    #ifdef USE_EXTERNAL_QEI
        #define USE_LS7366R
        #define USE_SPI1
    #endif
    #ifdef USE_INTERNAL_QEI
        #define USE_QEI1
        #define USE_QEI2
    #endif
#endif
#ifdef USE_IMU
    #define USE_ICM20948
    #define USE_SPI2
#endif

#if defined(USE_ULTRA_HIGH_CLOCK)
    #define FCY 70000000                //140MHz Clock (70MIPS)
#else
    #define FCY 60000000                //120MHz Clock (60MIPS)
#endif

#define SERVO_UP_US 1240
#define SERVO_DOWN_US 1690
/*******************************************************************************
 * LEDs DEFINITION
 ******************************************************************************/


#define _DEBUG _LED_B

#define TP_RK1 PORTKbits.RK1

/*******************************************************************************
 * DIGITAL INPUT DEFINITION (ToR)
 ******************************************************************************/
#define _JACK PORTJbits.RJ7
#define JACKPort PORTJ
#define JACKPin 7

#define _CONFIG1 PORTKbits.RK13
#define CONFIG1Port PORTK
#define CONFIG1Pin 13

#define _CONFIG2 PORTAbits.RA1
#define CONFIG2Port PORTA
#define CONFIG2Pin 1

#define _CONFIG3 PORTFbits.RF13
#define CONFIG3Port PORTF
#define CONFIG3Pin 13

#define _CONFIG4 PORTFbits.RF12
#define CONFIG4Port PORTF
#define CONFIG4Pin 12

//Definition des bouton poussoirs
#define SW1 0
#define _SW1 PORTHbits.RH0
#define SW1port PORTH
#define SW1pin 0
#define SW1_ACTIVE_STATE 1      //Bouton appuyï¿½ quand le niveau de la pin ==1
#define SW2 1
#define _SW2 PORTHbits.RH1
#define SW2port PORTH
#define SW2pin 1
#define SW2_ACTIVE_STATE 1      //Bouton appuyï¿½ quand le niveau de la pin ==1
#define SW3 2
#define _SW3 PORTHbits.RH2
#define SW3port PORTH
#define SW3pin 2
#define SW3_ACTIVE_STATE 1      //Bouton appuyï¿½ quand le niveau de la pin ==1

//Entrees INT des codeurs externes
#define _ENCODER8_INT PORTHbits.RH15
#define _ENCODER7_INT PORTHbits.RH14
#define _ENCODER6_INT PORTHbits.RH13
#define _ENCODER5_INT PORTHbits.RH12
#define _ENCODER4_INT PORTAbits.RA15
#define _ENCODER3_INT PORTAbits.RA14
#define _ENCODER2_INT PORTKbits.RK11
#define _ENCODER1_INT PORTKbits.RK12

//Entres INT autres
#define _SPI2_INT1 PORTDbits.RD7
#define _SPI2_INT2 PORTFbits.RF0
#define _SPI2_INT3 PORTDbits.RD3
#define _SPI2_INT4 PORTDbits.RD2
#define _SPI2_INT_IMU PORTDbits.RD11
#define _INT_ETHERNET PORTGbits.RG9


/*******************************************************************************
 * DIGITAL OUTPUT DEFINITION (ToR)
 ******************************************************************************/
#define ENABLE_PWM LATJbits.LATJ10

#define ENABLE_TIR LATJbits.LATJ12
#define TIR_1 LATJbits.LATJ13
#define TIR_2 LATJbits.LATJ14
#define TIR_3 LATJbits.LATJ15
#define TIR_4 LATKbits.LATK0


//Definition des chip select
#define SPI2_SS_IMU_PORT LATH
#define SPI2_SS_IMU_PIN 11
#define _SS_IMU LATHbits.LATH11
#define _INT_MPU PORTDbits.RD11
#define SPI2_SS1_PORT LATH
#define SPI2_SS1_PIN 4
#define _SPI2_SS1   LATHbits.LATH4
#define SPI2_SS2_PORT LATH
#define SPI2_SS2_PIN 5
#define _SPI2_SS2   LATHbits.LATH5
#define SPI2_SS3_PORT LATH
#define SPI2_SS3_PIN 6
#define _SPI2_SS3   LATHbits.LATH6
#define SPI2_SS4_PORT LATH
#define SPI2_SS4_PIN 7
#define _SPI2_SS4   LATHbits.LATH7

#define SPI3_SS_ETHERNET_PORT PORTD
#define SPI3_SS_ETHERNET_PIN 12
#define _SPI3_SS_ETHERNET LATDbits.LATD12

//Definitions des Leds
#define LED_VERTE_1 0
#define LED_VERTE_1_PORT LATH
#define LED_VERTE_1_PIN 10
#define _LED_V1 LATHbits.LATH10

//#define LED_ROUGE_1 1
//#define LED_ROUGE_1_PORT LATJ
//#define LED_ROUGE_1_PIN 11
#define _LED_R1 LATJbits.LATJ11
//#define LED_ORANGE_1 2
//#define LED_ORANGE_1_PORT LATJ
//#define LED_ORANGE_1_PIN 4
#define _LED_O1 LATJbits.LATJ4
//#define LED_BLEUE_1 3
//#define LED_BLEUE_1_PORT LATJ
//#define LED_BLEUE_1_PIN 5
#define _LED_BLE1 LATJbits.LATJ5

// Le problème est LA !!!!!!!!!!!!!!!!!!! mais pourquoi ???
//#define LED_BLANCHE_1 4
//#define LED_BLANCHE_1_PORT LATJ
//#define LED_BLANCHE_1_PIN 6
//#define _LED_B1 LATJbits.LATJ6
#define _LED_B1 LATKbits.LATK11

//#define LED_ROUGE_1 1
//#define LED_ROUGE_1_PORT LATG
//#define LED_ROUGE_1_PIN 9
//#define _LED_R1 LATGbits.LATG9
//#define LED_ORANGE_1 2
//#define LED_ORANGE_1_PORT LATG
//#define LED_ORANGE_1_PIN 9
//#define _LED_O1 LATGbits.LATG9
//#define LED_BLEUE_1 3
//#define LED_BLEUE_1_PORT LATG
//#define LED_BLEUE_1_PIN 9
//#define _LED_BLE1 LATGbits.LATG9
//#define LED_BLANCHE_1 4
//#define LED_BLANCHE_1_PORT LATG
//#define LED_BLANCHE_1_PIN 9
//#define _LED_B1 LATGbits.LATG9

#define LED_VERTE_2 5
#define LED_VERTE_2_PORT LATH
#define LED_VERTE_2_PIN 3
#define _LED_V2 LATHbits.LATH3
#define LED_ROUGE_2 6
#define LED_ROUGE_2_PORT LATA
#define LED_ROUGE_2_PIN 10
#define _LED_R2 LATAbits.LATA10
#define LED_ORANGE_2 7
#define LED_ORANGE_2_PORT LATK
#define LED_ORANGE_2_PIN 15
#define _LED_O2 LATKbits.LATK15
#define LED_BLEUE_2 8
#define LED_BLEUE_2_PORT LATA
#define LED_BLEUE_2_PIN 9
#define _LED_BLE2 LATAbits.LATA9
#define LED_BLANCHE_2 9
#define LED_BLANCHE_2_PORT LATA
#define LED_BLANCHE_2_PIN 0
#define _LED_B2 LATAbits.LATA0

#define LED_VERTE_3 13
#define LED_VERTE_3_PORT LATA
#define LED_VERTE_3_PIN 4
#define _LED_V3 LATAbits.LATA4
#define LED_ORANGE_3 12
#define LED_ORANGE_3_PORT LATH
#define LED_ORANGE_3_PIN 9
#define _LED_O3 LATHbits.LATH9
#define LED_BLEUE_3 11
#define LED_BLEUE_3_PORT LATH
#define LED_BLEUE_3_PIN 8
#define _LED_BLE3 LATHbits.LATH8
#define LED_ROUGE_3 10
#define LED_ROUGE_3_PORT LATK
#define LED_ROUGE_3_PIN 1
#define _LED_R3 LATKbits.LATK1
#define LED_BLANCHE_3 10
#define LED_BLANCHE_3_PORT LATK
#define LED_BLANCHE_3_PIN 14
#define _LED_B3 LATKbits.LATK14




/*******************************************************************************
 * Definitions relatives aux moteurs
 ******************************************************************************/

#define MOTOR1_ENLreg IOCON1bits.PENL
#define MOTOR1_ENHreg IOCON1bits.PENH
#define MOTOR1_PWMH LATEbits.LATE1
#define MOTOR1_PWML LATEbits.LATE0
#define MOTOR1_DUTY_CYCLEreg PDC1
#define MOTOR1_DUTY_CYCLE_SECONDARYreg SDC1

#define MOTOR2_ENLreg IOCON2bits.PENL
#define MOTOR2_ENHreg IOCON2bits.PENH
#define MOTOR2_PWMH LATEbits.LATE3
#define MOTOR2_PWML LATEbits.LATE2
#define MOTOR2_DUTY_CYCLEreg PDC2
#define MOTOR2_DUTY_CYCLE_SECONDARYreg SDC2


#define MOTOR3_ENLreg IOCON3bits.PENL
#define MOTOR3_ENHreg IOCON3bits.PENH
#define MOTOR3_PWMH LATEbits.LATE5
#define MOTOR3_PWML LATHbits.LATH9
#define MOTOR3_DUTY_CYCLEreg PDC3
#define MOTOR3_DUTY_CYCLE_SECONDARYreg SDC3


#define MOTOR4_ENLreg IOCON4bits.PENL
#define MOTOR4_ENHreg IOCON4bits.PENH
#define MOTOR4_PWMH LATEbits.LATE7
#define MOTOR4_PWML LATEbits.LATE6
#define MOTOR4_DUTY_CYCLEreg PDC4
#define MOTOR4_DUTY_CYCLE_SECONDARYreg SDC4


#define MOTOR5_ENLreg IOCON5bits.PENL
#define MOTOR5_ENHreg IOCON5bits.PENH
#define MOTOR5_PWMH LATCbits.LATC2
#define MOTOR5_PWML LATCbits.LATC1
#define MOTOR5_DUTY_CYCLEreg PDC5
#define MOTOR5_DUTY_CYCLE_SECONDARYreg SDC5


#define MOTOR6_ENLreg IOCON6bits.PENL
#define MOTOR6_ENHreg IOCON6bits.PENH
#define MOTOR6_PWMH LATCbits.LATC4
#define MOTOR6_PWML LATCbits.LATC3
#define MOTOR6_DUTY_CYCLEreg PDC6
#define MOTOR6_DUTY_CYCLE_SECONDARYreg SDC6


#define MOTOR7_ENLreg IOCON7bits.PENL
#define MOTOR7_ENHreg IOCON7bits.PENH
#define MOTOR7_PWMH LATJbits.LATJ9
#define MOTOR7_PWML LATJbits.LATJ8
#define MOTOR7_DUTY_CYCLEreg PDC7
#define MOTOR7_DUTY_CYCLE_SECONDARYreg SDC7

//Pas de PWM Natif sur MOTOR 8, utilisation des pwm Ouput/Compare
//#define MOTOR8_ENLreg IOCON8bits.PENL
//#define MOTOR8_ENHreg IOCON8bits.PENH
#define MOTOR8_PWMH LATGbits.LATG15
#define MOTOR8_PWML LATDbits.LATD1
//#define MOTOR8_DUTY_CYCLEreg PDC8

#define MOTOR_LEFT 0
#define MOTOR_RIGHT 6

/*******************************************************************************
 * Definitions des pin des chip select des encodeurs SPI
 ******************************************************************************/
#define SPI1_SS_ENC1 LATDbits.LATD6
#define SPI1_SS_ENC1port LATD
#define SPI1_SS_ENC1pin 6
#define SPI1_SS_ENC2 LATDbits.LATD5
#define SPI1_SS_ENC2port LATD
#define SPI1_SS_ENC2pin 5
#define SPI1_SS_ENC3 LATDbits.LATD4
#define SPI1_SS_ENC3port LATD
#define SPI1_SS_ENC3pin 4
#define SPI1_SS_ENC4 LATJbits.LATJ3
#define SPI1_SS_ENC4port LATJ
#define SPI1_SS_ENC4pin 3
#define SPI1_SS_ENC5 LATJbits.LATJ2
#define SPI1_SS_ENC5port LATJ
#define SPI1_SS_ENC5pin 2
#define SPI1_SS_ENC6 LATJbits.LATJ1
#define SPI1_SS_ENC6port LATJ
#define SPI1_SS_ENC6pin 1
#define SPI1_SS_ENC7 LATJbits.LATJ0
#define SPI1_SS_ENC7port LATJ
#define SPI1_SS_ENC7pin 0
#define SPI1_SS_ENC8 LATDbits.LATD13
#define SPI1_SS_ENC8port LATD
#define SPI1_SS_ENC8pin 13

/*******************************************************************************
 * Definitions des pins I2C
 ******************************************************************************/
#define _I2C_SCL LATDbits.LATD10
#define I2C_SCLtris TRISD
#define I2C_SCLpin 10
#define _I2C_SDA LATDbits.LATD9
#define I2C_SDAtris TRISD
#define I2C_SDApin 9

#define SET_DIGITAL_PIN(portModeAddress,pinNum) ((*portModeAddress)&=~(1U<<pinNum))
#define SET_ANALOG_PIN(portModeAddress,pinNum) ((*portModeAddress)|=(1U<<pinNum))
#define SET_PIN_INPUT(trisAddress,pinNum) ((*trisAddress)|=(1U<<pinNum))
#define SET_PIN_OUTPUT(trisAddress,pinNum) ((*trisAddress)&=~(1U<<pinNum))
#define CLEAR_PIN(latAddress,pinNum) ((*latAddress)&=~(1U<<pinNum))
#define SET_PIN(latAddress,pinNum) ((*latAddress)|=(1U<<pinNum))

// Definition for system clock
#define SYS_CLK_FrequencySystemGet()        FCY
// Definition for peripheral clock
#define SYS_CLK_FrequencyPeripheralGet()    SYS_CLK_FrequencySystemGet()
// Definition for instruction clock
#define SYS_CLK_FrequencyInstructionGet()   (SYS_CLK_FrequencySystemGet() / 2)

#define Reset() {__asm__ volatile ("reset");}

void SetCMV8IODirection(uint8_t pinNum, uint8_t direction);
void SetCMV8IOMode(uint8_t pinNum, uint8_t mode);
void SetCMV8IOValue(uint8_t pinNum, uint8_t value);
void InitIO(void);
void LockIO();
void UnlockIO();
void InitCN(void);
#endif	/* XC_HEADER_TEMPLATE_H */

