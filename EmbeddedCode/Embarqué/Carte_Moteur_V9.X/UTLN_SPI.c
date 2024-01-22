/******************************************************************************
  Filename:       UTLN_spi.c
  Revised:        $Date: 2017-08-22 18:52:21 $
  Revision:       $Revision: 00000 $

  Description:   UTLN Functions to drive the SPI Module (SPI Driver)

  Copyright 2017 Université du Sud Toulon Var. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer .You may not use this
  Software unless you agree to abide by the terms of the License. Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact DPT GEII at www.univ-tln.fr.

 *****************************************************************************/
/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "define.h"
#include "UTLN_SPI.h"
#include <Libpic30.h>

//#define USE_DMA_FOR_SPI

/*******************************************************************************
 * CONFIG DU SPI EN FONCTIONS DE LA PLATEFORME (MACRO)
 ******************************************************************************/

#define SPI_TIMEOUT 1000

/*******************************************************************************
 * VARIABLE
 ******************************************************************************/
#ifdef USE_SPI1
int antiBlockCounterSpi1;
#endif

#ifdef USE_SPI2
unsigned char spi2Crash=0;
unsigned int antiBlockCounterSpi2;
#endif
#ifdef USE_SPI4
int antiBlockCounterSpi4;
#endif


#ifdef USE_SPI1
/*******************************************************************************
 * FONCTIONS
 ******************************************************************************/
/*******************************************************************************
 * @fn      initSPI1()
 *
 * @brief   Fonction permettant l'initialisation du peripherique SPI.
 *
 * @param   None
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1Init(void)
{
    /* Holds the information about SPI configuartion */

    /* The following code shows the SPI register configuration for Master mode */

    IFS0bits.SPI1IF = 0;	// Clear the Interrupt Flag
    IEC0bits.SPI1IE = 0;	// Disable the Interrupt

    // SPI1CON1 Register Settings
    SPI1CON1bits.DISSCK = 0;// Internal Serial Clock is Enabled
    SPI1CON1bits.DISSDO = 0;// SDOx pin is controlled by the module
    SPI1CON1bits.MODE16 = 0;// 1 : Communication is word-wide (16 bits)
                                                    // 0 : Communication is char-wide (8 bits)
    SPI1CON1bits.SMP = 1;	// Input data is sampled at the middle of data output time
    SPI1CON1bits.CKE = 1;	// Serial output data changes on transition from active clock to idle clock state
    SPI1CON1bits.CKP = 0;	// Idle state for clock is a low level; active state is a high level
    SPI1CON1bits.MSTEN = 1;	// Master mode Enabled
    
    SPI1CON1bits.SPRE = 0b111;  // 0b111 : 1:1 <--
                                // 0b110 : 2:1
                                // 0b101 : 3:1
                                // 0b100 : 4:1 
                                // 0b011 : 5:1
                                // 0b010 : 6:1
                                // 0b001 : 7:1
                                // 0b000 : 8:1
    
    SPI1CON1bits.PPRE = 0b01;   // 0b11 : 1:1
                                // 0b10 : 4:1 
                                // 0b01 : 16:1 <--
                                // 0b00 : 64:1
    
    //Freq SPI2 : 60 MIPS / 4 / 4 = 3.75MHz

    // SPI1CON2 Register Settings
    SPI1CON2bits.FRMEN = 0;	// 0 : framing disabled
                                                    // 1 : framing enabled
    SPI1CON2bits.SPIFSD = 0;// 0 : Frame sync pulse output Master
                                                    // 1 : Frame sync pulse input Slave
    //SPI1CON2bits.SPIFPOL = 0;// 0 : sync pulse active low
                                                    // 1 : sync pulse active high
    //SPI1CON2bits = 0;// 1 : sync pulse coincides first bit
                                                    // 0 : sync pulse precedes first bit

    SPI1STATbits.SPIEN = 1;	// Enable SPI module


    #ifdef USE_DMA_FOR_SPI
        // Force First word after Enabling SPI
        DMA0REQbits.FORCE=1;
        while(DMA0REQbits.FORCE==1);
    #else
        SPI1BUF = 0x0000;       // Write data to be transmitted
        // Interrupt Controller Settings
        IPC2bits.SPI1IP=5;
        IFS0bits.SPI1IF = 0; 	// Clear the Interrupt Flag
        IEC0bits.SPI1IE = 1;	// Enable the Interrupt **
    #endif
 
}   

/*******************************************************************************
 * @fn      setSPI1_CKP_CKE()
 *
 * @brief   Fonction permettant de changer la configuration de l'horloge SPI a
 *          la volée.
 *
 * @param   ckp - 1 byte - Clock polarity
 *          cke - 1 byte - Clock edge
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1Set_CKP_CKE(int ckp,int cke)
{	
    SPI1STATbits.SPIEN = 0;	// Disable SPI module
    SPI1CON1bits.CKE = cke;
    SPI1CON1bits.CKP = ckp;
    SPI1STATbits.SPIEN = 1;	// Enable SPI module  
}

/*******************************************************************************
 * @fn      invertSPI1_CKP()
 *
 * @brief   Fonction permettant d'inverser la polarité de l'horloge a la volée.
 *          ( Idle State a l'etat haut, ou a l'etat bas)
 *
 * @param   None
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1Invert_CKP(void)
{	
    SPI1STATbits.SPIEN = 0;	// Disable SPI module
    SPI1CON1bits.CKP = !SPI1CON1bits.CKP;
    SPI1STATbits.SPIEN = 1;	// Enable SPI module    
}

/*******************************************************************************
 * @fn      setHighSpeedClk()
 *
 * @brief   Fonction permettant de changer la frequence de l'horloge en
 *          l'augmentant au maximum (4.096MHz) a la volée.
 *          Cette fonction permet de choisir une frequence en fonction du
 *          peripherique definit en parametre.
 *
 * @param   yn - 1 byte - Yes or No
 *          device - 1 byte - Le peripherique pour lequel on change la frequence.
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1SetHighSpeedClk(unsigned char yn,unsigned char device)
{
    if(yn)
    {
        SPI1STATbits.SPIEN = 0;	// Disable SPI module
//        //Reglage de l'horloge spi a 4.096Mhz
//        SPI1CON1bits.SPRE = 0b111;
//        SPI1CON1bits.PPRE = 0b10;
        
        //Reglage de l'horloge spi a 60/8/4 = 1.875Mhz        
        SPI1CON1bits.SPRE = 0b100;  // 0b111 : 1:1 
                                    // 0b110 : 2:1
                                    // 0b101 : 3:1
                                    // 0b100 : 4:1 <--
                                    // 0b011 : 5:1
                                    // 0b010 : 6:1
                                    // 0b001 : 7:1
                                    // 0b000 : 8:1
    
        SPI1CON1bits.PPRE = 0b10;   // 0b11 : 1:1
                                    // 0b10 : 4:1 <--
                                    // 0b01 : 16:1 
                                    // 0b00 : 64:1
    
        SPI1STATbits.SPIEN = 1;	// Enable SPI module
    }
    else
    {
        SPI1STATbits.SPIEN = 0;	// Disable SPI module
//        //Reglage de l'horloge spi a 2Mhz
//        SPI1CON1bits.SPRE = 0b110;
//        SPI1CON1bits.PPRE = 0b01;
        
        //Reglage de l'horloge spi a 60/4/8 = 1.85Mhz        
        SPI1CON1bits.SPRE = 0b000;  // 0b111 : 1:1 
                                    // 0b110 : 2:1
                                    // 0b101 : 3:1
                                    // 0b100 : 4:1 
                                    // 0b011 : 5:1
                                    // 0b010 : 6:1
                                    // 0b001 : 7:1
                                    // 0b000 : 8:1 <--
    
        SPI1CON1bits.PPRE = 0b10;   // 0b11 : 1:1
                                    // 0b10 : 4:1 <--
                                    // 0b01 : 16:1 
                                    // 0b00 : 64:1
        SPI1STATbits.SPIEN = 1;	// Enable SPI module
    }
}

/*******************************************************************************
 * @fn      invertSPI1_CKE()
 *
 * @brief   Fonction permettant d'inverser la polarité de l'horloge a la volée.
 *          (Actif sur front montant ou sur front descendant)
 *
 * @param   None
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1Invert_CKE(void)
{	
    SPI1STATbits.SPIEN = 0;	// Disable SPI module
    SPI1CON1bits.CKE = !SPI1CON1bits.CKE;
    SPI1STATbits.SPIEN = 1;	// Enable SPI module  
}


/*******************************************************************************
 * @fn      ChipSelectSpi1SetValue()
 *
 * @brief   Fonction utilisée dans la Fonction de l'ecture/ecriture sur le port.
 *          Elle permet de choisir un chip select (donc un peripherique), afin
 *          d'etablir une communication.
 *          Set ChipSelect (Platform dependent)
 *          Neccessite le fichier define.h
 *
 * @param   value - 1 byte - La valeur a atribuer au ChipSelect (0 ou 1).
 *          device - 1 byte - Le peripherique auquel l'on veut communiquer.
 *
 * @return  None
 *
 ******************************************************************************/
