#include "UTLN_LS7366R.h"
#include "UTLN_SPI.h"


bool LS7366Init(uint8 counterID)
{
    uint8 commands[2];
    uint8 commands2[2];
    //MBR0 Reg
    commands[0]=WRITE_MDR0;
    commands[1]=QUADRX4 | FREE_RUN | DISABLE_INDX ;
    SPI1WriteMultipleCommand( commands, 2, counterID );
    
    //MBR1 Reg
    commands2[0]=WRITE_MDR1;
    commands2[1]=BYTE_4 | EN_CNTR | NO_FLAGS ;
    SPI1WriteMultipleCommand( commands2, 2, counterID );
    
    uint8 readBackMDR0[1];
    SPI1WriteSingleCommandMultipleRead(READ_MDR0, readBackMDR0, 1, counterID, 0);
    uint8 readBackMDR1[1];
    SPI1WriteSingleCommandMultipleRead(READ_MDR1, readBackMDR1, 1, counterID, 0);
    
    if((readBackMDR0[0]!=commands[1])|| (readBackMDR1[0]!=commands2[1]))
    {
        return false;
    }
    return true;

}

int32 LS7366ReadCnt(uint8 counterID)
{
    uint8 commands[2];
    uint8 results[4];
    commands[0]=READ_CNTR;
    SPI1WriteSingleCommandMultipleRead(commands[0], results, 4,  counterID, 0);
    int32 val=BUILD_INT32(results[3],results[2],results[1],results[0]);
    return val;
}

uint8 LS7366ReadStatus(uint8 counterID)
{
    uint8 commands[2];
    uint8 results[1];
    commands[0]=READ_STR;
    SPI1WriteSingleCommandMultipleRead(commands[0], results, 1,  counterID, 0);
    return results[1];
}

void LS7366ClearCounter(uint8 counterID)
{
    uint8 commands[2];
    //MBR0 Reg
    commands[0]=CLR_CNTR;
    SPI1WriteMultipleCommand( commands, 1, counterID );
}

void LS7366WriteDTR32bits(uint32 value,uint8 counterID)
{
    uint8 commands[2];
    //MBR0 Reg
    commands[0]=WRITE_DTR;
    commands[1]=BREAK_UINT32(value,3);
    commands[2]=BREAK_UINT32(value,3);
    commands[3]=BREAK_UINT32(value,3);
    commands[4]=BREAK_UINT32(value,3);
    SPI1WriteMultipleCommand( commands, 5, counterID );
}