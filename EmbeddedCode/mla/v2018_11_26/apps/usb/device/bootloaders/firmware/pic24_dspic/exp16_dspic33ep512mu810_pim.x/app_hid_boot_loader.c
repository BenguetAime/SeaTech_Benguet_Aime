/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

/** INCLUDES *******************************************************/

#include <usb.h>
#include <usb_device_hid.h>
#include <boot.h>


/** C O N S T A N T S **********************************************************/

//Bootloader commands that this firmware implements (which the host PC application may issue to us to perform the firmware update process)
#define	QUERY_DEVICE				0x02    //Command that the host uses to learn about the device (what regions can be programmed, and what type of memory is the region)
#define	UNLOCK_CONFIG				0x03    //Note, this command is used for both locking and unlocking the config bits (see the "//Unlock Configs Command Definitions" below)
#define ERASE_DEVICE				0x04    //Host sends this command to start an erase operation.  Firmware controls which pages should be erased.
#define PROGRAM_DEVICE				0x05    //If host is going to send a full REQUEST_DATA_BLOCK_SIZE to be programmed, it uses this command.
#define	PROGRAM_COMPLETE			0x06    //If host send less than a REQUEST_DATA_BLOCK_SIZE to be programmed, or if it wished to program whatever was left in the buffer, it uses this command.
#define GET_DATA                    0x07    //The host sends this command in order to read out memory from the device.  Used during verify (and read/export hex operations)
#define	RESET_DEVICE				0x08    //Resets the microcontroller, so it can update the config bits (if they were programmed, and so as to leave the bootloader (and potentially go back into the main application)

//Unlock Configs Command Definitions
#define UNLOCKCONFIG				0x00    //Sub-command for the ERASE_DEVICE command
#define LOCKCONFIG                  0x01    //Sub-command for the ERASE_DEVICE command

//Query Device Response "Types" 
#define	TYPE_PROGRAM_MEMORY         0x01    //When the host sends a QUERY_DEVICE command, need to respond by populating a list of valid memory regions that exist in the device (and should be programmed)
#define TYPE_EEPROM                 0x02
#define TYPE_CONFIG_WORDS           0x03
#define	TYPE_END_OF_TYPES_LIST      0xFF    //Sort of serves as a "null terminator" like number, which denotes the end of the memory region list has been reached.


//BootState Variable States
#define	IDLE_STATE                              0x00
#define NOT_IDLE_STATE                          0x01

//OtherConstants
#define INVALID_ADDRESS                         0xFFFFFFFF

//Application and Microcontroller constants
#define BYTES_PER_FLASH_ADDRESS                 0x02    //For Flash memory: One byte per address on PIC18, two bytes per address on PIC24

#define	TOTAL_PACKET_SIZE                       0x40
#define WORDSIZE                                0x02    //PIC18 uses 2 byte instruction words, PIC24 uses 3 byte "instruction words" (which take 2 addresses, since each address is for a 16 bit word; the upper word contains a "phantom" byte which is unimplemented.).

#define REQUEST_DATA_BLOCK_SIZE                 56      //Number of bytes in the "Data" field of a standard request to/from the PC.  Note: Must be an even number from 2 to 56.
#define BUFFER_SIZE                             0x20    //32 16-bit words of buffer

#define CORRECT_NVM_ACESS_KEY                   0x600D      //"Good"
#define CORRECT_BOOTLOADER_INIT_KEY             0x1337BADA  //Locate in special RAM variable, as double check to make sure bootloader was properly initialized, and not inadvertently jumped into due to buggy code or overclocking.



/** PRIVATE PROTOTYPES *********************************************/
static void EraseFlash(uint16_t erasePage, uint16_t unlockKey);
static void WriteFlashSubBlock(uint16_t unlockKey);
uint32_t ReadProgramMemory(uint32_t);
bool CheckIfVoltageIsAdequateForSafeBootloaderOperation(void);
void InitNVMWrite(uint16_t unlockKey);
void APP_HIDBootloaderShutdown(void);

/** T Y P E  D E F I N I T I O N S ************************************/