SPI_SS_CONFIG ss1Config[MAX_SPI1_SS];
void SPI1ChipSelectSetValue( unsigned int value, unsigned int device)
{
    if(device<MAX_SPI1_SS)
    {
        if(value)
            *ss1Config[device].port|= (1U<<ss1Config[device].pinNumber);
        else
            *ss1Config[device].port&= ~(1U<<ss1Config[device].pinNumber);
    }
}

void SPI1RegisterChipSelect(volatile unsigned int* pPort, uint8 pinNum, uint8 device)
{
    if(device<MAX_SPI1_SS)
    {
        ss1Config[device].port=pPort;
        ss1Config[device].pinNumber=pinNum;
    }
}
/*******************************************************************************
 * @fn      WriteSingleCommandSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un octet sur le bus SPI.
 *          (Platform dependent).
 *
 * @param   command - 1 byte - Octet a envoyer sur le bus.
 *          device - 1 byte - Peripherique auquel on veut envoyer l'octet.
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1WriteSingleCommand( unsigned char command, unsigned char device)
{	   
    unsigned char commands[1];
    unsigned char results[1];
    commands[0]=command;
    SPI1WriteMultipleCommandMultipleRead(commands, 1, results, 0, device, 0);
}


/*******************************************************************************
 * @fn      WriteSingleCommandPlusNOPSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un octet sur le bus SPI, suivi d'un
 *          dummy byte (0x00).
 *          (Platform dependent).
 *
 * @param   command - 1 byte - Octet a envoyer sur le bus.
 *          device - 1 byte - Peripherique auquel on veut envoyer l'octet.
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1WriteSingleCommandPlusNOP( unsigned char command, unsigned char device )
{	    
    unsigned char commands[2];
    unsigned char results[2];
    commands[0]=command;
    commands[1]=0x00;
    SPI1WriteMultipleCommandMultipleRead(commands, 2, results, 0, device, 0);
}

/*******************************************************************************
 * @fn      WriteMultipleCommandSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un nombre N d'octets sur le bus SPI.
 *          (Platform dependent).
 *
 * @param   commands - 1-256 bytes - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte - Nombre d'octet a envoyer.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1WriteMultipleCommand( unsigned char commands[], int nbCommands, unsigned char device )
{	
    unsigned char results[1];
    SPI1WriteMultipleCommandMultipleRead(commands, nbCommands, results, 0, device, 0);
}


/*******************************************************************************
 * @fn      WriteSingleCommandMultipleReadSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un octet sur le bus SPI, puis de venir
 *          lire un nombre N d'octets.
 *          (Platform dependent).
 *
 * @param   command - 1 byte - Octet a envoyer sur le bus.
 *          results - 1-256 byte - Tableau ou seront stockés les octets lus.
 *          nbResults - 1 byte - Nombre d'octet a lire.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *          invertReadCKE - 1 byte - Si oui ou non on inverse la polarité de
 *                                   l'horloge lors de la lecture.
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1WriteSingleCommandMultipleRead(unsigned char command, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE )
{	    
    unsigned char commands[1];
    commands[0]=command;
    SPI1WriteMultipleCommandMultipleRead(commands, 1, results, nbResults, device, invertReadCKE);
}


/*******************************************************************************
 * @fn      WriteMultipleCommandMultipleReadSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un ou plusieurs octets sur le bus SPI,
 *          puis de venir lire un nombre N d'octets.
 *          Inversion possible de la polarité de CKE entre le write et le read.
 *          (Platform dependent).
 *
 * @param   commands - 1-256 byte - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte- Nombre d'octets a transmettre sur le BUS.
 *          results - 1-256 byte - Tableau ou seront stockés les octets lus.
 *          nbResults - 1 byte - Nombre d'octet a lire.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *          invertReadCKE - 1 byte - Si oui ou non on inverse la polarité de
 *                                   l'horloge lors de la lecture.
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1WriteMultipleCommandMultipleRead(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE )
{		
    int i =0;
    
    //On lit SPI2BUF au cas où une lecture n'ait pas été faite avant...
    //Très important, sinon le dernier octet est coupé
    volatile unsigned char dummy;
    if(SPI1STATbits.SPIRBF)
        dummy = SPI1BUF;   

    //On passe l'horloge spi a 4.096MHz (en fonction du perif)
    SPI1SetHighSpeedClk(0,device);
    //on déclenche le SS
    SPI1ChipSelectSetValue(0, device);
    __delay32(10);
    //On envoie les commandes
    for (i=0; i<nbCommands; i++)
    {
            SPI1BUF=(unsigned int)commands[i];

            //L'anti bloqueur est destiné à sortir d'un figeage SPI
            //Celui-ci se produit quand on reçoit des infos sur l'UART
            antiBlockCounterSpi1 = SPI_TIMEOUT;
            while ( !SPI1STATbits.SPIRBF  && antiBlockCounterSpi1>0)
            {  antiBlockCounterSpi1--; }
            if(antiBlockCounterSpi1 <= 0)
            {   }
            dummy = SPI1BUF;
    }

    //on permute la valeur de CKE si le flag d'inversion de CKE est active
    if(invertReadCKE)
    SPI1Invert_CKE();

    for (i=0; i<nbResults; i++)
    {
            //On envoie un signal nul
            SPI1BUF=0x0000;
            //Le passage à 1 de RBF indique que les données reçues sont dispo dans le buffer, on attend qu'il passe à 1
            //L'anti bloqueur est destiné à sortir d'un figeage SPI
            //Celui-ci se produit quand on reçoit des infos sur l'UART
            antiBlockCounterSpi1 = SPI_TIMEOUT;
            while ( !SPI1STATbits.SPIRBF  && antiBlockCounterSpi1>0)
            {  antiBlockCounterSpi1--; }
            if(antiBlockCounterSpi1 <= 0)
            { ; }
            results[i] = SPI1BUF;
    }

    //on permute la valeur de CKE si le flag d'inversion de CKE est active
    if(invertReadCKE)
    SPI1Invert_CKE();

    //on termine le SS
    SPI1ChipSelectSetValue(1, device);
    //On remet l'horloge spi a 1MHz
    SPI1SetHighSpeedClk(0,device);
}

/*******************************************************************************
 * @fn      WriteMultipleCommandMultipleReadSpi1Delay()
 *
 * @brief   Fonction permettant l'envoi d'un ou plusieurs octets sur le bus SPI,
 *          puis de venir lire un nombre N d'octets.
 *          Inversion possible de la polarité de CKE entre le write et le read.
 *          (Platform dependent).
 *
 * @param   commands - 1-256 byte - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte- Nombre d'octets a transmettre sur le BUS.
 *          results - 1-256 byte - Tableau ou seront stockés les octets lus.
 *          nbResults - 1 byte - Nombre d'octet a lire.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *          invertReadCKE - 1 byte - Si oui ou non on inverse la polarité de
 *                                   l'horloge lors de la lecture.
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1WriteMultipleCommandMultipleReadDelay(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE )
{
    int i =0;
 
    //On passe l'horloge spi a 4.096MHz (en fonction du perif)
    SPI1SetHighSpeedClk(1,device);
    //on déclenche le SS
//    static int delay = TCLK_ADS129x;
//    __delay32(2*delay); //Il faut attendre 2 CLK, dans le ADS129x
    SPI1ChipSelectSetValue(0, device);
    //On envoie les commandes
    volatile unsigned char dummy;
    for (i=0; i<nbCommands; i++)
    {
            SPI1BUF=(unsigned int)commands[i];

            //L'anti bloqueur est destiné à sortir d'un figeage SPI
            //Celui-ci se produit quand on reçoit des infos sur l'UART
            antiBlockCounterSpi1 = SPI_TIMEOUT;
            while ( !SPI1STATbits.SPIRBF  && antiBlockCounterSpi1>0)
            {  antiBlockCounterSpi1--; }
            if(antiBlockCounterSpi1 <= 0)
            {   }

            dummy = SPI1BUF;
    }

    //on permute la valeur de CKE si le flag d'inversion de CKE est active
    if(invertReadCKE)
    SPI1Invert_CKE();

//    __delay32(4*TCLK_ADS129x); //Il faut attendre 4 CLK, le temps que l'ADS charge ses résultats

    for (i=0; i<nbResults; i++)
    {
            //On envoie un signal nul
            SPI1BUF=0x0000;
            //Le passage a 0 de TBF indique la fin de transmission, on attend qu'il revienne à 0
            //Le passage à 1 de RBF indique que les données reçues sont dispo dans le buffer, on attend qu'il passe à 1

            //L'anti bloqueur est destiné à sortir d'un figeage SPI
            //Celui-ci se produit quand on reçoit des infos sur l'UART
            antiBlockCounterSpi1 = SPI_TIMEOUT;
            while ( !SPI1STATbits.SPIRBF  && antiBlockCounterSpi1>0)
            {  antiBlockCounterSpi1--; }
            if(antiBlockCounterSpi1 <= 0)
            {   }

            results[i] = SPI1BUF;
    }

    //on permute la valeur de CKE si le flag d'inversion de CKE est active
    if(invertReadCKE)
    SPI1Invert_CKE();

    //on termine le SS
//    __delay32(3*TCLK_ADS129x); //Il faut attendre 3 CLK dans le ADS129x
    SPI1ChipSelectSetValue(1, device);
    //On remet l'horloge spi a 1MHz
    SPI1SetHighSpeedClk(0,device);

}

/*******************************************************************************
 * @fn      WriteSingleCommandSpi1Delay()
 *
 * @brief   Fonction permettant l'envoi d'un octet sur le bus SPI.
 *          (Platform dependent).
 *          Ajoute un delay de 4us avant la remonté du chip select.(ads1292)
 *
 * @param   command - 1 byte - Octet a envoyer sur le bus.
 *          device - 1 byte - Peripherique auquel on veut envoyer l'octet.
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1WriteSingleCommandDelay( unsigned char command, unsigned char device)
{
    unsigned char commands[1];
    unsigned char results[1];
    commands[0]=command;
    SPI1WriteMultipleCommandMultipleReadDelay(commands, 1, results, 0, device, 0);
}

/*******************************************************************************
 * @fn      WriteMultipleCommandSpi1Delay()
 *
 * @brief   Fonction permettant l'envoi d'un nombre N d'octets sur le bus SPI.
 *          (Platform dependent).
 *          Rajoute un delay de 4 coups d'horloge a la fin de la com (ads1292)
 *
 * @param   commands - 1-256 bytes - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte - Nombre d'octet a envoyer.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1WriteMultipleCommandDelay( unsigned char commands[], int nbCommands, unsigned char device )
{
    unsigned char results[1];
    SPI1WriteMultipleCommandMultipleReadDelay(commands, nbCommands, results, 0, device, 0);
}


/******************************************************************************
 * INTERRUPT SERVICE ROUTINE
 ******************************************************************************/
