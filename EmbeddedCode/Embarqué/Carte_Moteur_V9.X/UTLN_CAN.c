#include <p33Exxxx.h>
#include <stdint.h>
#include "UTLN_CAN.h"
#include "UTLN_RM_C620.h"
#include "Define.h"
#include "RobotObj.h"


#define NUM_OF_CAN_BUFFERS 32
/* This is the CAN message buffer declaration. Note the buffer alignment. */
//unsigned int can1MsgBuf[NUM_OF_CAN_BUFFERS][8]
//__attribute__((aligned(NUM_OF_CAN_BUFFERS * 16))) ;

//Due to ERRATA SHEET of CAN&DMA
//Place ECAN Buffer in DMA SPACE (DPSRAM)
__eds__ unsigned int can1MsgBuf[NUM_OF_CAN_BUFFERS][8] __attribute__((eds,space(dma),aligned(NUM_OF_CAN_BUFFERS*16)));


bool _can1IsInit=false;

uint16_t can1MotBuff0Watchdog=0;
uint16_t can1MotBuff1Watchdog=0;
uint16_t can1MotBuff2Watchdog=0;
uint16_t can1MotBuff3Watchdog=0;
/* Place code to set device speed here. For this example the device speed should be set at
40 MHz (i.e., the device is operating at 40 MIPS). */
//ConfigureDeviceClockFor40MIPS();
/* The dsPIC33E device features I/O remap. This I/O remap configuration for the CAN module can
be performed here. */
//SetIORemapForCANModule();

//<editor-fold defaultstate="collapsed" desc="CAN1_Init">
void CAN1_Init(void)
{
    //Elevate the system priority of DMA by writing a 0x20 to the MSTRPR (Master Priority)
    //SFR register (address: 0x0058). This will also prevent the DMA from being held ?OFF?.
    //MSTRPR=0x20;
    
    /* Set up the CAN1 module to operate at 250 kbps. The CAN module should be first placed
    in configuration mode. */
    C1CTRL1bits.REQOP = 4;
    while(C1CTRL1bits.OPMODE != 4);
    C1CTRL1bits.WIN = 0;
    C1CTRL1bits.CANCKS=1;
    /* Set up the CAN module for 250kbps speed with 10 Tq per bit. */
    //C1CFG1 = 0x47; // BRP = 8 SJW = 2 Tq
    
    C1CFG1bits.BRP=1;        //TQ=2*30*1/FCAN
    C1CFG2bits.SEG1PH=4;        //Segment 1 5TQ
    C1CFG2bits.SEG2PHTS=1;      //Segment 2 programmable
    C1CFG2bits.SEG2PH=3;        //Segment 2 = 4TQ
    C1CFG2bits.PRSEG=4;         //Propagation Time = 5 TQ         
    
    C1FCTRL = 0xC01F; // No FIFO, 32 Buffers
    /* Assign 32x8word Message Buffers for CAN1 in device RAM. This example uses DMA0 for TX.
    Refer to Section 9.1 ?DMA Operation for Transmitting Data? for details on DMA channel
    configuration for CAN transmit. */
    DMA0CONbits.SIZE = 0x0;
    DMA0CONbits.DIR = 0x1;
    DMA0CONbits.AMODE = 0x2;
    DMA0CONbits.MODE = 0x0;
    DMA0REQbits.IRQSEL = 0b01000110;
    DMA0CNT = 7;
    DMA0PAD = (volatile unsigned int)&C1TXD;
    DMA0STAL = (unsigned int) &can1MsgBuf;
    DMA0STAH = 0;
    //DMA0CONbits.CHEN = 0x1;
    
    //DMA1 For CAN Receive
    DMA1CONbits.SIZE = 0x0;
    DMA1CONbits.DIR = 0x0;
    DMA1CONbits.AMODE = 0x2;
    DMA1CONbits.MODE = 0x0;
    DMA1REQ = 34;
    DMA1CNT = 7;
    DMA1PAD = (volatile unsigned int)&C1RXD;
    DMA1STAL = (unsigned int) &can1MsgBuf;
    DMA1STAH = 0;
    //DMA1CONbits.CHEN = 0x1;
    
    /* Channel Interrupt Enable: Enable DMA Channel 0 Interrupt */
    IFS0bits.DMA0IF=0;
    IEC0bits.DMA0IE = 1;
    IPC1bits.DMA0IP=6;
    /* Channel Interrupt Enable: Enable DMA Channel 1 Interrupt */
    IFS0bits.DMA1IF=0;
    IEC0bits.DMA1IE = 1;
    IPC3bits.DMA1IP=6;
    

    /* Configure Message Buffer 0 for Transmission and assign priority */
    C1TR01CONbits.TXEN0 = 0x1;
    C1TR01CONbits.TX0PRI = 0x3;
    
    C1INTEbits.RBIE=1;
    C1INTEbits.RBOVIE=1;
    IEC2bits.C1IE=1;
    //IEC2bits.C1RXIE=1;
    
    /* At this point the CAN1 module is ready to transmit a message. Place the CAN module in
    Normal mode. */
    C1CTRL1bits.REQOP = 0;
    while(C1CTRL1bits.OPMODE != 0);
    
    _can1IsInit=true;
}
 //</editor-fold>

