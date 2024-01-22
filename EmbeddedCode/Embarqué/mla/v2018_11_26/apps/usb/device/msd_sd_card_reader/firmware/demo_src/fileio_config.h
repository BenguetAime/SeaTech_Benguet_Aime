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

#ifndef _FS_DEF_
#define _FS_DEF_

// Macro indicating how many drives can be mounted simultaneously.
#define FILEIO_CONFIG_MAX_DRIVES        1

// Delimiter for directories.
#define FILEIO_CONFIG_DELIMITER '/'

// Macro defining the maximum supported sector size for the FILEIO module.  This value should always be 512 , 1024, 2048, or 4096 bytes.
// Most media uses 512-byte sector sizes.
// Note: If interfacing with an SD-card, the value 512 should always be used, as
// this is the only value that all v1 and v2 spec cards are required to support
// (they optionally may support other sizes, but for universal compatibility, 512
// needs to be used).
#define FILEIO_CONFIG_MEDIA_SECTOR_SIZE 		512


// Definition needed when interface with an SD/MMC card via the SPI interface 
#define USE_SD_INTERFACE_WITH_SPI

// Definition for using software polling of the media to determine presense.
// If MEDIA_SOF_DETECT is not defined, then it is assumed that the media
// socket is providing a card detect I/O pin signal instead.
//#define MEDIA_SOFT_DETECT


/* *******************************************************************************************************/
/************** Compiler options to enable/Disable Features based on user's application ******************/
/* *******************************************************************************************************/

// Uncomment FILEIO_CONFIG_FUNCTION_SEARCH to disable the functions used to search for files.
//#define FILEIO_CONFIG_SEARCH_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_WRITE to disable the functions that write to a drive.  Disabling this feature will
// force the file system into read-only mode.
//#define FILEIO_CONFIG_WRITE_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_FORMAT to disable the function used to format drives.
#define FILEIO_CONFIG_FORMAT_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_DIRECTORY to disable use of directories on your drive.  Disabling this feature will
// limit you to performing all file operations in the root directory.
//#define FILEIO_CONFIG_DIRECTORY_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_PROGRAM_MEMORY_STRINGS to disable functions that accept ROM string arguments.
// This is only necessary on PIC18 parts.
#define FILEIO_CONFIG_PROGRAM_MEMORY_STRINGS_DISABLE

// Uncomment FILEIO_CONFIG_FUNCTION_DRIVE_PROPERTIES to disable the FILEIO_DrivePropertiesGet function.  This function
// will determine the properties of your device, including unused memory.
//#define FILEIO_CONFIG_DRIVE_PROPERTIES_DISABLE

// Uncomment FILEIO_CONFIG_MULTIPLE_BUFFER_MODE_DISABLE to disable multiple buffer mode.  This will force the library to
// use a single instance of the FAT and Data buffer.  Otherwise, it will use one FAT buffer and one data buffer per drive
// (defined by FILEIO_CONFIG_MAX_DRIVES).  If you are only using one drive in your application, this option has no effect.
//#define FILEIO_CONFIG_MULTIPLE_BUFFER_MODE_DISABLE


#endif