/* This is SPI interrupt routine */
void __attribute__((interrupt, no_auto_psv)) _SPI1Interrupt (void)
{
	IFS0bits.SPI1IF = CLEAR;
}

#endif //#ifdef USE_SPI1


#ifdef USE_SPI2
/*******************************************************************************
 * @fn      initSPI2()
 *
 * @brief   Fonction permettant l'initialisation du peripherique SPI2.
 *
 * @param   None
 *
 * @return  None
 *
 ******************************************************************************/
void SPI2Init(void)
{
    /* Holds the information about SPI2 configuartion */

    /* The following code shows the SPI2 register configuration for Master mode */

    IFS2bits.SPI2IF = CLEAR;	// Clear the Interrupt Flag
    IEC2bits.SPI2IE = OFF;	// Disable the Interrupt

    // SPI1CON1 Register Settings
    SPI2CON1bits.DISSCK = 0;    // Internal Serial Clock is Enabled
    SPI2CON1bits.DISSDO = 0;    // SDOx pin is controlled by the module
    SPI2CON1bits.MODE16 = 0;    // 1 : Communication is word-wide (16 bits)
                                // 0 : Communication is char-wide (8 bits)
    
    SPI2CON1bits.CKE = 0;       // Serial output data changes on transition from active clock to idle clock state
    SPI2CON1bits.CKP = 1;       // Idle state for clock is a low level; active state is a high level
    SPI2CON1bits.MSTEN = 1;     // Master mode Enabled
    SPI2CON1bits.SMP = 1;       // Input data is sampled at the middle of data output time
    SPI2CON1bits.SPRE = 0b100;  // 0b111 : 1:1 
                                // 0b110 : 2:1
                                // 0b101 : 3:1
                                // 0b100 : 4:1 <--
                                // 0b011 : 5:1
                                // 0b010 : 6:1
                                // 0b001 : 7:1
                                // 0b000 : 8:1
    
    SPI2CON1bits.PPRE = 0b10;   // 0b11 : 1:1
                                // 0b10 : 4:1 <--
                                // 0b01 : 16:1 
                                // 0b00 : 64:1
    
    //Freq SPI2 : 60 MIPS / 4 / 4 = 3.75MHz
    
    //SPI2SetBaseFreq();
    //SPI2STATbits.SPIEN = 0;	// Disable SPI module

    // SPI1CON2 Register Settings
    SPI2CON2bits.FRMEN = 0;	// 0 : framing disabled
                                                    // 1 : framing enabled
    SPI2CON2bits.SPIFSD = 0;// 0 : Frame sync pulse output Master
                                                    // 1 : Frame sync pulse input Slave
    SPI2CON2bits.FRMPOL = 0;// 0 : sync pulse active low
                                                    // 1 : sync pulse active high
    SPI2CON2bits.FRMPOL = 0;// 1 : sync pulse coincides first bit
                                                    // 0 : sync pulse precedes first bit

    SPI2STATbits.SPIEN = 1;	// Enable SPI module


    #ifdef USE_DMA_FOR_SPI
        // Force First word after Enabling SPI
        DMA0REQbits.FORCE=1;
        while(DMA0REQbits.FORCE==1);
    #else
        SPI2BUF = 0x0000;       // Write data to be transmitted
        while(SPI2STATbits.SPITBF)
            Nop();
        // Interrupt Controller Settings
        IFS2bits.SPI2IF = CLEAR; 	// Clear the Interrupt Flag
        IEC2bits.SPI2IE = OFF;	// Enable the Interrupt **
        //IPC8bits.SPI2IP = 7;
    #endif    
     flagSystem.Spi2Interrupted = FALSE;
}

