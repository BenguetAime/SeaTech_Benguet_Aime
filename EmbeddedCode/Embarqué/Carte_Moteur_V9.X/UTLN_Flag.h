/* 
 * File:   ustv_flag.h
 * Author: Valentin
 *
 * Created on 11 novembre 2013, 18:28
 */

#ifndef USTV_FLAG_H
#define	USTV_FLAG_H
#include "define.h"
__extension__ typedef struct FLAG_SYSTEMBITS{
union {
struct{
  unsigned long ALL:28;
   };

struct {
  unsigned UART1TxTransmissionEnCours:1;
  unsigned UART2TxTransmissionEnCours:1;
  unsigned UART3TxTransmissionEnCours:1;
  unsigned UART4TxTransmissionEnCours:1;
  unsigned UART5TxTransmissionEnCours:1;
  unsigned IsSensorOperationEnabled:1;
  unsigned virtualTimer10HzTimeout:1;
  unsigned virtualTimer1Timeout:1;
  unsigned virtualTimer2Timeout:1;
  unsigned virtualTimer3Timeout:1;
  unsigned virtualTimer4Timeout:1;
  unsigned virtualTimer5Timeout:1;
  unsigned virtualTimer6Timeout:1;
  unsigned virtualTimer7Timeout:1;
  unsigned flag50Hz;
  unsigned flag1Hz;
  unsigned Spi1Interrupted:1;
  unsigned Spi2Interrupted:1;
  unsigned SleepModeRequested:1;
  unsigned LightSleepModeRequested:1;
  unsigned FirstSystemBoot:1;
  };
};
}FLAG_SYSTEM_bits;

__extension__ typedef struct FLAG_INTERFACE_STATE{
union {
struct{
  unsigned long long ALL:36;
   };

struct {
  unsigned Sw1LastState:1;
  unsigned Sw1Interrupt:1;
  unsigned Sw1ClickEvent:1;
  unsigned Sw1LongClickEvent:1;
  unsigned Int0TriggeredAndNotProcessed:1;
  unsigned Sw1LongPressEvent:1;
  };
};
}FLAG_INTERFACE_STATE_bits;

__extension__ typedef struct FLAG_SYSTEM_ERROR_BITS{
union {
struct{
  unsigned long long ALL:32;
   };

struct {
  unsigned UART1ParityError:1;
  unsigned UART1FramingError:1;
  unsigned UART1OverrunError:1;
  unsigned UART2ParityError:1;
  unsigned UART2FramingError:1;
  unsigned UART2OverrunError:1;
  unsigned UART3ParityError:1;
  unsigned UART3FramingError:1;
  unsigned UART3OverrunError:1;
  unsigned UART4ParityError:1;
  unsigned UART4FramingError:1;
  unsigned UART4OverrunError:1;
  
  unsigned LS7366_1InitError:1;
  unsigned LS7366_2InitError:1;
  unsigned LS7366_3InitError:1;
  unsigned LS7366_4InitError:1;
  unsigned LS7366_5InitError:1;
  unsigned LS7366_6InitError:1;
  unsigned LS7366_7InitError:1;
  unsigned LS7366_8InitError:1;
  unsigned IMUInitError:1;
  
  unsigned USBBufferFull:1;
  unsigned USBChkSumError:1;

  
  };
};
}FLAG_SYSTEM_ERROR_bits;

extern volatile FLAG_SYSTEM_bits flagSystem;
extern volatile FLAG_SYSTEM_ERROR_bits flagError;
extern volatile FLAG_INTERFACE_STATE_bits flagInterface;


void InitFlags(void);


#endif	/* USTV_FLAG_H */

