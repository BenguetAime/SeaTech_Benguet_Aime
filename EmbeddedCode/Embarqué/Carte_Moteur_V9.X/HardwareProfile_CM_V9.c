#include <stdbool.h>
#include "HardwareProfile_CM_V9.h"
#include "UTLN_Flag.h"


void InitIO(void)
{    
    /****************************************************************************************************/
    // Declaration des pin Analogiques
    /****************************************************************************************************/
    ANSELA=0;             //Desactivation de toutes entree analogique
    ANSELB=0;             //Desactivation de toutes entree analogique
    ANSELD=0;             //Desactivation de toutes entree analogique
    ANSELC=0;             //Desactivation de toutes entree analogique
    ANSELE=0;             //Desactivation de toutes entree analogique
    ANSELG=0;             //Desactivation de toutes entree analogique

    //Activation des pin analogiques
    ANSELBbits.ANSB0=1;                  //ANA_0-B0
    ANSELBbits.ANSB8=1;                 //ANA_8-B8
    ANSELBbits.ANSB9=1;                 //ANA_9-B9
    ANSELBbits.ANSB10=1;                //ANA_10-B10
    ANSELBbits.ANSB11=1;                //ANA_11-B11
    ANSELBbits.ANSB12=1;                //ANA_12-B12
    ANSELBbits.ANSB13=1;                //ANA_13-B13
    ANSELBbits.ANSB14=1;                //ANA_14-B14
    ANSELBbits.ANSB15=1;                //ANA_15-B15
    ANSELAbits.ANSA6=1;                 //ANA_22-A6
    ANSELAbits.ANSA7=1;                 //ANA_23-A7 - BATT_CMD_VOLTAGE
    ANSELBbits.ANSB7=1;                 //ANA_7-B7 - I_POWER_BATT
    ANSELBbits.ANSB6=1;                 //ANA_6-B6 - BATT_MOT_VOLTAGE
    ANSELBbits.ANSB1=1;                 //ANA_1-B1 - I_CMD_BATT
    
    
    ANSELEbits.ANSE8=1;                 //MOTOR 1 ISense
    ANSELEbits.ANSE9=1;                 //MOTOR 2 ISense
    ANSELBbits.ANSB5=1;                 //MOTOR 3 ISense
    ANSELBbits.ANSB4=1;                 //MOTOR 4 ISense
    ANSELBbits.ANSB3=1;                 //MOTOR 5 ISense
    ANSELBbits.ANSB2=1;                 //MOTOR 6 ISense 

    
    /****************************************************************************************************/
    // Declaration des sorties
    /****************************************************************************************************/
    TRISHbits.TRISH10=0;                //LED_VERTE_1
    TRISJbits.TRISJ11=0;                //LED_ROUGE_1
    TRISJbits.TRISJ10=0;                //ENABLE_PWM
    TRISJbits.TRISJ4=0;                 //LED_ORANGE_1
    TRISJbits.TRISJ5=0;                 //LED_BLEUE_1
    TRISJbits.TRISJ6=0;                 //LED_BLANCHE_1
    TRISHbits.TRISH3=0;                 //LED_VERTE_2
    TRISAbits.TRISA10=0;                //LED_ROUGE_2
    TRISKbits.TRISK15=0;                //LED_ORANGE_2
    TRISAbits.TRISA9=0;                 //LED_BLEUE_2
    TRISAbits.TRISA0=0;                 //LED_BLANCHE_2
    TRISAbits.TRISA4=0;                 //LED_VERTE_3
    TRISKbits.TRISK1=0;                 //LED_ROUGE_3
    TRISHbits.TRISH9=0;                 //LED_ORANGE_3
    TRISHbits.TRISH8=0;                 //LED_BLEUE_3
    TRISKbits.TRISK14=0;                //LED_BLANCHE_3
    
    //Declarations des pin relatives au hacheurs
    TRISEbits.TRISE1=0;                //MOTOR1_PWMH
    TRISEbits.TRISE0=0;                //MOTOR1_PWML
//    TRISAbits.TRISA4=0;                //MOTOR1_PWMH
//    TRISKbits.TRISK1=0;                //MOTOR2_INA
//    TRISHbits.TRISH8=0;                //MOTOR2_PWMH
//    TRISGbits.TRISG15=0;               //MOTOR3_INA
//    TRISHbits.TRISH9=0;                //MOTOR3_PWMH
//    TRISJbits.TRISJ10=0;               //MOTOR4_INA
//    TRISHbits.TRISH10=0;               //MOTOR4_PWMH
//    TRISCbits.TRISC1=0;                //MOTOR5_INA
//    TRISDbits.TRISD8=0;                //MOTOR5_PWMH
//    TRISJbits.TRISJ11=0;               //MOTOR6_INA
//    TRISCbits.TRISC13=0;               //MOTOR6_PWMH
//    TRISJbits.TRISJ8=0;                //MOTOR7_INA
//    TRISDbits.TRISD1=0;                //MOTOR7_PWMH
    
    TRISJbits.TRISJ10=0;               //ENABLE_PWM     
    TRISJbits.TRISJ12=0;               //ENABLE_TIR
    TRISJbits.TRISJ13=0;               //TIR_1
    TRISJbits.TRISJ14=0;               //TIR_2
    TRISJbits.TRISJ15=0;               //TIR_3
    TRISKbits.TRISK0=0;                //TIR_4
    
    TRISHbits.TRISH11=0;                //SPI2_SS_IMU
    
    //Declarations des pin relatives au chip select encodeurs
    TRISDbits.TRISD6=0;                 //SPI1_SS_ENC1
    TRISDbits.TRISD5=0;                 //SPI1_SS_ENC2
    TRISDbits.TRISD4=0;                 //SPI1_SS_ENC3
    TRISJbits.TRISJ3=0;                 //SPI1_SS_ENC4
    TRISJbits.TRISJ2=0;                 //SPI1_SS_ENC5
    TRISJbits.TRISJ1=0;                 //SPI1_SS_ENC6
    TRISJbits.TRISJ0=0;                 //SPI1_SS_ENC7
    TRISDbits.TRISD13=0;                //SPI1_SS_ENC8
    
    
    TRISHbits.TRISH4=0;                 //SPI2_SS1
    

    /****************************************************************************************************/
    // Declaration des entrées
    /****************************************************************************************************/
    TRISEbits.TRISE8=1;                 //MOTOR 1 ISense
    TRISEbits.TRISE9=1;                 //MOTOR 2 ISense
    TRISBbits.TRISB5=1;                 //MOTOR 3 ISense
    TRISBbits.TRISB4=1;                 //MOTOR 4 ISense
    TRISBbits.TRISB3=1;                 //MOTOR 5 ISense
    TRISBbits.TRISB2=1;                 //MOTOR 6 ISense
    TRISBbits.TRISB11=1;                //ANA11 B11 (Motor 7 ISense)
    
    
    TRISBbits.TRISB7=1;                 //BATT_MOT_CURRENT
    TRISBbits.TRISB1=1;                 //BATT_CMD_CURRENT
    TRISBbits.TRISB6=1;                 //BATT_MOT_VOLTAGE
    TRISAbits.TRISA7=1;                 //BATT_CMD_VOLTAGE
    TRISAbits.TRISA6=1;                 //ANA_22 A6 (Motor 8 ISense)
    
    TRISJbits.TRISJ7=1;                 //Jack
    TRISKbits.TRISK13=1;                //Config 1
    TRISAbits.TRISA1=1;                 //Config 2
    TRISFbits.TRISF13=1;                //Config 3 
    TRISFbits.TRISF12=1;                //Config 4
    TRISHbits.TRISH0=1;                 //SW1
    TRISHbits.TRISH1=1;                 //SW2
    TRISHbits.TRISH2=1;                 //SW3
    
    //INTERRUPT SPI
    TRISDbits.TRISD7=1;                 //SPI2_INT1
    TRISFbits.TRISF0=1;                 //SPI2_INT2
    TRISDbits.TRISD3=1;                 //SPI2_INT3
    TRISDbits.TRISD2=1;                 //SPI2_INT4
    TRISDbits.TRISD11=1;                //SPI2 INT IMU
    TRISAbits.TRISA2=1;                 //Uart2 RX
    //TRISAbits.TRISA4=1;                 //SPI1_CLK Input (Workaround SPI1_CLK)

    /****************************************************************************************************/
    // Gestion des pin remappables
    /****************************************************************************************************/
    UnlockIO(); // On unlock les registres d'entrées/sorties, ainsi que les registres des PPS

    //PPS OUTPUT
    #if defined(USE_UART1)
        RPOR4bits.RP79R=1;                 //UART1 TX sur RP79
    #elif defined(USE_OUTPUTCOMPARE34)
        RPOR4bits.RP79R=0b010011;           //Output Compare 4 sur RP79
    #endif
    RPOR8bits.RP98R=3;                 //UART2 TX sur RP98
    RPOR11bits.RP104R=27;              //UART3 TX sur RP104 (RS232 TX)
    RPOR9bits.RP100R=0b011101;         //UART4 TX sur RP100 (RF4)
    
    RPOR8bits.RP99R=6;                 //SPI1CLK sur RP99
    RPOR0bits.RP64R=5;                 //SPI1_MOSI sur RP64

//    RPOR9bits.RP101R=31;                //SPI3 MOSI sur RP101 (RF5) ETH Module
//    RPOR9bits.RP100R=9;                 //SPI3 CLK sur RP100 (RF4) ETH Module
    
    RPOR15bits.RP127R=0b010000;         //Output Compare 1 sur RP127
    RPOR0bits.RP65R=0b010001;         //Output Compare 2 sur RP65
    
    RPOR7bits.RP97R=0b001110;         //CAN1TX sur RP97 (EncoderB A)
    
    

    //PPS INPUT
    #if defined(USE_UART1)
        RPINR18bits.U1RXR=78;               //UART1 RX sur RPI78
    #elif defined(USE_OUTPUTCOMPARE34)
        
    #endif
    RPINR19bits.U2RXR=18;               //UART2 RX sur RPI18
    RPINR27bits.U3RXR=21;               //UART3 RX sur RPI21 (RS232 RX)
    RPINR28bits.U4RXR=72;               //UART4 RX sur RPI72 (RD8)
    
    RPINR20bits.SDI1R=19;               //SPI1_MISO sur RPI19
    //RPINR29bits.SDI3R=72;               //SPI3_MISO sur RPI72 (RC14) ETH Module
    RPINR14bits.QEA1R=126;              //QEI 1 A sur RP 126 (RG14)->MOTORA Encoder A
    RPINR14bits.QEB1R=124;              //QEI 1 B sur RPI 124 (RG12)->MOTORA Encoder B
    RPINR15bits.INDX1R=125;             //QEI 1 Index sur RP125 (RG13)->MOTORA Encoder Index
    //RPINR16bits.QEA2R=97;               //QEI 2 A sur RP 97 (RF1)->MOTORB Encoder Index
    //RPINR16bits.QEB2R=113;              //QEI 2 B sur RP113 (RG1)->MOTORB Encoder B
    RPINR17bits.INDX2R=112;             //QEI 2 Index sur RP112 (RG0)->MOTORB Encoder A
    
    RPINR26bits.C1RXR=113;              //CAN1 RX sur RPI113 (EncoderB B)
    

    LockIO(); // On lock les registres d'entrées/sorties, ainsi que les registres des PPS
    

/*******************************************************************************
 * Set des etats initiaux des pins (SS, Brake, ...)
 ******************************************************************************/
    
    SPI1_SS_ENC1=1;
    SPI1_SS_ENC2=1;
    SPI1_SS_ENC3=1;
    SPI1_SS_ENC4=1;
    SPI1_SS_ENC5=1;
    SPI1_SS_ENC6=1;
    SPI1_SS_ENC7=1;
    SPI1_SS_ENC8=1;
}