//<editor-fold defaultstate="collapsed" desc="CAN1_Stop">
void CAN1_Stop(void)
{
    //Elevate the system priority of DMA by writing a 0x20 to the MSTRPR (Master Priority)
    //SFR register (address: 0x0058). This will also prevent the DMA from being held ?OFF?.
    //MSTRPR=0x20;
    
    /* Set up the CAN1 module to operate at 250 kbps. The CAN module should be first placed
    in configuration mode. */
    C1CTRL1bits.REQOP = 4;
    while(C1CTRL1bits.OPMODE != 4);
    
    IEC0bits.DMA0IE = 0;
    IEC0bits.DMA1IE = 0;
    

    /* Configure Message Buffer 0 for Transmission and assign priority */
    C1TR01CONbits.TXEN0 = 0x0;
    C1TR01CONbits.TX0PRI = 0x3;
    
    C1INTEbits.RBIE=0;
    C1INTEbits.RBOVIE=0;
    IEC2bits.C1IE=0;
   
    /* At this point the CAN1 module is ready to transmit a message. Place the CAN module in
    Normal mode. */
    C1CTRL1bits.REQOP = 0;
    while(C1CTRL1bits.OPMODE != 0);
    
    _can1IsInit=false;
}
 //</editor-fold>

//<editor-fold defaultstate="collapsed" desc="CAN1_SetAcceptanceFilterMask">
void CAN1_SetAcceptanceFilterMask(uint8_t maskNum, uint16_t mask)
{
    /*SFR Map Window Select bit
    1 = Uses filter window
    0 = Uses buffer window*/
    C1CTRL1bits.WIN = 1;
    
    /* Configure Acceptance Filter Mask maskNum register to mask SID<2:0>
    Tous les bits du SID doivent matcher les bits du filtre (les 1 indiquent quels bits du SID du message recu doivent correspondre avec le filtre)*/
    switch(maskNum&0b11)
    {
        case 0: /* Configure Acceptance Filter Mask 0 register to mask SID<2:0>*/
                C1RXM0SIDbits.SID = mask;
                break;
        case 1: /* Configure Acceptance Filter Mask 1 register to mask SID<2:0>*/
                C1RXM1SIDbits.SID = mask;
                break;
        case 2: /* Configure Acceptance Filter Mask 2 register to mask SID<2:0>*/
                C1RXM2SIDbits.SID = mask;
                break;
        default:
            break;
    }
    /* Clear Window Bit to Access CAN
    * Control Registers */
    C1CTRL1bits.WIN=0x0;
}
 //</editor-fold>

