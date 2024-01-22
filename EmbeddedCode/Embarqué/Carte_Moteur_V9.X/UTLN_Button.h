/* 
 * File:   ustv_button.h
 * Author: Valentin
 *
 * Created on 17 novembre 2013, 13:44
 */

#ifndef UTLN_BUTTON_H
#define	UTLN_BUTTON_H
#include "Define.h"
#define SB_TIME_WINDOW_START_STOP_ACQ 100
#define SB_TIME_WINDOW_WAKESLEEP 2000
#define BUTTON_COUNT UTLN_BUTTON_COUNT
#define SEQ_COUNT 2
#define BUTTON_V2
extern volatile unsigned long g_longTimeStamp;
__extension__ typedef struct CLICK_SEQUENCEBITS{
union {
struct {
  unsigned ALL:16;
   };

struct {
  unsigned click1:2;
  unsigned click2:2;
  unsigned click3:2;
  unsigned click4:2;
  unsigned click5:2;
  unsigned click6:2;
  unsigned click7:2;
  unsigned click8:2;
  };
};
}CLICK_SEQUENCE_bits;

//Declaration d'un nouveau type (pointeur sur fonction)
typedef void (*BUTTON_CallBack)(void);

typedef struct BUTTONCONFIGBITS{
  volatile unsigned int swPushedTimeStamp;
  volatile unsigned int swReleasedTimeStamp;
  volatile unsigned int swLastClickTimeStamp;
  uint8 swLastState;
  volatile unsigned int* port;
  unsigned int specialPressTime;
  BUTTON_CallBack clickCallback;                 //Pointeur vers un callback utilisatuer
  BUTTON_CallBack longClickcallBack;             //Pointeur vers un callback utilisatuer
  uint8 pin;
}BUTTON_CONFIG_bits;

#define SEQUENCE_CallBack BUTTON_CallBack
typedef struct BUTTONSEQUENCEBITS{
  uint8 sequenceLen;
  uint8 seq[8];
  SEQUENCE_CallBack seqCallback;
}BUTTON_SEQUENCE_bits;

#ifdef BUTTON_V2
void ButtonRegister(uint8 buttonNum, unsigned int* buttonPort, uint8 buttonPin);
void ButtonRegisterWithCallBack(uint8 buttonNum,volatile unsigned int* buttonPort, uint8 buttonPin,BUTTON_CallBack callback);
void ButtonClickCallbackRegister(uint8 buttonNum,BUTTON_CallBack callback);
void ButtonSequenceRegister(uint8 *buttonSequence, uint8 sequenceLength, BUTTON_CallBack callback);
void ButtonLongClickRegister(uint8 buttonNum, unsigned int pressTime, BUTTON_CallBack callback);
void ClickSequenceReset(void);
void ClickSequenceAdd(uint8 numButton);
#ifdef USE_LONG_CLICK    
    void LongClickSequenceAdd(unsigned char numButton);
#endif
    
void SwPushed(uint8 buttonNum);
void SwReleased(uint8 buttonNum);
void IsSequenceFinished(void);
void AnalyzeClickSequence(void);
#else
#ifdef HW_SW1
void InitSW1(void);
void Sw1Pushed(void);
void Sw1Released(void);
void Sw1OnClick(void);
void Sw1OnLongClick(void);
void Sw1OnDoubleClick(void);
void Sw1OnTripleClick(void);
void Sw1OnQuadrupleClick(void);
void Sw1OnFiveClick(void);
void Sw1OnSixtClick(void);
void Sw1OnSeventhClick(void);
#endif

#ifdef HW_SW2
void InitSW2(void);
void Sw2Pushed(void);
void Sw2Released(void);
void Sw2OnClick(void);
void Sw2OnDoubleClick(void);
void Sw2OnTripleClick(void);
#endif

#ifdef HW_SW3
void InitSW3(void);
void Sw3Pushed(void);
void Sw3Released(void);
void Sw3OnClick(void);
void Sw3OnDoubleClick(void);
void Sw3OnTripleClick(void);
#endif

//#ifdef USE_USB_CHARGE
void InitPresenceUSB(void);
void UsbConnected(void);
void UsbDisconnected(void);
void UsbChargeStarted(void);
void UsbChargeFinished(void);
//#endif

#if (defined(HW_SW1) || defined(HW_SW2))
void ClickSequenceReset(void);
void ClickSequenceAdd(unsigned char numButton);
void LongClickSequenceAdd(unsigned char numButton);
void IsSequenceFinished(void);
void AnalyzeClickSequence(void);
#endif

#endif
#endif	/* USTV_BUTTON_H */