void LockIO() {
    asm volatile ("mov #OSCCON,w1 \n"
                "mov #0x46, w2 \n"
                "mov #0x57, w3 \n"
                "mov.b w2,[w1] \n"
                "mov.b w3,[w1] \n"
                "bset OSCCON, #6":: : "w1", "w2", "w3");
}

void UnlockIO() {
    asm volatile ("mov #OSCCON,w1 \n"
                "mov #0x46, w2 \n"
                "mov #0x57, w3 \n"
                "mov.b w2,[w1] \n"
                "mov.b w3,[w1] \n"
                "bclr OSCCON, #6":: : "w1", "w2", "w3");
}


void InitCN(void)
{
    CNENAbits.CNIEA15=1;        //CNEN Jack
//    CNENGbits.CNIEG1=1;
//    CNENFbits.CNIEF0=1;
//    CNENDbits.CNIED6=1;
//    CNENCbits.CNIEC8=1;
//    CNENDbits.CNIED12=1;
//    CNENCbits.CNIEC6=1;
//    CNENAbits.CNIEA14=1;
//    CNENCbits.CNIEC10=1;
    
    IEC1bits.CNIE=1;
}

void SetCMV8IODirection(uint8_t pinNum, uint8_t direction)
{
    if(pinNum==0)
        (direction)?SET_PIN_INPUT(&TRISB,pinNum):SET_PIN_OUTPUT(&TRISB,pinNum);
    else if(pinNum>7 && pinNum<16)
        (direction)?SET_PIN_INPUT(&TRISB,pinNum):SET_PIN_OUTPUT(&TRISB,pinNum);
    else if(pinNum==6)
        (direction)?SET_PIN_INPUT(&TRISA,pinNum):SET_PIN_OUTPUT(&TRISA,pinNum);
}

