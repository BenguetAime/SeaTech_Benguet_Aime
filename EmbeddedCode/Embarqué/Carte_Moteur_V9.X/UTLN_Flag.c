#include "UTLN_Flag.h"


volatile FLAG_SYSTEM_bits flagSystem;
volatile FLAG_SYSTEM_ERROR_bits flagError;
volatile FLAG_INTERFACE_STATE_bits flagInterface;

void InitFlags(void)
{
    flagSystem.ALL = 0x00;
    flagError.ALL= 0x0000;
    
    flagInterface.ALL = 0x00;
}