//<editor-fold defaultstate="collapsed" desc="CAN1_SetAcceptanceFilter">
void CAN1_SetAcceptanceFilter(uint8_t filterNum, uint16_t sid, uint8_t selectedMask, uint8_t bufferPointerIndex)
{
    /*SFR Map Window Select bit
    1 = Uses filter window
    0 = Uses buffer window*/
    C1CTRL1bits.WIN = 1;
    
    switch(filterNum&0x0f)
    {
        case 0: /* Select Acceptance Filter Mask 0 for Acceptance Filter 0 */
                C1FMSKSEL1bits.F0MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 0 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF0SIDbits.SID = sid;
                /* Acceptance Filter 0 to check for Standard Identifier */
                C1RXF0SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 0 to use Message Buffer 10 to store message */
                C1BUFPNT1bits.F0BP = bufferPointerIndex;
                /* Filter 0 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN0=0x1;
                break;
        case 1: /* Select Acceptance Filter Mask for Acceptance Filter 1 */
                C1FMSKSEL1bits.F1MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 1 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF1SIDbits.SID = sid;
                /* Acceptance Filter 0 to check for Standard Identifier */
                C1RXF1SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 0 to use Message Buffer 10 to store message */
                C1BUFPNT1bits.F1BP = bufferPointerIndex;
                /* Filter 0 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN1=0x1;
                break;
        case 2: /* Select Acceptance Filter Mask for Acceptance Filter 2 */
                C1FMSKSEL1bits.F2MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 2 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF2SIDbits.SID = sid;
                /* Acceptance Filter 2 to check for Standard Identifier */
                C1RXF2SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 2 to use Message Buffer XX to store message */
                C1BUFPNT1bits.F2BP = bufferPointerIndex;
                /* Filter 2 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN2=0x1;
                break;   
                
        case 3: /* Select Acceptance Filter Mask for Acceptance Filter 3 */
                C1FMSKSEL1bits.F3MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 3 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF3SIDbits.SID = sid;
                /* Acceptance Filter 3 to check for Standard Identifier */
                C1RXF3SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 3 to use Message Buffer XX to store message */
                C1BUFPNT1bits.F3BP = bufferPointerIndex;
                /* Filter 3 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN3=0x1;
                break;
        case 4: /* Select Acceptance Filter Mask for Acceptance Filter 4 */
                C1FMSKSEL1bits.F4MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 1 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF4SIDbits.SID = sid;
                /* Acceptance Filter 4 to check for Standard Identifier */
                C1RXF4SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 4 to use Message Buffer XX to store message */
                C1BUFPNT2bits.F4BP = bufferPointerIndex;
                /* Filter 4 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN4=0x1;
                break;
        case 5: /* Select Acceptance Filter Mask for Acceptance Filter 5 */
                C1FMSKSEL1bits.F5MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 5 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF5SIDbits.SID = sid;
                /* Acceptance Filter 5 to check for Standard Identifier */
                C1RXF5SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 5 to use Message Buffer XX to store message */
                C1BUFPNT2bits.F5BP = bufferPointerIndex;
                /* Filter 5 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN5=0x1;
                break;
        case 6: /* Select Acceptance Filter Mask for Acceptance Filter 6 */
                C1FMSKSEL1bits.F6MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 6 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF6SIDbits.SID = sid;
                /* Acceptance Filter 6 to check for Standard Identifier */
                C1RXF6SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 6 to use Message Buffer XX to store message */
                C1BUFPNT2bits.F6BP = bufferPointerIndex;
                /* Filter 6 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN6=0x1;
                break;
        case 7: /* Select Acceptance Filter Mask for Acceptance Filter 7 */
                C1FMSKSEL1bits.F7MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 7 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF7SIDbits.SID = sid;
                /* Acceptance Filter 7 to check for Standard Identifier */
                C1RXF7SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 7 to use Message Buffer XX to store message */
                C1BUFPNT2bits.F7BP = bufferPointerIndex;
                /* Filter 7 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN7=0x1;
                break;
        case 8: /* Select Acceptance Filter Mask for Acceptance Filter 8 */
                C1FMSKSEL2bits.F8MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 8 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF8SIDbits.SID = sid;
                /* Acceptance Filter 8 to check for Standard Identifier */
                C1RXF8SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 8 to use Message Buffer XX to store message */
                C1BUFPNT3bits.F8BP = bufferPointerIndex;
                /* Filter 8 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN8=0x1;
                break;
        case 9: /* Select Acceptance Filter Mask for Acceptance Filter 9 */
                C1FMSKSEL2bits.F9MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 9 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF9SIDbits.SID = sid;
                /* Acceptance Filter 9 to check for Standard Identifier */
                C1RXF9SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 9 to use Message Buffer XX to store message */
                C1BUFPNT3bits.F9BP = bufferPointerIndex;
                /* Filter 9 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN9=0x1;
                break;
        case 10: /* Select Acceptance Filter Mask for Acceptance Filter 10 */
                C1FMSKSEL2bits.F10MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 10 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF10SIDbits.SID = sid;
                /* Acceptance Filter 10 to check for Standard Identifier */
                C1RXF10SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 10 to use Message Buffer XX to store message */
                C1BUFPNT3bits.F10BP = bufferPointerIndex;
                /* Filter 10 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN10=0x1;
                break;  
        case 11: /* Select Acceptance Filter Mask for Acceptance Filter 10 */
                C1FMSKSEL2bits.F11MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 11 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF11SIDbits.SID = sid;
                /* Acceptance Filter 11 to check for Standard Identifier */
                C1RXF11SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 11 to use Message Buffer XX to store message */
                C1BUFPNT3bits.F11BP = bufferPointerIndex;
                /* Filter 11 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN11=0x1;
                break;
        case 12: /* Select Acceptance Filter Mask for Acceptance Filter 12 */
                C1FMSKSEL2bits.F12MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 12 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF12SIDbits.SID = sid;
                /* Acceptance Filter 12 to check for Standard Identifier */
                C1RXF12SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 12 to use Message Buffer XX to store message */
                C1BUFPNT4bits.F12BP = bufferPointerIndex;
                /* Filter 12 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN12=0x1;
                break;
        case 13: /* Select Acceptance Filter Mask for Acceptance Filter 13 */
                C1FMSKSEL2bits.F13MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 13 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF13SIDbits.SID = sid;
                /* Acceptance Filter 13 to check for Standard Identifier */
                C1RXF13SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 13 to use Message Buffer XX to store message */
                C1BUFPNT4bits.F13BP = bufferPointerIndex;
                /* Filter 13 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN13=0x1;
                break;
        case 14: /* Select Acceptance Filter Mask for Acceptance Filter 14 */
                C1FMSKSEL2bits.F14MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 14 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF14SIDbits.SID = sid;
                /* Acceptance Filter 14 to check for Standard Identifier */
                C1RXF14SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 14 to use Message Buffer XX to store message */
                C1BUFPNT4bits.F14BP = bufferPointerIndex;
                /* Filter 14 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN14=0x1;
                break;
        case 15: /* Select Acceptance Filter Mask for Acceptance Filter 15 */
                C1FMSKSEL2bits.F15MSK=selectedMask&0b11;
                /* Configure Acceptance Filter 15 to match standard identifier
                Filter Bits (11-bits)*/
                C1RXF15SIDbits.SID = sid;
                /* Acceptance Filter 15 to check for Standard Identifier */
                C1RXF15SIDbits.EXIDE= 0x0;
                /* Acceptance Filter 15 to use Message Buffer XX to store message */
                C1BUFPNT4bits.F15BP = bufferPointerIndex;
                /* Filter 15 enabled for Identifier match with incoming message */
                C1FEN1bits.FLTEN15=0x1;
                break;                
        default:
            break;
    }
    
    /* Clear Window Bit to Access CAN
    * Control Registers */
    C1CTRL1bits.WIN=0x0;
}
 //</editor-fold>