void SetCMV8IOMode(uint8_t pinNum, uint8_t mode)
{
    if(pinNum==0)
        (mode)?SET_ANALOG_PIN(&ANSELB,pinNum):SET_DIGITAL_PIN(&ANSELB,pinNum);
    else if(pinNum>7 && pinNum<16)
        (mode)?SET_ANALOG_PIN(&ANSELB,pinNum):SET_DIGITAL_PIN(&ANSELB,pinNum);
    else if(pinNum==6)
        (mode)?SET_ANALOG_PIN(&ANSELA,pinNum):SET_DIGITAL_PIN(&ANSELA,pinNum);
}

void SetCMV8IOValue(uint8_t pinNum, uint8_t value)
{
    if(pinNum==0)
        (value)?SET_PIN(&LATB,pinNum):CLEAR_PIN(&LATB,pinNum);
    else if(pinNum>7 && pinNum<16)
        (value)?SET_PIN(&LATB,pinNum):CLEAR_PIN(&LATB,pinNum);
    else if(pinNum==6)
        (value)?SET_PIN(&LATA,pinNum):CLEAR_PIN(&LATA,pinNum);
}

unsigned char oldSW1=1;
unsigned char oldJack=1;
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    IFS1bits.CNIF = 0;
    
    if(_JACK==0 && oldJack==1)
    {
        oldJack=0;
    }
    else if(_JACK==1 && oldJack==0)
    {
        oldJack=1;
    }
}