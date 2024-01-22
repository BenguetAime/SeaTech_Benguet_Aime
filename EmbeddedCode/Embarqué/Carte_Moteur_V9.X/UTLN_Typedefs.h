/* 
 * File:   ustv_typedefs.h
 * Author: Val
 *
 * Created on 10 f�vrier 2014, 13:38
 */

#ifndef USTV_TYPEDEFS_H
#define	USTV_TYPEDEFS_H

#include<libq.h>
#include <stddef.h>
#include <stdbool.h>

#define BOOL unsigned char
typedef unsigned char SPI_DATA_TYPE;
/* ------------------------------------------------------------------------------------------------
 *                                               Types
 * ------------------------------------------------------------------------------------------------
 */
typedef signed   char   int8;
typedef unsigned char   uint8;

typedef signed   short  int16;
typedef unsigned short  uint16;

typedef signed   long   int32;
typedef unsigned long   uint32;

typedef signed   char   int8_t;
typedef unsigned char   uint8_t;

#ifndef int16_t
typedef signed   short  int16_t;
#define int16_t int16_t
#endif
#ifndef uint16_t
typedef unsigned short  uint16_t;
#define uint16_t uint16_t
#endif


typedef signed   long   int32_t;
typedef unsigned long   uint32_t;



typedef struct GPS_DATA
{
    char ggaLatitude[9];
    char ggaLongitude[10];
    char ggaNsIndicator;
    char ggaEwIndicator;
    char ggaPfi;
    char vtgSpeedKmh[5];
    char vtgCapDegreTrue[5];
    unsigned long longTimeStamp;
}GpsData;





typedef struct ALTITUDE_DATA
{
    unsigned long altitude;
    unsigned int temperature;
    unsigned long longTimeStamp;
}AltitudeData;

typedef struct Data_XYZ_struct {
    double X;
    double Y;
    double Z;
}DataXYZ;

typedef struct DATAXYZ_UL_DATA
{
    int X;
    int Y;
    int Z;
    unsigned long longTimeStamp;
}DataXYZ_uL;

typedef struct Data_XYZ_Q16_struct {
    _Q16 X;
    _Q16 Y;
    _Q16 Z;
}_Q16DataXYZ;

typedef struct Data_Q16_struct {
    _Q16 Value;
}_Q16Data;



/* ------------------------------------------------------------------------------------------------
 *                                        Standard Defines
 * ------------------------------------------------------------------------------------------------
 */
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#endif	/* USTV_TYPEDEFS_H */