//Note: The Data[] field is accessed with integer pointers.  Therefore, the location must be aligned on a 2 byte (even address) boundary, to ensure proper operation.
typedef union __attribute__ ((packed,aligned(2))) _USB_HID_BOOTLOADER_COMMAND
{
    unsigned char Contents[64];

    struct __attribute__ ((packed))
    {
        unsigned char Command;
        uint16_t AddressHigh;
        uint16_t AddressLow;
        unsigned char Size;
        unsigned char PadBytes[(TOTAL_PACKET_SIZE - 6) - (REQUEST_DATA_BLOCK_SIZE)];
        unsigned int Data[REQUEST_DATA_BLOCK_SIZE/WORDSIZE];
    };

    //PROGRAM_DEVICE and GET_DATA packet formatting
    struct __attribute__ ((packed))
    {
        unsigned char Command;
        uint32_t Address;
        unsigned char Size;
        unsigned char PadBytes[(TOTAL_PACKET_SIZE - 6) - (REQUEST_DATA_BLOCK_SIZE)];
        unsigned int Data[REQUEST_DATA_BLOCK_SIZE/WORDSIZE];
    };

    //QUERY_DEVICE response packet format
    struct __attribute__ ((packed))
    {
        unsigned char Command;
        unsigned char PacketDataFieldSize;
        unsigned char BytesPerAddress;
        unsigned char Type1;
        unsigned long Address1;
        unsigned long Length1;
        unsigned char Type2;
        unsigned long Address2;
        unsigned long Length2;
        unsigned char Type3;		//End of sections list indicator goes here, when not programming the vectors, in that case fill with 0xFF.
        unsigned long Address3;
        unsigned long Length3;
        unsigned char Type4;		//End of sections list indicator goes here, fill with 0xFF.
        unsigned char ExtraPadBytes[33];
    };

    //UNLOCK_CONFIG packet format
    struct __attribute__ ((packed))
    {						
        unsigned char Command;
        unsigned char LockValue;
    };
} PacketToFromPC;

typedef union
{
    uint32_t Val;
    uint16_t w[2] __attribute__((packed));
    uint8_t v[4];
    struct __attribute__((packed))
    {
        uint16_t LW;
        uint16_t HW;
    } word;
    struct __attribute__((packed))
    {
        uint8_t LB;
        uint8_t HB;
        uint8_t UB;
        uint8_t MB;
    } byte;
} uint32_t_VAL;

/** VARIABLES ******************************************************/
PacketToFromPC PacketFromPC;		//64 byte buffer for receiving packets on EP1 OUT from the PC
PacketToFromPC PacketToPC;			//64 byte buffer for sending packets on EP1 IN to the PC
PacketToFromPC PacketFromPCBuffer;

USB_HANDLE USBOutHandle = 0;
USB_HANDLE USBInHandle = 0;
bool blinkStatusValid = true;

uint16_t MaxPageToErase = BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_END_NO_CONFIGS;
unsigned long ProgramMemStopAddress;
unsigned char BootState;
uint16_t ErasePageTracker;
unsigned int ProgrammingBuffer[BUFFER_SIZE + 4];
uint16_t BufferedDataIndex;
unsigned long ProgrammedPointer;
unsigned char ConfigsProtected = LOCKCONFIG;
uint32_t bootloaderProperInitKey = 0;


void APP_HIDBootLoaderInitialize(void)
{   
    //Don't allow most bootloader operations (namely the "unsafe" ones that can modify
    //program memory contents), until the proper PC application connects
    //up to the firmware and issues the appropriate command (ex: QUERY_DEVICE) to unlock the device
    //and allow normal bootloader operation.
    APP_HIDBootloaderShutdown();    
    
    //initialize the variable holding the handle for the last
    // transmission
    USBOutHandle = 0;
    USBInHandle = 0;

    //Initialize bootloader state variables
    MaxPageToErase = BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_END_NO_CONFIGS;		//Assume we will not allow erase/programming of config words (unless host sends override command)
    ProgramMemStopAddress = BOOT_CONFIG_USER_MEMORY_END_ADDRESS_NO_CONFIGS;
    ConfigsProtected = LOCKCONFIG;					//Assume we will not erase or program the vector table at first.  Must receive unlock config bits/vectors command first.
    BootState = IDLE_STATE;
    ProgrammedPointer = INVALID_ADDRESS;
    BufferedDataIndex = 0;

    //enable the HID endpoint
    USBEnableEndpoint(CUSTOM_DEVICE_HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    //Arm the OUT endpoint for the first packet
    USBOutHandle = HIDRxPacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&PacketFromPCBuffer,64);
}//end UserInit