//<editor-fold defaultstate="collapsed" desc="CAN1_SetAcceptanceFilter0">
void CAN_SetAcceptanceFilter0(uint16_t sid)
{
    /*SFR Map Window Select bit
    1 = Uses filter window
    0 = Uses buffer window*/
    C1CTRL1bits.WIN = 1;
    
    /* Select Acceptance Filter Mask 0 for Acceptance Filter 0 */
    C1FMSKSEL1bits.F0MSK=0x0;
    /* Configure Acceptance Filter Mask 0 register to mask SID<2:0>
    * Mask Bits (11-bits) : 0b111 1111 1000 */
    C1RXM0SIDbits.SID = 0x7FF;      //Tous les bits du SID doivent matcher les bits du filtre (les 1 indiquent quels bits du SID du message recu doivent correspondre avec le SID filtre)
    
    /* Configure Acceptance Filter 0 to match standard identifier
    Filter Bits (11-bits): 0b011 1010 xxx with the mask setting, message with SID
    range 0x1D0-0x1D7 will be accepted by the CAN module. */
    //C1RXF0SIDbits.SID = 0x01D0;
    C1RXF0SIDbits.SID = sid;//<<2;
    
    /* Acceptance Filter 0 to check for Standard Identifier */
    C1RXM0SIDbits.MIDE = 0x1;
    C1RXF0SIDbits.EXIDE= 0x0;
    
    /* Acceptance Filter 0 to use Message Buffer 10 to store message */
    C1BUFPNT1bits.F0BP = 0xA;
    /* Filter 0 enabled for Identifier match with incoming message */
    C1FEN1bits.FLTEN0=0x1;
    /* Clear Window Bit to Access CAN
    * Control Registers */
    C1CTRL1bits.WIN=0x0;
}
 //</editor-fold>