/*******************************************************************************
 * @fn      setSPI2_CKP_CKE()
 *
 * @brief   Fonction permettant de changer la configuration de l'horloge SPI a
 *          la volée.
 *
 * @param   ckp - 1 byte - Clock polarity
 *          cke - 1 byte - Clock edge
 *
 * @return  None
 *
 ******************************************************************************/
void Spi2SetCkpCke(int ckp,int cke)
{
    SPI2STATbits.SPIEN = OFF;	// Disable SPI module
    SPI2CON1bits.CKE = cke;
    SPI2CON1bits.CKP = ckp;
    SPI2STATbits.SPIEN = ON;	// Enable SPI module
}

/*******************************************************************************
 * @fn      invertSPI1_CKP()
 *
 * @brief   Fonction permettant d'inverser la polarité de l'horloge a la volée.
 *          ( Idle State a l'etat haut, ou a l'etat bas)
 *
 * @param   None
 *
 * @return  None
 *
 ******************************************************************************/
void Spi2InvertCkp(void)
{
    SPI2STATbits.SPIEN = OFF;	// Disable SPI module
    SPI2CON1bits.CKP = !SPI2CON1bits.CKP;
    SPI2STATbits.SPIEN = ON;	// Enable SPI module
}

/*******************************************************************************
 * @fn      setHighSpeedClk()
 *
 * @brief   Fonction permettant de changer la frequence de l'horloge en
 *          l'augmentant au maximum (4.096MHz) a la volée.
 *          Cette fonction permet de choisir une frequence en fonction du
 *          peripherique definit en parametre.
 *
 * @param   yn - 1 byte - Yes or No
 *          device - 1 byte - Le peripherique pour lequel on change la frequence.
 *
 * @return  None
 *
 ******************************************************************************/
//void Spi2SetHighSpeedClk(unsigned char yn,unsigned char device)
//{
//    /*if(((device == SRAM) || (device == GYRO)) && (device != OLED) && (device !=ECG))
//    {
//        if(yn)
//        {
//            SPI2STATbits.SPIEN = 0;	// Disable SPI module
//            //Reglage de l'horloge spi a 4.096Mhz
//            SPI2CON1bits.SPRE = 0b111;
//            SPI2CON1bits.PPRE = 0b10;
//            SPI2STATbits.SPIEN = 1;	// Enable SPI module
//        }
//        else
//        {
//            SPI2STATbits.SPIEN = 0;	// Disable SPI module
//            //Reglage de l'horloge spi a 1Mhz
//            SPI2CON1bits.SPRE = 0b111;
//            SPI2CON1bits.PPRE = 0b01;
//            SPI2STATbits.SPIEN = 1;	// Enable SPI module
//        }
//    }*/
//}

/*******************************************************************************
 * @fn      invertSPI2_CKE()
 *
 * @brief   Fonction permettant d'inverser la polarité de l'horloge a la volée.
 *          (Actif sur front montant ou sur front descendant)
 *
 * @param   None
 *
 * @return  None
 *
 ******************************************************************************/
void Spi2InvertCke(void)
{
    SPI2STATbits.SPIEN = OFF;	// Disable SPI module
    SPI2CON1bits.CKE = !SPI2CON1bits.CKE;
    SPI2STATbits.SPIEN = ON;	// Enable SPI module
}


/*******************************************************************************
 * @fn      ChipSelectSpi2SetValue()
 *
 * @brief   Fonction utilisée dans la Fonction de l'ecture/ecriture sur le port.
 *          Elle permet de choisir un chip select (donc un peripherique), afin
 *          d'etablir une communication.
 *          Set ChipSelect (Platform dependent)
 *          Neccessite le fichier define.h
 *
 * @param   value - 1 byte - La valeur a atribuer au ChipSelect (0 ou 1).
 *          device - 1 byte - Le peripherique auquel l'on veut communiquer.
 *
 * @return  None
 *
 ******************************************************************************/
SPI_SS_CONFIG ss2Config[MAX_SPI2_SS];
void SPI2ChipSelectSetValue( unsigned int value, unsigned int device)
{
    if(device<MAX_SPI2_SS)
    {
        if(value)
            *ss2Config[device].port|= (1U<<ss2Config[device].pinNumber);
        else
            *ss2Config[device].port&= ~(1U<<ss2Config[device].pinNumber);
    }
}

void SPI2RegisterChipSelect(volatile unsigned int* pPort, uint8 pinNum, uint8 device)
{
    if(device<MAX_SPI2_SS)
    {
        ss2Config[device].port=pPort;
        ss2Config[device].pinNumber=pinNum;
    }
}
/*******************************************************************************
 * @fn      WriteSingleCommandSpi2()
 *
 * @brief   Fonction permettant l'envoi d'un octet sur le bus SPI.
 *          (Platform dependent).
 *
 * @param   command - 1 byte - Octet a envoyer sur le bus.
 *          device - 1 byte - Peripherique auquel on veut envoyer l'octet.
 *
 * @return  None
 *
 ******************************************************************************/
void WriteSingleCommandSpi2( unsigned char command, unsigned char device)
{
    unsigned char commands[1];
    unsigned char results[1];
    commands[0]=command;
    SPI2WriteMultipleCommandMultipleRead(commands, 1, results, 0, device);
}