/******************************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user routines.
 *                  It is a mixture of both USB and non-USB tasks.
 *
 * Note:            None
 *****************************************************************************/
void APP_HIDBootLoaderTasks(void)
{
    unsigned char i;
    volatile unsigned int j;
    uint32_t_VAL FlashMemoryValue;

    if(BootState == IDLE_STATE)
    {
        //Are we done sending the last response?  We need to be before we 
        //  receive the next command because we clear the PacketToPC buffer
        //  once we receive a command
        if(!USBHandleBusy(USBInHandle))
        {
            if(!USBHandleBusy(USBOutHandle))		//Did we receive a command?
            {
                //We just received a packet from the PC.  Copy the packet and then
                //re-arm the USB endpoint so it can receive the next packet from the host.
                for(i = 0; i < TOTAL_PACKET_SIZE; i++)
                {
                    PacketFromPC.Contents[i] = PacketFromPCBuffer.Contents[i];
                }
                
                USBOutHandle = USBRxOnePacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&PacketFromPCBuffer,64);
                BootState = NOT_IDLE_STATE;     //We have a command that needs processing, currently sitting in the PacketFromPC[] buffer.
                
                //Prepare the next packet we will send to the host, by initializing the entire packet to 0x00.	
                for(i = 0; i < TOTAL_PACKET_SIZE; i++)
                {
                    //This saves code space, since we don't have to do it independently in the QUERY_DEVICE and GET_DATA cases.
                    PacketToPC.Contents[i] = 0;	
                }
            }
        }
    }
    else //(BootState must be in NotIdleState)
    {
        switch(PacketFromPC.Command)
        {
            case QUERY_DEVICE:
            {
                //Prepare a response packet, which lets the PC software know about the memory ranges of this device.
                PacketToPC.Command = (unsigned char)QUERY_DEVICE;
                PacketToPC.PacketDataFieldSize = (unsigned char)REQUEST_DATA_BLOCK_SIZE;
                PacketToPC.BytesPerAddress = (unsigned char)BYTES_PER_FLASH_ADDRESS;

                PacketToPC.Type1 = (unsigned char)TYPE_PROGRAM_MEMORY;
                PacketToPC.Address1 = (unsigned long)BOOT_CONFIG_USER_MEMORY_START_ADDRESS;
                PacketToPC.Length1 = (unsigned long)(ProgramMemStopAddress - BOOT_CONFIG_USER_MEMORY_START_ADDRESS);	//Size of program memory area
                PacketToPC.Type2 = (unsigned char)TYPE_END_OF_TYPES_LIST;

                if(ConfigsProtected == UNLOCKCONFIG)
                {
                    PacketToPC.Type2 = (unsigned char)TYPE_CONFIG_WORDS;
                    PacketToPC.Address2 = (unsigned long)BOOT_MEMORY_CONFIG_START_ADDRESS;
                    PacketToPC.Length2 = (unsigned long)(BOOT_MEMORY_CONFIG_END_ADDRESS - BOOT_MEMORY_CONFIG_START_ADDRESS);
                    PacketToPC.Type3 = (unsigned char)TYPE_END_OF_TYPES_LIST;
                }

                //Init pad bytes to 0x00...  Already done after we received the QUERY_DEVICE command (just after calling HIDRxPacket()).

                if(!USBHandleBusy(USBInHandle))
                {
                    USBInHandle = USBTxOnePacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&PacketToPC,64);
                    BootState = IDLE_STATE;
                }

                //The PC application connected to us and issued an appropriate query device command.
                //Go ahead and unlock the rest of the bootloader so it can perform erase/write
                //commands when requested to do so.
                bootloaderProperInitKey = CORRECT_BOOTLOADER_INIT_KEY;
                break;
            }

            case UNLOCK_CONFIG:
            {
                if(PacketFromPC.LockValue == UNLOCKCONFIG)
                {
                        MaxPageToErase = BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_END_CONFIGS;		//Assume we will not allow erase/programming of config words (unless host sends override command)
                        ProgramMemStopAddress = BOOT_CONFIG_USER_MEMORY_END_ADDRESS_CONFIGS;
                        ConfigsProtected = UNLOCKCONFIG;
                        #pragma message "Programming of config words not currently supported in this code."
                }
                else	//LockValue must be == LOCKCONFIG
                {
                    MaxPageToErase = BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_END_NO_CONFIGS;
                    ProgramMemStopAddress = BOOT_CONFIG_USER_MEMORY_END_ADDRESS_NO_CONFIGS;
                    ConfigsProtected = LOCKCONFIG;
                }
                
                BootState = IDLE_STATE;
                break;
            }

            case ERASE_DEVICE:
            {                
                //For this command, we need to erase all of the application program memory space.
                for(ErasePageTracker = BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_START; ErasePageTracker < (MaxPageToErase + 1); ErasePageTracker++)
                {
                    ClrWdt();           //Periodically clear WDT in case the application enables it, since we don't want a WDT timeout during the flash erase operation (which takes awhile and blocks the CPU).
                    EraseFlash(ErasePageTracker, CORRECT_NVM_ACESS_KEY);
                    USBDeviceTasks(); 	//Call USBDeviceTasks() periodically to prevent falling off the bus if any SETUP packets should happen to arrive.
                }

                BootState = IDLE_STATE;
                break;
            }

            case PROGRAM_DEVICE:
            {
                //Check if we are starting from a new address region (rather than continuing a contiguous program memory region started from a prior PROGRAM_DEVICE command)
                if(ProgrammedPointer == (unsigned long)INVALID_ADDRESS)
                {
                    ProgrammedPointer = PacketFromPC.Address;
                    BufferedDataIndex = 0;
                }

                if(ProgrammedPointer == (unsigned long)PacketFromPC.Address)
                {
                    for(i = 0; i < (PacketFromPC.Size / WORDSIZE); i++)
                    {
                        ProgrammingBuffer[BufferedDataIndex] = PacketFromPC.Data[(REQUEST_DATA_BLOCK_SIZE - PacketFromPC.Size) / WORDSIZE + i];	//Data field is right justified.  Need to put it in the buffer left justified.
                        BufferedDataIndex++;
                        ProgrammedPointer++;
                        if(BufferedDataIndex >= (WORDSIZE * 2))	//Need to make sure it doesn't call WriteFlashSubBlock() unless BufferedDataIndex/2 is an integer
                        {
                            WriteFlashSubBlock(CORRECT_NVM_ACESS_KEY);
                        }
                    }
                }
                //else host sent us a non-contiguous packet address...  to make this firmware simpler, host should not do this without sending a PROGRAM_COMPLETE command in between program sections.
                BootState = IDLE_STATE;
                break;
            }

            case PROGRAM_COMPLETE:
            {
                //Make sure we always do a full double instruction word write.
                //If we aren't already on a double instruction boundary, pad the 
                //extra locations to reach the next boundary with 0xFFFF.
                while((BufferedDataIndex % 4) != 0)
                {
                    ProgrammingBuffer[BufferedDataIndex++] = 0xFFFF;
                    ProgrammedPointer++;
                }
                //Commit the data in the ProgrammingBuffer[] RAM buffer to flash memory.
                WriteFlashSubBlock(CORRECT_NVM_ACESS_KEY);
                ProgrammedPointer = INVALID_ADDRESS;		//Reinitialize pointer to an invalid range, so we know the next PROGRAM_DEVICE will be the start address of a contiguous section.
                BootState = IDLE_STATE;
                break;
            }

            case GET_DATA:  //This command is primarily used by the PC application for post-programming verification purposes.
            {
                //Make sure we have control over the USB endpoint buffer, before loading it with the newly requested response packet data.
                if(!USBHandleBusy(USBInHandle))
                {
                    //Prepare a response packet to the USB host PC application, with the requested NVM data read from the device.
                    
                    //Init pad bytes to 0x00...  Already done after we received the QUERY_DEVICE command (just after calling HIDRxReport()).
                    PacketToPC.Command = GET_DATA;
                    PacketToPC.Address = PacketFromPC.Address;
                    PacketToPC.Size = PacketFromPC.Size;

                    //Load the data field with data read from the NVM.
                    for(i = 0; i < (PacketFromPC.Size / 2); i=i+2)
                    {
                        FlashMemoryValue.Val = ReadProgramMemory(PacketFromPC.Address + i);
                        PacketToPC.Data[REQUEST_DATA_BLOCK_SIZE/WORDSIZE + i - PacketFromPC.Size/WORDSIZE] = FlashMemoryValue.word.LW;		//Low word, pure 16-bits of real data
                        FlashMemoryValue.byte.MB = 0x00;	//Set the "phantom byte" = 0x00, since this is what is in the .HEX file generated by MPLAB.
                        PacketToPC.Data[REQUEST_DATA_BLOCK_SIZE/WORDSIZE + i + 1 - PacketFromPC.Size/WORDSIZE] = FlashMemoryValue.word.HW;	//Upper word, which contains the phantom byte
                    }

                    //Let the USB module send the packet now, the at the next IN token read request opportunity from the host.
                    USBInHandle = USBTxOnePacket(CUSTOM_DEVICE_HID_EP,(uint8_t*)&PacketToPC.Contents[0],64);
                    //Finished processing this command, go back to idle state ready to process the next command that may arrive from the host.
                    BootState = IDLE_STATE;
                }
                break;
            }

                        
            case RESET_DEVICE:
            {
                //Disable USB module and wait awhile for the USB cable capacitance to discharge down to disconnected (SE0) state.
                //Otherwise the host might not realize we disconnected/reconnected when we do the reset.
                //A delay of at least 80+ms is recommended, before re-attaching to the USB host, to ensure
                //the host had enough time to process any plug and play event handlers and become ready for re-attachment.
                for(j = 0; j < 0xFFFF; j++)
                {
                    APP_HIDBootloaderShutdown();
                    U1CON = 0x0000;				//Disable USB module
                }
                asm("reset");
                break;
            }
        }//End switch
    }//End if/else

}//End ProcessIO()