//<editor-fold defaultstate="collapsed" desc="CAN1_SetAcceptanceFilter1">
void CAN_SetAcceptanceFilter1(uint16_t sid)
{
    /*SFR Map Window Select bit
    1 = Uses filter window
    0 = Uses buffer window*/
    C1CTRL1bits.WIN = 1;
    
    /* Select Acceptance Filter Mask 0 for Acceptance Filter 1 */
    C1FMSKSEL1bits.F1MSK=0x0;
    /* Configure Acceptance Filter Mask 0 register to mask SID<2:0>
    * Mask Bits (11-bits) : 0b111 1111 1000 */
    //C1RXM0SIDbits.SID = 0x7F8;
    //C1RXM1SIDbits.SID=sid<<2;
    
    /* Configure Acceptance Filter 0 to match standard identifier
    Filter Bits (11-bits): 0b011 1010 xxx with the mask setting, message with SID
    range 0x1D0-0x1D7 will be accepted by the CAN module. */
    //C1RXF0SIDbits.SID = 0x01D0;
    C1RXF1SIDbits.SID = sid;//<<2;
    
    /* Acceptance Filter 1 to check for Standard Identifier */
    C1RXM1SIDbits.MIDE = 0x1;
    C1RXF1SIDbits.EXIDE= 0x0;
    /* Acceptance Filter 1 to use Message Buffer 11 to store message */
    C1BUFPNT1bits.F1BP = 0xB;
    /* Filter 1 enabled for Identifier match with incoming message */
    C1FEN1bits.FLTEN1=0x1;
    /* Clear Window Bit to Access CAN
    * Control Registers */
    C1CTRL1bits.WIN=0x0;
}
 //</editor-fold>
//<editor-fold defaultstate="collapsed" desc="CAN1_SetAcceptanceFilter2">
void CAN_SetAcceptanceFilter2(uint16_t sid)
{
    /*SFR Map Window Select bit
    1 = Uses filter window
    0 = Uses buffer window*/
    C1CTRL1bits.WIN = 1;
    
    /* Select Acceptance Filter Mask 0 for Acceptance Filter 2 */
    C1FMSKSEL1bits.F2MSK=0x0;
    /* Configure Acceptance Filter Mask 0 register to mask SID<2:0>
    * Mask Bits (11-bits) : 0b111 1111 1000 */
    //C1RXM0SIDbits.SID = 0x7F8;
    //C1RXM1SIDbits.SID=sid<<2;
    
    /* Configure Acceptance Filter 0 to match standard identifier
    Filter Bits (11-bits): 0b011 1010 xxx with the mask setting, message with SID
    range 0x1D0-0x1D7 will be accepted by the CAN module. */
    //C1RXF0SIDbits.SID = 0x01D0;
    C1RXF2SIDbits.SID = sid;//<<2;
    
    /* Acceptance Filter 2 to check for Standard Identifier */
    C1RXF2SIDbits.EXIDE= 0x0;
    /* Acceptance Filter 2 to use Message Buffer 12 to store message */
    C1BUFPNT1bits.F2BP = 0xC;
    /* Filter 2 enabled for Identifier match with incoming message */
    C1FEN1bits.FLTEN2=0x1;
    /* Clear Window Bit to Access CAN
    * Control Registers */
    C1CTRL1bits.WIN=0x0;
}
 //</editor-fold>
