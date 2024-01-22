/*==============================================================================
Copyright 2016 Microchip Technology Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef __BPS_H
#define __BPS_H

#ifdef __cplusplus
     extern "C" {
 #endif

// A bare minimum BSP to blink the Explorer16 LED

// LEDs
#define LED0_TRIS           (TRISAbits.TRISA0)  // Ref D3
#define LED0_IO             (LATAbits.LATA0)
#define LED1_TRIS           (TRISAbits.TRISA1)  // Ref D4
#define LED1_IO             (LATAbits.LATA1)
#define LED2_TRIS           (TRISAbits.TRISA2)  // Ref D5
#define LED2_IO             (LATAbits.LATA2)
#define LED3_TRIS           (TRISAbits.TRISA3)  // Ref D6
#define LED3_IO             (LATAbits.LATA3)
#define LED4_TRIS           (TRISAbits.TRISA4)  // Ref D7
#define LED4_IO             (LATAbits.LATA4)
#define LED5_TRIS           (TRISAbits.TRISA5)  // Ref D8
#define LED5_IO             (LATAbits.LATA5)
#define LED6_TRIS           (TRISAbits.TRISA6)  // Ref D9
#define LED6_IO             (LATAbits.LATA6)
#define LED7_TRIS           (LATAbits.LATA7)    // Ref D10;  Note: This is multiplexed with BUTTON1, so this LED can't be used.  However, it will glow very dimmly due to a weak pull up resistor.
#define LED7_IO             (LATAbits.LATA7)
#define LED_GET()           (*((volatile unsigned char *)(&LATA)))
#define LED_PUT(a)          (*((volatile unsigned char *)(&LATA)) = (a))

// Momentary push buttons
#define BUTTON0_TRIS        (TRISDbits.TRISD13) // Ref S4
#define BUTTON0_IO          (PORTDbits.RD13)
#define BUTTON1_TRIS        (TRISAbits.TRISA7)  // Ref S5;  Note: This is multiplexed with LED7
#define BUTTON1_IO          (PORTAbits.RA7)
#define BUTTON2_TRIS        (TRISDbits.TRISD7)  // Ref S6
#define BUTTON2_IO          (PORTDbits.RD7)
#define BUTTON3_TRIS        (TRISDbits.TRISD6)  // Ref S3
#define BUTTON3_IO          (PORTDbits.RD6)

void BspInit(void);
void ToggleLed(void);
void SpiInit(void);

#ifdef __cplusplus
     }
 #endif


#endif // __BPS_H