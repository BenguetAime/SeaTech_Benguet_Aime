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

/* This address defines the starting address of the user memory. */
#define BOOT_CONFIG_USER_MEMORY_START_ADDRESS                   0x00002000

#define BOOT_ERASE_BLOCK_SIZE                                   2048    //Number of flash memory addresses (containing a 16-bit WORD [or 8-bit implemented + 8-bit unimplemented WORD for odd addresses])

/* Defines the first page to start erasing.  We will set this to the
 * first block of user program memory, but in some applications the user
 * might want to change this so there is a block of user memory that
 * doesn't ever get changed when a new app is loaded (serial number,
 * board specific calibration data, etc.).  This option allows the user
 * to make such a region at the start of the memory. */
#define	BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_START                (BOOT_CONFIG_USER_MEMORY_START_ADDRESS/BOOT_ERASE_BLOCK_SIZE)


#define NVM_WRITE_LATCH_STARTING_ADDRESS    0x00FA0000        //Location of the write latches on these devices


#if defined(__PIC24FJ1024GB610__) || defined(__PIC24FJ1024GB606__)
    /* This address defines the address at which programming ends (NOTE: this
     * address does not get programmed as it is the address where programming
     * ends).  This address must be word aligned.  This option is for if the
     * config words are not going to be programmed.
     */
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_NO_CONFIGS      0x000AB800 

    /* This address defines the address at which programming ends (NOTE: this
     * address does not get programmed as it is the address where programming
     * ends).  This address must be word aligned.  This option is for if the
     * config words are going to be programmed.
     */
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_CONFIGS         0x000ABF00 

    #define BOOT_MEMORY_CONFIG_START_ADDRESS                    0x000ABF00
    #define BOOT_MEMORY_CONFIG_FSIGN_ADDRESS                    0x000ABF14
    #define BOOT_MEMORY_CONFIG_END_ADDRESS                      0x000ABF30

#elif defined(__PIC24FJ512GB610__) || defined(__PIC24FJ512GB606__)
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
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_CONFIGS         0x00055F00 

    #define BOOT_MEMORY_CONFIG_START_ADDRESS                    0x00055F00
    #define BOOT_MEMORY_CONFIG_FSIGN_ADDRESS                    0x00055F14
    #define BOOT_MEMORY_CONFIG_END_ADDRESS                      0x00055F30

#elif defined(__PIC24FJ256GB610__) || defined(__PIC24FJ256GB606__)
    /* This address defines the address at which programming ends (NOTE: this
     * address does not get programmed as it is the address where programming
     * ends).  This address must be word aligned.  This option is for if the
     * config words are not going to be programmed.
     */
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_NO_CONFIGS      0x0002A800 

    /* This address defines the address at which programming ends (NOTE: this
     * address does not get programmed as it is the address where programming
     * ends).  This address must be word aligned.  This option is for if the
     * config words are going to be programmed.
     */
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_CONFIGS         0x0002AF00 

    #define BOOT_MEMORY_CONFIG_START_ADDRESS                    0x0002AF00
    #define BOOT_MEMORY_CONFIG_FSIGN_ADDRESS                    0x0002AF14
    #define BOOT_MEMORY_CONFIG_END_ADDRESS                      0x0002AF30

#elif defined(__PIC24FJ128GB610__) || defined(__PIC24FJ128GB606__)
    /* This address defines the address at which programming ends (NOTE: this
     * address does not get programmed as it is the address where programming
     * ends).  This address must be word aligned.  This option is for if the
     * config words are not going to be programmed.
     */
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_NO_CONFIGS      0x00015800 

    /* This address defines the address at which programming ends (NOTE: this
     * address does not get programmed as it is the address where programming
     * ends).  This address must be word aligned.  This option is for if the
     * config words are going to be programmed.
     */
    #define BOOT_CONFIG_USER_MEMORY_END_ADDRESS_CONFIGS         0x00015F00 

    #define BOOT_MEMORY_CONFIG_START_ADDRESS                    0x00015F00
    #define BOOT_MEMORY_CONFIG_FSIGN_ADDRESS                    0x00015F14
    #define BOOT_MEMORY_CONFIG_END_ADDRESS                      0x00015F30

#else
    #error "This file only covers the PIC24F1024GB610 family devices.  Please see another folder for the bootloader appropriate for the selected device."
#endif

#define BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_END_NO_CONFIGS       (BOOT_CONFIG_USER_MEMORY_END_ADDRESS_NO_CONFIGS/BOOT_ERASE_BLOCK_SIZE)
#define BOOT_CONFIG_USER_MEMORY_ERASE_PAGE_END_CONFIGS          (1ul + (BOOT_MEMORY_CONFIG_END_ADDRESS/BOOT_ERASE_BLOCK_SIZE))