//<editor-fold defaultstate="collapsed" desc="CAN1_SetAcceptanceFilter3">
void CAN_SetAcceptanceFilter3(uint16_t sid)
{
    /*SFR Map Window Select bit
    1 = Uses filter window
    0 = Uses buffer window*/
    C1CTRL1bits.WIN = 1;
    
    /* Select Acceptance Filter Mask 0 for Acceptance Filter 3 */
    C1FMSKSEL1bits.F3MSK=0x0;
    /* Configure Acceptance Filter Mask 0 register to mask SID<2:0>
    * Mask Bits (11-bits) : 0b111 1111 1000 */
    //C1RXM0SIDbits.SID = 0x7F8;
    //C1RXM1SIDbits.SID=sid<<2;
    
    /* Configure Acceptance Filter 0 to match standard identifier
    Filter Bits (11-bits): 0b011 1010 xxx with the mask setting, message with SID
    range 0x1D0-0x1D7 will be accepted by the CAN module. */
    //C1RXF0SIDbits.SID = 0x01D0;
    C1RXF3SIDbits.SID = sid;
    
    /* Acceptance Filter 3 to check for Standard Identifier */
    C1RXF3SIDbits.EXIDE= 0x0;
    /* Acceptance Filter 3 to use Message Buffer 13 to store message */
    C1BUFPNT1bits.F3BP = 0xD;
    /* Filter 3 enabled for Identifier match with incoming message */
    C1FEN1bits.FLTEN3=0x1;
    /* Clear Window Bit to Access CAN
    * Control Registers */
    C1CTRL1bits.WIN=0x0;
}
 //</editor-fold>

//<editor-fold defaultstate="collapsed" desc="CAN1_StartTransmissions">
void CAN1_StartTransmissions(void)
{
    DMA0CONbits.CHEN = 0x1;
    DMA1CONbits.CHEN = 0x1;
}
 //</editor-fold>

//<editor-fold defaultstate="collapsed" desc="CAN_SendStdFrame">
void CAN_SendStdFrame(uint16_t sid,uint8_t payload[], uint8_t payloadLength)
{
    
    //See Fig 7-1 of Ref Manual for more information about data organisation
    
    //C1TXD=0xBB;
    //DMA0CNT = 3+payloadLength/2;
    /* Write to message buffer 0 */
    /* CiTRBnSID = 0bxxx1 0010 0011 1100
    IDE = 0b0
    SRR = 0b0
    SID<10:0>= 0b100 1000 1111 */
    //can1MsgBuf[0][0] = 0x123C;
    can1MsgBuf[0][0] = sid<<2;
    /* CiTRBnEID = 0bxxxx 0000 0000 0000
    EID<17:6> = 0b0000 0000 0000 */
    can1MsgBuf[0][1] = 0x0000;
    /* CiTRBnDLC = 0b0000 0000 xxx0 1111
    EID<17:6> = 0b000000
    RTR = 0b0
    RB1 = 0b0
    RB0 = 0b0
    DLC = 0b1111 */
    can1MsgBuf[0][2] = (payloadLength&0b1111);
    /* Write message data bytes */
//    can1MsgBuf[0][3] = 0xabcd;
//    can1MsgBuf[0][4] = 0xabcd;
//    can1MsgBuf[0][5] = 0xabcd;
//    can1MsgBuf[0][6] = 0xabcd;

    uint8_t i;
    for(i=0;i<payloadLength;i++)
    {
        if(i&0x01)
            can1MsgBuf[0][3+i/2]|=(payload[i]<<8);
        else
            can1MsgBuf[0][3+i/2]=(payload[i]);
    }
    /* Request message buffer 0 transmission */
    C1TR01CONbits.TXREQ0 = 0x1;
    /* The following shows an example of how the TXREQ bit can be polled to check if transmission
    is complete. */
    uint32_t antiBlock=0;
    //while(C1TR01CONbits.TXREQ0 == 1);
    while(C1TR01CONbits.TXREQ0 == 1 && antiBlock<10000)
    {
        antiBlock++;
    }
    
    if(C1TR01CONbits.TXREQ0==1)
    {
        //On a eu une erreur de transmission
        if(C1TR01CONbits.TXLARB0)
        {
            //Message retransmitted succesfully
        }
        else
        {
            //Message not retransmitted
            _LED_R1=!_LED_R1;
        }
    }
    /* Message was placed successfully on the bus */
}
 //</editor-fold>