/*******************************************************************************
 * @fn      WriteMultipleCommandSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un nombre N d'octets sur le bus SPI.
 *          (Platform dependent).
 *
 * @param   commands - 1-256 bytes - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte - Nombre d'octet a envoyer.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *
 * @return  None
 *
 ******************************************************************************/
void WriteMultipleCommandSpi2( unsigned char commands[], int nbCommands, unsigned char device )
{
    unsigned char results[1];
    SPI2WriteMultipleCommandMultipleRead(commands, nbCommands, results, 0, device);
}


/*******************************************************************************
 * @fn      WriteSingleCommandMultipleReadSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un octet sur le bus SPI, puis de venir
 *          lire un nombre N d'octets.
 *          (Platform dependent).
 *
 * @param   command - 1 byte - Octet a envoyer sur le bus.
 *          results - 1-256 byte - Tableau ou seront stockés les octets lus.
 *          nbResults - 1 byte - Nombre d'octet a lire.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *          invertReadCKE - 1 byte - Si oui ou non on inverse la polarité de
 *                                   l'horloge lors de la lecture.
 *
 * @return  None
 *
 ******************************************************************************/
void WriteSingleCommandMultipleReadSpi2(unsigned char command, unsigned char results[], int nbResults, unsigned char device )
{
    unsigned char commands[1];
    commands[0]=command;
    SPI2WriteMultipleCommandMultipleRead(commands, 1, results, nbResults, device);
}


/*******************************************************************************
 * @fn      WriteMultipleCommandMultipleReadSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un ou plusieurs octets sur le bus SPI,
 *          puis de venir lire un nombre N d'octets.
 *          Inversion possible de la polarité de CKE entre le write et le read.
 *          (Platform dependent).
 *
 * @param   commands - 1-256 byte - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte- Nombre d'octets a transmettre sur le BUS.
 *          results - 1-256 byte - Tableau ou seront stockés les octets lus.
 *          nbResults - 1 byte - Nombre d'octet a lire.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *          invertReadCKE - 1 byte - Si oui ou non on inverse la polarité de
 *                                   l'horloge lors de la lecture.
 *
 * @return  None
 *
 ******************************************************************************/
void SPI2WriteMultipleCommandMultipleRead(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device )
{
        int i =0;
        volatile unsigned char dummy;
        
        //On lit SPI2BUF au cas où une lecture n'ait pas été faite avant...
        //Très important, sinon le dernier octet est coupé
        if(SPI2STATbits.SPIRBF)
            dummy = SPI2BUF;   

        //on déclenche le SS
        SPI2ChipSelectSetValue(LOW, device);
        //On envoie les commandes
        for (i=0; i<nbCommands; i++)
        {
            SPI2BUF=(unsigned int)commands[i];
            while(!SPI2STATbits.SPIRBF); //On attend que l'interrupt soit à 1 (fin de transmission/réception)
            dummy = SPI2BUF;   
        }
        
        for (i=0; i<nbResults; i++)
        {
            //On envoie un signal nul
            SPI2BUF=0x0000;
            while(!SPI2STATbits.SPIRBF);
            results[i] = SPI2BUF;  
        }
        //on termine le SS
        SPI2ChipSelectSetValue(HIGH, device);
}

/*******************************************************************************
 * @fn      WriteMultipleCommandMultipleReadSpi2Delay()
 *
 * @brief   Fonction permettant l'envoi d'un ou plusieurs octets sur le bus SPI,
 *          puis de venir lire un nombre N d'octets.
 *          Inversion possible de la polarité de CKE entre le write et le read.
 *          (Platform dependent).
 *          Ajout d'un delay avant la remontee du chip select : utile pour l'ads1292
 *
 * @param   commands - 1-256 byte - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte- Nombre d'octets a transmettre sur le BUS.
 *          results - 1-256 byte - Tableau ou seront stockés les octets lus.
 *          nbResults - 1 byte - Nombre d'octet a lire.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *          invertReadCKE - 1 byte - Si oui ou non on inverse la polarité de
 *                                   l'horloge lors de la lecture.
 *
 * @return  None
 *
 ******************************************************************************/
//void WriteMultipleCommandMultipleReadSpi2Delay(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device )
//{
//    if(IsSpi2Crashed()==FALSE)
//    {
//        int i =0;
//
//        //on déclenche le SS
//        __delay32(2*32); //Il faut attendre 2 CLK dans le ADS129x
//        SPI2ChipSelectSetValue(LOW, device);
//        //On envoie les commandes
//        volatile unsigned char dummy;
//        for (i=0; i<nbCommands; i++)
//        {
//            if(IsSpi2Crashed()==FALSE)
//            {
//                SPI2BUF=(unsigned int)commands[i];
//
//                //L'anti bloqueur est destiné à sortir d'un figeage SPI
//                //Celui-ci se produit quand on reçoit des infos sur l'UART
//                ResetSpi2AntiBlockCounter();
//                while(!SPI2STATbits.SPIRBF)
//                {
//                    Nop();
//                }
//                if((antiBlockCounterSpi2 >= SPI_TIMEOUT))// || ((device == FRAM) && (flagSystem.Spi2Interrupted == TRUE)))
//                {
//                    SetSpi2Crash();
//                }
//                dummy = SPI2BUF;
//            }
//        }
//
//        __delay32(4*32); //Il faut attendre 4 CLK dans le ADS129x
//
//        for (i=0; i<nbResults; i++)
//        {
//            if(IsSpi2Crashed()==FALSE)
//            {
//                //On envoie un signal nul
//                SPI2BUF=0x0000;
//                //Le passage à 1 de RBF indique que les données reçues sont dispo dans le buffer, on attend qu'il passe à 1
//
//                //L'anti bloqueur est destiné à sortir d'un figeage SPI
//                //Celui-ci se produit quand on reçoit des infos sur l'UART
//                ResetSpi2AntiBlockCounter();
//                while(!SPI2STATbits.SPIRBF)
//                {
//                    Nop();
//                }
//                if((antiBlockCounterSpi2 >= SPI_TIMEOUT))// || ((device == FRAM) && (flagSystem.Spi2Interrupted == TRUE)))
//                {
//                    SetSpi2Crash();
//                }
//                results[i] = SPI2BUF;
//            }
//        }
//
//
//        __delay32(3*32); //Il faut attendre 3 CLK dans le ADS129x
//
//        //on termine le SS
//        SPI2ChipSelectSetValue(HIGH, device);
//    }
//}



/*******************************************************************************
 * @fn      Gestion des crash
 *
 * @brief
 *
 * @param
 *
 * @return  None
 *
 ******************************************************************************/
void IncrementSpi2AntiBlockCounter(void)
{
    antiBlockCounterSpi2+=1;
}

void ResetSpi2AntiBlockCounter(void)
{
    antiBlockCounterSpi2=0;
}


void SetSpi2Crash(void)
{
    spi2Crash = 1;
}

void ResetSpi2Crash(void)
{
    spi2Crash = 0;
}

unsigned char IsSpi2Crashed(void)
{
    return spi2Crash;
}


/******************************************************************************
 * INTERRUPT SERVICE ROUTINE
 ******************************************************************************/
/* This is SPI2 interrupt routine */
void __attribute__((interrupt, no_auto_psv)) _SPI2Interrupt (void)
{
	IFS2bits.SPI2IF = CLEAR;
}