//Erases a page of flash memory, with index of erasePage.  Note: In order to use this
//function, the unlockKey value passed to this function must be == CORRECT_NVM_ACESS_KEY.
static void EraseFlash(uint16_t erasePage, uint16_t unlockKey)
{
    uint16_t tblpagSave;
    uint32_t_VAL MemAddressToErase;
    MemAddressToErase.Val = (((uint32_t)erasePage) * BOOT_ERASE_BLOCK_SIZE);

    //Verify the address isn't part of the bootloader itself.
    if(MemAddressToErase.Val < (uint32_t)BOOT_CONFIG_USER_MEMORY_START_ADDRESS)
    {
        //Someone called this function inappropriately.  Return without doing
        //any actual erasure, since the address is invalid, and corresponds with part
        //of this bootloader code itself.
        return;
    }

	NVMCON = 0x4003;				//Page erase on next WR->1 event

    NVMADRU = MemAddressToErase.w[1];
    NVMADR =  MemAddressToErase.w[0];

    tblpagSave = TBLPAG;
    TBLPAG = NVM_WRITE_LATCH_STARTING_ADDRESS >> 16;
    __builtin_tblwtl((uint16_t)NVM_WRITE_LATCH_STARTING_ADDRESS, 0xFFFF);
    __builtin_tblwth((uint16_t)(NVM_WRITE_LATCH_STARTING_ADDRESS+1), 0xFFFF);

    InitNVMWrite(unlockKey);
    while(NVMCONbits.WR == 1){}

    TBLPAG = tblpagSave;

    NVMCONbits.WREN = 0;		//Good practice to clear WREN bit anytime we are not expecting to do erase/write operations, further reducing probability of accidental activation.
}	