//<editor-fold defaultstate="collapsed" desc="CAN_ReadValuesFromBuffer0">
void CAN_ReadValuesFromBuffer0(void)
{
    //Get Motor1 Values
    robotState.brushlessStatus[0].rotorAngle=MSB_LSB_INVERT(can1MsgBuf[10][3]);
    robotState.brushlessStatus[0].rotorAngleDeg= (360.0/8192.0)*robotState.brushlessStatus[0].rotorAngle;
    robotState.brushlessStatus[0].rotorSpeed=MSB_LSB_INVERT(can1MsgBuf[10][4]);
    robotState.brushlessStatus[0].motorTemperature=can1MsgBuf[10][6];
    robotState.brushlessStatus[0].currentTorque=MSB_LSB_INVERT(can1MsgBuf[10][5]);
}
 //</editor-fold>
//<editor-fold defaultstate="collapsed" desc="CAN_ReadValuesFromBuffer1">
void CAN_ReadValuesFromBuffer1(void)
{
    //Get Motor2 Values
    robotState.brushlessStatus[1].rotorAngle=MSB_LSB_INVERT(can1MsgBuf[11][3]);
    robotState.brushlessStatus[1].rotorAngleDeg= (360.0/8192.0)*robotState.brushlessStatus[1].rotorAngle;
    robotState.brushlessStatus[1].rotorSpeed=MSB_LSB_INVERT(can1MsgBuf[11][4]);
    robotState.brushlessStatus[1].motorTemperature=can1MsgBuf[11][6];
    robotState.brushlessStatus[1].currentTorque=MSB_LSB_INVERT(can1MsgBuf[11][5]);
}
 //</editor-fold>
//<editor-fold defaultstate="collapsed" desc="CAN_ReadValuesFromBuffer2">
void CAN_ReadValuesFromBuffer2(void)
{
    //Get Motor2 Values
    robotState.brushlessStatus[2].rotorAngle=MSB_LSB_INVERT(can1MsgBuf[12][3]);
    robotState.brushlessStatus[2].rotorAngleDeg= (360.0/8192.0)*robotState.brushlessStatus[2].rotorAngle;
    robotState.brushlessStatus[2].rotorSpeed=MSB_LSB_INVERT(can1MsgBuf[12][4]);
    robotState.brushlessStatus[2].motorTemperature=can1MsgBuf[12][6];
    robotState.brushlessStatus[2].currentTorque=MSB_LSB_INVERT(can1MsgBuf[12][5]);
}
 //</editor-fold>
//<editor-fold defaultstate="collapsed" desc="CAN_ReadValuesFromBuffer3">
void CAN_ReadValuesFromBuffer3(void)
{
    //Get Motor2 Values
    robotState.brushlessStatus[3].rotorAngle=MSB_LSB_INVERT(can1MsgBuf[13][3]);
    robotState.brushlessStatus[3].rotorAngleDeg= (360.0/8192.0)*robotState.brushlessStatus[3].rotorAngle;
    robotState.brushlessStatus[3].rotorSpeed=MSB_LSB_INVERT(can1MsgBuf[13][4]);
    robotState.brushlessStatus[3].motorTemperature=can1MsgBuf[13][6];
    robotState.brushlessStatus[3].currentTorque=MSB_LSB_INVERT(can1MsgBuf[13][5]);
}
 //</editor-fold>
int16_t value;
volatile uint8_t payload[8];

//<editor-fold defaultstate="collapsed" desc="CAN_UpdateSpeed">
void CAN_UpdateSpeed(void)
{
    if(_can1IsInit)
    {    
        value=SPEEDPERCENT_TO_CAN_RATIO*robotState.vitesseMoteur1CommandePercent;
        payload[0]=MSB_UINT16(value);
        payload[1]=LSB_UINT16(value);
        value=SPEEDPERCENT_TO_CAN_RATIO*robotState.vitesseMoteur2CommandePercent;
        payload[2]=MSB_UINT16(value);
        payload[3]=LSB_UINT16(value);
        value=SPEEDPERCENT_TO_CAN_RATIO*robotState.vitesseMoteur3CommandePercent;
        payload[4]=MSB_UINT16(value);
        payload[5]=LSB_UINT16(value);
        value=SPEEDPERCENT_TO_CAN_RATIO*robotState.vitesseMoteur4CommandePercent;
        payload[6]=MSB_UINT16(value);
        payload[7]=LSB_UINT16(value);
        CAN_SendStdFrame(0x200,(uint8_t*)payload, 8);
    }
}
 //</editor-fold>