#endif //#ifdef USE_SPI2

#ifdef USE_SPI4
/*******************************************************************************
 * FONCTIONS
 ******************************************************************************/
/*******************************************************************************
 * @fn      initSPI1()
 *
 * @brief   Fonction permettant l'initialisation du peripherique SPI.
 *
 * @param   None
 *
 * @return  None
 *
 ******************************************************************************/
void InitSPI4(void)
{
    /* Holds the information about SPI configuartion */

    /* The following code shows the SPI register configuration for Master mode */

    IFS5bits.SPI4IF = 0;	// Clear the Interrupt Flag
    IEC5bits.SPI4IE = 0;	// Disable the Interrupt

    SPI4CON1Lbits.SPIEN = 0;	// Disable SPI module
    
    // SPI4CON1 Register Settings
    SPI4CON1Lbits.DISSCK = 0;// Internal Serial Clock is Enabled
    SPI4CON1Lbits.DISSDO = 0;// SDOx pin is controlled by the module
    SPI4CON1Lbits.MODE16 = 0;// 1 : Communication is word-wide (16 bits)
                                                    // 0 : Communication is char-wide (8 bits)
    SPI4CON1Lbits.SMP = 1;	// 0:Input data is sampled at the middle of data output time 1:Input data is sampled at the end of data output time
    SPI4CON1Lbits.CKE =0 ;	//0: Transmit happens on transition from Idle clock state to active clock state 1:Serial output data changes on transition from active clock to idle clock state
    SPI4CON1Lbits.CKP = 1;	//0: Idle state for clock is a low level--active state is a high level  1:Idle state for clock is a high level; active state is a low level
    SPI4CON1Lbits.MSTEN = 1;	// Master mode Enabled
    
    SPI4STATLbits.SPIROV=0;     //Clear SPIROV bit
    
    SPI4CON1Lbits.ENHBUF=0;     //Enhanced Buffer Mode
    
    SPI4CON1Hbits.IGNTUR=0;     //0 = A TUR is a critical error that stops SPI operation 1 = A Transmit Underrun (TUR) is NOT a critical error and data indicated by URDTEN is transmitted
                                //until the SPIxTXB is not empty
    SPI4CON1Hbits.IGNROV=0;     //0 = A ROV is a critical error that stops SPI operation 1 = A Receive Overflow (ROV) is NOT a critical error; during ROV, data in the FIFO is not overwritten
                                //by the receive data

    SPI4BRGL=5;     //

    // SPI1CON2 Register Settings
    SPI4CON2bits.FRMEN = 0;	// 0 : framing disabled
                                                    // 1 : framing enabled
    SPI4CON1Hbits.FRMSYNC = 0;// 0 : Frame sync pulse output Master
                                                    // 1 : Frame sync pulse input Slave
    SPI4CON1Hbits.FRMPOL = 0;// 0 : sync pulse active low
                                                    // 1 : sync pulse active high
    

    SPI4CON1bits.SPIEN = 1;	// Enable SPI module


    #ifdef USE_DMA_FOR_SPI
        // Force First word after Enabling SPI
        DMA0REQbits.FORCE=1;
        while(DMA0REQbits.FORCE==1);
    #else
        //SPI4BUFL = 0x0000;       // Write data to be transmitted
        // Interrupt Controller Settings
        IFS5bits.SPI4IF = 0; 	// Clear the Interrupt Flag
        IPC23bits.SPI4IP=4;
        IEC5bits.SPI4IE = 1;	// Enable the Interrupt **
    #endif

    #ifdef USE_MPU9250
        SS_MPU9250=1;
    #endif
    //#ifdef USE_MPU9250_2
        SS_MPU9250_2=1;
    //#endif
}   

/*******************************************************************************
 * @fn      setSPI4_CKP_CKE()
 *
 * @brief   Fonction permettant de changer la configuration de l'horloge SPI a
 *          la volée.
 *
 * @param   ckp - 1 byte - Clock polarity
 *          cke - 1 byte - Clock edge
 *
 * @return  None
 *
 ******************************************************************************/
void setSPI4_CKP_CKE(int ckp,int cke)
{	
    SPI4CON1bits.SPIEN = 0;	// Disable SPI module
    SPI4CON1bits.CKE = cke;
    SPI4CON1bits.CKP = ckp;
    SPI4CON1bits.SPIEN = 1;	// Enable SPI module  
}

/*******************************************************************************
 * @fn      invertSPI4_CKP()
 *
 * @brief   Fonction permettant d'inverser la polarité de l'horloge a la volée.
 *          ( Idle State a l'etat haut, ou a l'etat bas)
 *
 * @param   None
 *
 * @return  None
 *
 ******************************************************************************/
void invertSPI4_CKP(void)
{	
    SPI4CON1bits.SPIEN = 0;	// Disable SPI module
    SPI4CON1bits.CKP = !SPI4CON1bits.CKP;
    SPI4CON1bits.SPIEN = 1;	// Enable SPI module    
}

/*******************************************************************************
 * @fn      setHighSpeedClk()
 *
 * @brief   Fonction permettant de changer la frequence de l'horloge en
 *          l'augmentant au maximum (4.096MHz) a la volée.
 *          Cette fonction permet de choisir une frequence en fonction du
 *          peripherique definit en parametre.
 *
 * @param   yn - 1 byte - Yes or No
 *          device - 1 byte - Le peripherique pour lequel on change la frequence.
 *
 * @return  None
 *
 ******************************************************************************/
void SPI1SetHighSpeedClk(unsigned char yn,unsigned char device)
{
//    if(device == MPU9250 || device==MPU9250_2)
//    {
//        if(yn)
//        {
//            SPI4CON1bits.SPIEN = 0;	// Disable SPI module
//            //Reglage de l'horloge spi a 4.096Mhz
//
//            SPI4CON1bits.SPIEN = 1;	// Enable SPI module
//        }
//        else
//        {
//            SPI4CON1bits.SPIEN = 0;	// Disable SPI module
//            //Reglage de l'horloge spi a 1Mhz
//            SPI4CON1bits.SPIEN = 1;	// Enable SPI module
//        }
//    }
}

/*******************************************************************************
 * @fn      invertSPI4_CKE()
 *
 * @brief   Fonction permettant d'inverser la polarité de l'horloge a la volée.
 *          (Actif sur front montant ou sur front descendant)
 *
 * @param   None
 *
 * @return  None
 *
 ******************************************************************************/
void invertSPI4_CKE(void)
{	
    SPI4CON1bits.SPIEN = 0;	// Disable SPI module
    SPI4CON1bits.CKE = !SPI4CON1bits.CKE;
    SPI4CON1bits.SPIEN = 1;	// Enable SPI module  
}

void ChipSelectSpi4SetValue_2( unsigned int *port, unsigned char portPin, unsigned char value)
{
    if(value)
        *port |= 1U << portPin;
    else
        *port &= ~(1U<<portPin);
}
/*******************************************************************************
 * @fn      ChipSelectSpi1SetValue()
 *
 * @brief   Fonction utilisée dans la Fonction de l'ecture/ecriture sur le port.
 *          Elle permet de choisir un chip select (donc un peripherique), afin
 *          d'etablir une communication.
 *          Set ChipSelect (Platform dependent)
 *          Neccessite le fichier define.h
 *
 * @param   value - 1 byte - La valeur a atribuer au ChipSelect (0 ou 1).
 *          device - 1 byte - Le peripherique auquel l'on veut communiquer.
 *
 * @return  None
 *
 ******************************************************************************/