//Use double instructions word writes (ex: 4 addresses = 8 bytes)to commit data from the ProgrammingBuffer[] RAM buffer 
//to the flash memory.  Note: The unlockKey must be == CORRECT_NVM_ACESS_KEY for the operation to complete successfully.
static void WriteFlashSubBlock(uint16_t unlockKey)
{
    uint16_t tblpagSave;
    unsigned int i = 0;
    uint32_t_VAL Address;

	NVMCON = 0x4001;		//Perform double instruction word write (ex: 4 word addresses) next time WR gets set = 1.

    tblpagSave = TBLPAG;

    while(BufferedDataIndex != 0)		//While data is still in the buffer.
    {
        Address.Val = ProgrammedPointer - BufferedDataIndex;

        //Verify the address is actually part of the application program memory 
        //region, and not part of this bootloader.
        if(Address.Val >= BOOT_CONFIG_USER_MEMORY_START_ADDRESS)
        {
            //Address looks good.  Go ahead and use it.
            NVMADRU = Address.word.HW;
            NVMADR =  Address.word.LW;

            //Write the prog word (24-bits) to the programming latches
            TBLPAG = NVM_WRITE_LATCH_STARTING_ADDRESS >> 16;
            __builtin_tblwtl((uint16_t)(NVM_WRITE_LATCH_STARTING_ADDRESS+0), ProgrammingBuffer[i + 0]);	//Write the low word to the latch
            __builtin_tblwth((uint16_t)(NVM_WRITE_LATCH_STARTING_ADDRESS+1), ProgrammingBuffer[i + 1]);	//Write the high word to the latch (8 bits of data + 8 bits of "phantom data")
            __builtin_tblwtl((uint16_t)(NVM_WRITE_LATCH_STARTING_ADDRESS+2), ProgrammingBuffer[i + 2]);	//Write the second low word to the latch
            __builtin_tblwth((uint16_t)(NVM_WRITE_LATCH_STARTING_ADDRESS+3), ProgrammingBuffer[i + 3]);	//Write the second high word to the latch (8 bits of data + 8 bits of "phantom data")
            i = i + 4;

            //Commit the programming latch contents into the actual flash memory (at the NVMADRU:NVMADR address).
            InitNVMWrite(unlockKey);
            while(NVMCONbits.WR == 1){}
        }

        BufferedDataIndex -= 4;		//Used up 4 (16-bit) words from the buffer.
    }

    NVMCONbits.WREN = 0;		//Good practice to clear WREN bit anytime we are not expecting to do erase/write operations, further reducing probability of accidental activation.
    TBLPAG = tblpagSave;
}


