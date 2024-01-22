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

#include <xc.h>

/* This address defines the starting addres of the user memory. */
#define BOOT_CONFIG_USER_MEMORY_START_ADDRESS                   0x00002000

#define BOOT_ERASE_BLOCK_SIZE                                   2048    //Number of flash memory addresses (containing a 16-bit WORD [or 8-bit implemented + 8-bit unimplemented WORD for odd addresses])

/* Defines the first page to start erasing.  We will set this to the
 * first block of user program memory, but in some applications the user
 * might want to change this so there is a block of user memory that
 * doesn't ever get changed when a new app is loaded (serial number,
 * board specific calibration data, etc.).  This option allows the user
 * to make such a region at the start of the memory. */
#define	BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_START                (BOOT_CONFIG_USER_MEMORY_START_ADDRESS/BOOT_ERASE_BLOCK_SIZE)


#define NVM_WRITE_LATCH_STARTING_ADDRESS    0xFA0000        //Location of the write latches on these devices


#if defined(__dsPIC33EP512MU806__) || defined(__dsPIC33EP512MU810__) || defined(__dsPIC33EP512MU814__) || defined(__PIC24EP512GU810__) || defined(__PIC24EP512GU814__)
    /* This address defines the address at which programming ends (NOTE: this
     * address does not get programmed as it is the address where programming
     * ends).  This address must be word aligned.  This option is for if the
     * config words are not going to be programmed.
     */
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_NO_CONFIGS      0x00055800

    /* This address defines the address at which programming ends (NOTE: this
     * address does not get programmed as it is the address where programming
     * ends).  This address must be word aligned.  This option is for if the
     * config words are going to be programmed.
     */
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_CONFIGS         0x00055800

    #define BOOT_MEMORY_CONFIG_START_ADDRESS                    0x00F80000
    #define BOOT_MEMORY_CONFIG_END_ADDRESS                      0x00F80014

#elif defined(__dsPIC33EP256MU806__) ||  defined(__dsPIC33EP256MU810__) || defined(__dsPIC33EP256MU814__) || defined(__PIC24EP256GU810__) || defined(__PIC24EP256GU814__)
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_NO_CONFIGS      0x0002AC00
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_CONFIGS         0x0002AC00
    #define BOOT_MEMORY_CONFIG_START_ADDRESS                    0x00F80000
    #define BOOT_MEMORY_CONFIG_END_ADDRESS                      0x00F80014
#else
    #error "This build configuration only covers devices in the dsPIC33EP512MU810 family.  If using a different device, either add new/appropriate values for your device, or use a different boot.h file that may already have the appropriate definitions."
#endif




#define BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_END_NO_CONFIGS       ((BOOT_CONFIG_USER_MEMORY_END_ADDRESS_NO_CONFIGS/BOOT_ERASE_BLOCK_SIZE)-1)
#define BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_END_CONFIGS          BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_END_NO_CONFIGS   //Same address on dsPIC33EP512MU810 family devices.  Config words are stored in separate address range.
