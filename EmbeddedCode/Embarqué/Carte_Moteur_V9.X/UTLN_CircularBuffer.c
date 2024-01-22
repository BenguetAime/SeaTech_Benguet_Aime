#include "UTLN_CircularBuffer.h"
#include "HardwareProfile_CM_V9.h"


#ifdef USE_USB_CIRCULAR_BUFFER
    unsigned int USBCircularRxBufferHead = 0;
    unsigned int USBCircularRxBufferTail = 0;
    unsigned int USBCircularTxBufferHead = 0;
    unsigned int USBCircularTxBufferTail = 0;
    unsigned char USBRxBuffer[USB_CIRCULAR_BUFFER_SIZE];
    unsigned char USBTxBuffer[USB_CIRCULAR_BUFFER_SIZE];   
#endif
    
#ifdef USE_USB_CIRCULAR_BUFFER
void USBResetRxBuffer(void)
{
    USBCircularRxBufferHead = 0;
    USBCircularRxBufferTail = 0;
}

void USBResetTxBuffer(void)
{
    USBCircularTxBufferHead = 0;
    USBCircularTxBufferTail = 0;
}

//void USBReadToRxBuffer(void)
//{
//    USBRxBuffer[USBCircularRxBufferHead] = U1RXREG;
//    if(USBCircularRxBufferHead<USB_CIRCULAR_BUFFER_SIZE-1)
//        USBCircularRxBufferHead+=1;
//    else
//        USBCircularRxBufferHead=0;
//}

BOOL USBIsDataReadyInRxBuffer(void)
{
    if(USBCircularRxBufferHead != USBCircularRxBufferTail)
        return TRUE;
    else
        return FALSE;
}

unsigned char USBReadFromRxBuffer(void)
{
    unsigned char data = USBRxBuffer[USBCircularRxBufferTail];
    if(USBCircularRxBufferTail<USB_CIRCULAR_BUFFER_SIZE-1)
        USBCircularRxBufferTail+=1;
    else
        USBCircularRxBufferTail=0;
    return data;
}

void USBWriteToTxBuffer(unsigned char value)
{
    USBTxBuffer[USBCircularTxBufferHead] = value;
    if(USBCircularTxBufferHead<USB_CIRCULAR_BUFFER_SIZE-1)
        USBCircularTxBufferHead+=1;
    else
        USBCircularTxBufferHead=0;
}

BOOL USBIsDataReadyInTxBuffer(void)
{
    if(USBCircularTxBufferHead != USBCircularTxBufferTail)
        return TRUE;
    else
        return FALSE;
}

unsigned char USBGetOneFromTxBuffer(void)
{
    unsigned char data = USBTxBuffer[USBCircularTxBufferTail];
    if(USBCircularTxBufferTail<USB_CIRCULAR_BUFFER_SIZE-1)
        USBCircularTxBufferTail+=1;
    else
        USBCircularTxBufferTail=0;

    return data;
}

unsigned int USBGetNbOfValuesInTxBuffer(void)
{
    if(USBCircularTxBufferHead>=USBCircularTxBufferTail)
        return USBCircularTxBufferHead-USBCircularTxBufferTail;
    else
        return USB_CIRCULAR_BUFFER_SIZE - USBCircularTxBufferTail + USBCircularTxBufferHead;
}


unsigned int USBGetRemainingSpaceInTxBuffer(void)
{
    return USB_CIRCULAR_BUFFER_SIZE - USBGetNbOfValuesInTxBuffer();
}

#endif
    