void ChipSelectSpi4SetValue( unsigned int value, unsigned char device)
{
    switch(device)
    {
        #ifdef USE_ECG
        case ECG:
                SS_ECG = value;
                break;
        #endif
        #ifdef USE_GYRO
        case GYRO:
                SS_L3G4200 = value;
                break;
        #endif
        case MPU9250:SS_MPU9250=value;
            break;
        case MPU9250_2:SS_MPU9250_2=value;
            break;
        default:
                break;
    }
}


/*******************************************************************************
 * @fn      WriteSingleCommandSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un octet sur le bus SPI.
 *          (Platform dependent).
 *
 * @param   command - 1 byte - Octet a envoyer sur le bus.
 *          device - 1 byte - Peripherique auquel on veut envoyer l'octet.
 *
 * @return  None
 *
 ******************************************************************************/
void WriteSingleCommandSpi4( unsigned char command, unsigned char device)
{	   
    unsigned char commands[1];
    unsigned char results[1];
    commands[0]=command;
    WriteMultipleCommandMultipleReadSpi4(commands, 1, results, 0, device, 0);
}


/*******************************************************************************
 * @fn      WriteSingleCommandPlusNOPSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un octet sur le bus SPI, suivi d'un
 *          dummy byte (0x00).
 *          (Platform dependent).
 *
 * @param   command - 1 byte - Octet a envoyer sur le bus.
 *          device - 1 byte - Peripherique auquel on veut envoyer l'octet.
 *
 * @return  None
 *
 ******************************************************************************/
void WriteSingleCommandPlusNOPSpi4( unsigned char command, unsigned char device )
{	    
    unsigned char commands[2];
    unsigned char results[2];
    commands[0]=command;
    commands[1]=0x00;
    WriteMultipleCommandMultipleReadSpi4(commands, 2, results, 0, device, 0);
}

/*******************************************************************************
 * @fn      WriteMultipleCommandSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un nombre N d'octets sur le bus SPI.
 *          (Platform dependent).
 *
 * @param   commands - 1-256 bytes - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte - Nombre d'octet a envoyer.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *
 * @return  None
 *
 ******************************************************************************/
void WriteMultipleCommandSpi4( unsigned char commands[], int nbCommands, unsigned char device )
{	
    unsigned char results[1];
    WriteMultipleCommandMultipleReadSpi4(commands, nbCommands, results, 0, device, 0);
}


/*******************************************************************************
 * @fn      WriteSingleCommandMultipleReadSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un octet sur le bus SPI, puis de venir
 *          lire un nombre N d'octets.
 *          (Platform dependent).
 *
 * @param   command - 1 byte - Octet a envoyer sur le bus.
 *          results - 1-256 byte - Tableau ou seront stockés les octets lus.
 *          nbResults - 1 byte - Nombre d'octet a lire.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *          invertReadCKE - 1 byte - Si oui ou non on inverse la polarité de
 *                                   l'horloge lors de la lecture.
 *
 * @return  None
 *
 ******************************************************************************/
void WriteSingleCommandMultipleReadSpi4(unsigned char command, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE )
{	    
    unsigned char commands[1];
    commands[0]=command;
    WriteMultipleCommandMultipleReadSpi4(commands, 1, results, nbResults, device, invertReadCKE);
}


/*******************************************************************************
 * @fn      WriteMultipleCommandMultipleReadSpi1()
 *
 * @brief   Fonction permettant l'envoi d'un ou plusieurs octets sur le bus SPI,
 *          puis de venir lire un nombre N d'octets.
 *          Inversion possible de la polarité de CKE entre le write et le read.
 *          (Platform dependent).
 *
 * @param   commands - 1-256 byte - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte- Nombre d'octets a transmettre sur le BUS.
 *          results - 1-256 byte - Tableau ou seront stockés les octets lus.
 *          nbResults - 1 byte - Nombre d'octet a lire.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *          invertReadCKE - 1 byte - Si oui ou non on inverse la polarité de
 *                                   l'horloge lors de la lecture.
 *
 * @return  None
 *
 ******************************************************************************/
void WriteMultipleCommandMultipleReadSpi4(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE )
{		
    int i =0;
        
    //On passe l'horloge spi a 4.096MHz (en fonction du perif)
    SPI1SetHighSpeedClk(1,device);
    //on déclenche le SS
    ChipSelectSpi4SetValue(0, device);
    //On envoie les commandes
    volatile unsigned char dummy;
    for (i=0; i<nbCommands; i++)
    {
            SPI4BUFL=(unsigned int)commands[i];

            //L'anti bloqueur est destiné à sortir d'un figeage SPI
            //Celui-ci se produit quand on reçoit des infos sur l'UART
            antiBlockCounterSpi4 = SPI_TIMEOUT;
            while ( !SPI4STATLbits.SPIRBF  && antiBlockCounterSpi4>0)
            {  antiBlockCounterSpi4--; }
            if(antiBlockCounterSpi4 <= 0)
            { ;  }
            dummy = SPI4BUFL;
    }

    //on permute la valeur de CKE si le flag d'inversion de CKE est active
    if(invertReadCKE)
        invertSPI4_CKE();

    for (i=0; i<nbResults; i++)
    {
            //On envoie un signal nul
            SPI4BUFL=0x0000;
            //Le passage à 1 de RBF indique que les données reçues sont dispo dans le buffer, on attend qu'il passe à 1
            //L'anti bloqueur est destiné à sortir d'un figeage SPI
            //Celui-ci se produit quand on reçoit des infos sur l'UART
            antiBlockCounterSpi4 = SPI_TIMEOUT;
            while ( !SPI4STATLbits.SPIRBF  && antiBlockCounterSpi4>0)
            {  antiBlockCounterSpi4--; }
            if(antiBlockCounterSpi4 <= 0)
            { ; }
            results[i] = SPI4BUFL;
    }

    //on permute la valeur de CKE si le flag d'inversion de CKE est active
    if(invertReadCKE)
        invertSPI4_CKE();

    //on termine le SS
    ChipSelectSpi4SetValue(1, device);
    //On remet l'horloge spi a 1MHz
    SPI1SetHighSpeedClk(0,device);
}
/*******************************************************************************
 * @fn      WriteMultipleCommandMultipleReadSpi4_2()
 *
 * @brief   Fonction permettant l'envoi d'un ou plusieurs octets sur le bus SPI,
 *          puis de venir lire un nombre N d'octets.
 *          Inversion possible de la polarité de CKE entre le write et le read.
 *          (Platform dependent).
 *
 * @param   commands - 1-256 byte - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte- Nombre d'octets a transmettre sur le BUS.
 *          results - 1-256 byte - Tableau ou seront stockés les octets lus.
 *          nbResults - 1 byte - Nombre d'octet a lire.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *          invertReadCKE - 1 byte - Si oui ou non on inverse la polarité de
 *                                   l'horloge lors de la lecture.
 *
 * @return  None
 *
 ******************************************************************************/
