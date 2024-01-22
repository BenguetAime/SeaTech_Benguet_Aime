#include "UTLN_QEI.h"
#include <xc.h>
#include <math.h>

void InitQEI1(void)
{
    QEI1IOCbits.SWPAB = 1; //QEAx and QEBx are swapped        
    QEI1GECL = 0xFFFF;
    QEI1GECH = 0xFFFF;    
    QEI1CONbits.QEIEN = 1; // Enable QEI Module 
}

void InitQEI2(void)
{
    QEI2IOCbits.SWPAB = 1; //QEAx and QEBx are not swapped 
    QEI2GECL = 0xFFFF;
    QEI2GECH = 0xFFFF;
    QEI2CONbits.QEIEN = 1; // Enable QEI Module 
}

long GetQEI1RawValue(void)
{
    long QEI1RawValue = POS1CNTL;
    QEI1RawValue += ((long)POS1HLD<<16);
    return QEI1RawValue;
}

long GetQEI2RawValue(void)
{
    long QEI2RawValue = POS2CNTL;
    QEI2RawValue += ((long)POS2HLD<<16);
    return QEI2RawValue;
}