void APP_HIDBootloaderShutdown(void)
{
    //Deliberately set the special RAM value to something other than for bootloader operation.
    //This reduces chances of application code inapproparitely jumping into the bootloader
    //and executing some rogue erase/write operations.
    bootloaderProperInitKey = ~CORRECT_BOOTLOADER_INIT_KEY;
}



void InitNVMWrite(uint16_t unlockKey)
{
    if((unlockKey != CORRECT_NVM_ACESS_KEY) || (bootloaderProperInitKey != CORRECT_BOOTLOADER_INIT_KEY))
    {
        //Error, should never get here.  If we do, it means someone is calling or otherwise
        //jumping to this function improperly (ex: due to buggy code, such as misuse of function pointers,
        //or by violating the voltage versus frequency graph in the datasheet causing microcontroller
        //overclocking.
        //Do not initiate the NVM write/erase operation.  Instead, do some kind of safe shutdown.
        while(1)
        {
            Sleep();
        }
    }

    if(CheckIfVoltageIsAdequateForSafeBootloaderOperation() == true)
    {
        asm("DISI #16");					//Disable interrupts for next few instructions for unlock sequence
        __builtin_write_NVM();
    }
    else
    {
        //Insufficient voltage for safe bootloader.  Should perform safe shutdown.
        while(1)
        {
            Sleep();
        }
    }
}

bool CheckIfVoltageIsAdequateForSafeBootloaderOperation(void)
{
    #pragma message "Recommended to implement code here to check VDD and make sure that it is well within published VDD versus frequency graph."

    //If the voltage is insufficient for safe operation at the current frequency
    //and for NVM write/erase operations, return false.  Otherwise return true.
    return true;
}


/*********************************************************************
 * Function:        uint32_t ReadProgramMemory(uint32_t address)
 *
 * PreCondition:    None
 *
 * Input:           Program memory address to read from.  Should be 
 *                            an even number.
 *
 * Output:          Program word at the specified address.  For the 
 *                            PIC24, dsPIC, etc. which have a 24 bit program 
 *                            word size, the upper byte is 0x00.
 *
 * Side Effects:    None
 *
 * Overview:        Modifies and restores TBLPAG.  Make sure that if 
 *                            using interrupts and the PSV feature of the CPU 
 *                            in an ISR that the TBLPAG register is preloaded 
 *                            with the correct value (rather than assuming 
 *                            TBLPAG is always pointing to the .const section.
 *
 * Note:            None
 ********************************************************************/
uint32_t ReadProgramMemory(uint32_t address)
{  
    uint32_t_VAL dwvResult;
    uint16_t wTBLPAGSave;
 
    wTBLPAGSave = TBLPAG;
    TBLPAG = (uint16_t)(address>>16);

    dwvResult.w[1] = __builtin_tblrdh((uint16_t)address);
    dwvResult.w[0] = __builtin_tblrdl((uint16_t)address);
    TBLPAG = wTBLPAGSave;
 
    return dwvResult.Val;
}