void WriteMultipleCommandMultipleReadSpi4_2(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned int* csPort,unsigned char csPinNum, unsigned char invertReadCKE )
{		
    int i =0;
    //on déclenche le SS
    ChipSelectSpi4SetValue_2( csPort,csPinNum, 0);
    //On envoie les commandes
    volatile unsigned char dummy;
    for (i=0; i<nbCommands; i++)
    {
            SPI4BUFL=(unsigned int)commands[i];

            //L'anti bloqueur est destiné à sortir d'un figeage SPI
            //Celui-ci se produit quand on reçoit des infos sur l'UART
            antiBlockCounterSpi4 = SPI_TIMEOUT;
            while ( !SPI4STATLbits.SPIRBF  && antiBlockCounterSpi4>0)
            {  antiBlockCounterSpi4--; }
            if(antiBlockCounterSpi4 <= 0)
            { ;  }
            dummy = SPI4BUFL;
    }

    //on permute la valeur de CKE si le flag d'inversion de CKE est active
    if(invertReadCKE)
        invertSPI4_CKE();

    for (i=0; i<nbResults; i++)
    {
            //On envoie un signal nul
            SPI4BUFL=0x0000;
            //Le passage à 1 de RBF indique que les données reçues sont dispo dans le buffer, on attend qu'il passe à 1
            //L'anti bloqueur est destiné à sortir d'un figeage SPI
            //Celui-ci se produit quand on reçoit des infos sur l'UART
            antiBlockCounterSpi4 = SPI_TIMEOUT;
            while ( !SPI4STATLbits.SPIRBF  && antiBlockCounterSpi4>0)
            {  antiBlockCounterSpi4--; }
            if(antiBlockCounterSpi4 <= 0)
            { ; }
            results[i] = SPI4BUFL;
    }

    //on permute la valeur de CKE si le flag d'inversion de CKE est active
    if(invertReadCKE)
        invertSPI4_CKE();

    //on termine le SS
    ChipSelectSpi4SetValue_2( csPort,csPinNum, 1);
}
/*******************************************************************************
 * @fn      WriteMultipleCommandMultipleReadSpi4Delay()
 *
 * @brief   Fonction permettant l'envoi d'un ou plusieurs octets sur le bus SPI,
 *          puis de venir lire un nombre N d'octets.
 *          Inversion possible de la polarité de CKE entre le write et le read.
 *          (Platform dependent).
 *
 * @param   commands - 1-256 byte - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte- Nombre d'octets a transmettre sur le BUS.
 *          results - 1-256 byte - Tableau ou seront stockés les octets lus.
 *          nbResults - 1 byte - Nombre d'octet a lire.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *          invertReadCKE - 1 byte - Si oui ou non on inverse la polarité de
 *                                   l'horloge lors de la lecture.
 *
 * @return  None
 *
 ******************************************************************************/
void WriteMultipleCommandMultipleReadSpi4Delay(unsigned char commands[], int nbCommands, unsigned char results[], int nbResults, unsigned char device, unsigned char invertReadCKE )
{
    int i =0;
 
    //On passe l'horloge spi a 4.096MHz (en fonction du perif)
    SPI1SetHighSpeedClk(1,device);
    //on déclenche le SS
 //   static int delay = TCLK_ADS129x;
 //   __delay32(2*delay); //Il faut attendre 2 CLK, dans le ADS129x
    ChipSelectSpi4SetValue(0, device);
    //On envoie les commandes
    volatile unsigned char dummy;
    for (i=0; i<nbCommands; i++)
    {
            SPI4BUFL=(unsigned int)commands[i];

            //L'anti bloqueur est destiné à sortir d'un figeage SPI
            //Celui-ci se produit quand on reçoit des infos sur l'UART
            antiBlockCounterSpi4 = SPI_TIMEOUT;
            while ( !SPI4STATLbits.SPIRBF  && antiBlockCounterSpi4>0)
            {  antiBlockCounterSpi4--; }
            if(antiBlockCounterSpi4 <= 0)
            {   }

            dummy = SPI4BUFL;
    }

    //on permute la valeur de CKE si le flag d'inversion de CKE est active
    if(invertReadCKE)
    invertSPI4_CKE();

    //__delay32(4*TCLK_ADS129x); //Il faut attendre 4 CLK, le temps que l'ADS charge ses résultats

    for (i=0; i<nbResults; i++)
    {
            //On envoie un signal nul
            SPI4BUFL=0x0000;
            //Le passage a 0 de TBF indique la fin de transmission, on attend qu'il revienne à 0
            //Le passage à 1 de RBF indique que les données reçues sont dispo dans le buffer, on attend qu'il passe à 1

            //L'anti bloqueur est destiné à sortir d'un figeage SPI
            //Celui-ci se produit quand on reçoit des infos sur l'UART
            antiBlockCounterSpi4 = SPI_TIMEOUT;
            while ( !SPI4STATLbits.SPIRBF  && antiBlockCounterSpi4>0)
            {  antiBlockCounterSpi4--; }
            if(antiBlockCounterSpi4 <= 0)
            {   }

            results[i] = SPI4BUFL;
    }

    //on permute la valeur de CKE si le flag d'inversion de CKE est active
    if(invertReadCKE)
    invertSPI4_CKE();

    //on termine le SS
  //  __delay32(3*TCLK_ADS129x); //Il faut attendre 3 CLK dans le ADS129x
    ChipSelectSpi4SetValue(1, device);
    //On remet l'horloge spi a 1MHz
    SPI1SetHighSpeedClk(0,device);

}

/*******************************************************************************
 * @fn      WriteSingleCommandSpi4Delay()
 *
 * @brief   Fonction permettant l'envoi d'un octet sur le bus SPI.
 *          (Platform dependent).
 *          Ajoute un delay de 4us avant la remonté du chip select.(ads1292)
 *
 * @param   command - 1 byte - Octet a envoyer sur le bus.
 *          device - 1 byte - Peripherique auquel on veut envoyer l'octet.
 *
 * @return  None
 *
 ******************************************************************************/
void WriteSingleCommandSpi4Delay( unsigned char command, unsigned char device)
{
    unsigned char commands[1];
    unsigned char results[1];
    commands[0]=command;
    WriteMultipleCommandMultipleReadSpi4Delay(commands, 1, results, 0, device, 0);
}

/*******************************************************************************
 * @fn      WriteMultipleCommandSpi4Delay()
 *
 * @brief   Fonction permettant l'envoi d'un nombre N d'octets sur le bus SPI.
 *          (Platform dependent).
 *          Rajoute un delay de 4 coups d'horloge a la fin de la com (ads1292)
 *
 * @param   commands - 1-256 bytes - Octets a envoyer sur le bus. (tableau)
 *          nbCommands - 1 byte - Nombre d'octet a envoyer.
 *          device - 1 byte - Peripherique auquel on veut envoyer le/les octet(s).
 *
 * @return  None
 *
 ******************************************************************************/
void WriteMultipleCommandSpi4Delay( unsigned char commands[], int nbCommands, unsigned char device )
{
    unsigned char results[1];
    WriteMultipleCommandMultipleReadSpi4Delay(commands, nbCommands, results, 0, device, 0);
}

#endif //#ifdef USE_SPI4


