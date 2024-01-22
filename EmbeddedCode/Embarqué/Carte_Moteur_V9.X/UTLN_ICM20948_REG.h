/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File: UTLN_ICM20948.c   
 * Author: Valentin BARCHASZ
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UTLN_ICM20948_REG_H
#define	UTLN_ICM20948_REG_H
#include <xc.h> // include processor files - each processor file is guarded.  

#ifndef NULL
    #define NULL 0
#endif


//Device Identifier
#define ICM20948_ID 0xEA 

//AK09916 I2C Address
#define AK09916_ADDRESS 0x0C
#define AK09916_READ_MASK(x) ((x<<1) | 0x01)
#define AK09916_WRITE_MASK(x) ((x<<1) & ~0x01)

//AK09916 Register address
#define AK09916_WHO_I_AM 0x01
#define AK09916_STATUS_1 0x02
#define AK09916_OUT_X_L 0x11
#define AK09916_OUT_X_H 0x12
#define AK09916_OUT_Y_L 0x13
#define AK09916_OUT_Y_H 0x14
#define AK09916_OUT_Z_L 0x15
#define AK09916_OUT_Z_H 0x16
#define AK09916_STATUS_2 0x18
#define AK09916_CONTROL_2 0x31
#define AK09916_CONTROL_3 0x32
#define AK09916_TEST_1 0x33
#define AK09916_TEST_2 0x34

//AK09916 CONFIG2 VALUE (MODE X)
#define AK09916_POWER_DOWN 0x00
#define AK09916_SINGLE_SHOT 0x01
#define AK09916_ODR_10HZ 0x02
#define AK09916_ODR_20HZ 0x04
#define AK09916_ODR_50HZ 0x06
#define AK09916_ODR_100HZ 0x08
#define AK09916_SELF_TEST 0x10


//Register address map for ICM20948
//USER BANK 0
#define WHO_AM_I 0x00
#define USER_CTRL 0x03
#define LP_CONFIG 0x05
#define PWR_MGMT_1 0x06
#define PWR_MGMT_2 0x07
#define INT_PIN_CFG 0x0F
#define INT_ENABLE 0x10
#define INT_ENABLE_1 0x11
#define INT_ENABLE_2 0x12
#define INT_ENABLE_3 0x13
#define I2C_MST_STATUS 0x17
#define INT_STATUS 0x19
#define INT_STATUS_1 0x1A
#define INT_STATUS_2 0x1B
#define INT_STATUS_3 0x1C
#define DELAY_TIMEH 0x28
#define DELAY_TIMEL 0x29
#define ACCEL_XOUT_H 0x2D
#define ACCEL_XOUT_L 0x2E
#define ACCEL_YOUT_H 0x2F
#define ACCEL_YOUT_L 0x30
#define ACCEL_ZOUT_H 0x31
#define ACCEL_ZOUT_L 0x32
#define GYRO_XOUT_H 0x33
#define GYRO_XOUT_L 0x34
#define GYRO_YOUT_H 0x35
#define GYRO_YOUT_L 0x36
#define GYRO_ZOUT_H 0x37
#define GYRO_ZOUT_L 0x38
#define TEMP_OUT_H 0x39
#define TEMP_OUT_L 0x3A

#define EXT_SLV_SENS_DATA_00 0x3B
#define EXT_SLV_SENS_DATA_01 0x3C
#define EXT_SLV_SENS_DATA_02 0x3D
#define EXT_SLV_SENS_DATA_03 0x3E
#define EXT_SLV_SENS_DATA_04 0x3F
#define EXT_SLV_SENS_DATA_05 0x40
#define EXT_SLV_SENS_DATA_06 0x41
#define EXT_SLV_SENS_DATA_07 0x42
#define EXT_SLV_SENS_DATA_08 0x43
#define EXT_SLV_SENS_DATA_09 0x44
#define EXT_SLV_SENS_DATA_10 0x45
#define EXT_SLV_SENS_DATA_11 0x46
#define EXT_SLV_SENS_DATA_12 0x47
#define EXT_SLV_SENS_DATA_13 0x48
#define EXT_SLV_SENS_DATA_14 0x49
#define EXT_SLV_SENS_DATA_15 0x4A
#define EXT_SLV_SENS_DATA_16 0x4B
#define EXT_SLV_SENS_DATA_17 0x4C
#define EXT_SLV_SENS_DATA_18 0x4D
#define EXT_SLV_SENS_DATA_19 0x4E
#define EXT_SLV_SENS_DATA_20 0x4F
#define EXT_SLV_SENS_DATA_21 0x50
#define EXT_SLV_SENS_DATA_22 0x51
#define EXT_SLV_SENS_DATA_23 0x52

#define FIFO_EN_1 0x66
#define FIFO_EN_2 0x67
#define FIFO_RST 0x68
#define FIFO_MODE 0x69
#define FIFO_COUNTH 0x70
#define FIFO_COUNTL 0x71
#define FIFO_R_W 0x72
#define DATA_RDY_STATUS 0x74
#define FIFO_CFG 0x76
#define REG_BANK_SEL 0x7F

//USER BANK 1
#define SELF_TEST_X_GYRO 0x02
#define SELF_TEST_Y_GYRO 0x03
#define SELF_TEST_Z_GYRO 0x04
#define SELF_TEST_X_ACCEL 0x0E
#define SELF_TEST_Y_ACCEL 0x0F
#define SELF_TEST_Z_ACCEL 0x10
#define XA_OFFS_H 0x14
#define XA_OFFS_L 0x15
#define YA_OFFS_H 0x17
#define YA_OFFS_L 0x18
#define ZA_OFFS_H 0x1A
#define ZA_OFFS_L 0x1B
#define TIMEBASE_CORRECTION_PLL 0x28
#define REG_BANK_SEL 0x7F

//USER BANK 2
#define GYRO_SMPLRT_DIV 0x00
#define GYRO_CONFIG_1 0x01
#define GYRO_CONFIG_2 0x02
#define XG_OFFS_USRH 0x03
#define XG_OFFS_USRL 0x04
#define YG_OFFS_USRH 0x05
#define YG_OFFS_USRL 0x06
#define ZG_OFFS_USRH 0x07
#define ZG_OFFS_USRL 0x08
#define ODR_ALIGN_EN 0x09
#define ACCEL_SMPLRT_DIV_1 0x10
#define ACCEL_SMPLRT_DIV_2 0x11
#define ACCEL_INTEL_CTRL 0x12
#define ACCEL_WOM_THR 0x73
#define ACCEL_CONFIG 0x14
#define ACCEL_CONFIG_2 0x15
#define FSYNC_CONFIG 0x52
#define TEMP_CONFIG 0x53
#define MOD_CTRL_USR 0x54
#define REG_BANK_SEL 0x7F

//USER BANK 3
#define I2C_MST_ODR_CONFIG 0x00
#define I2C_MST_CTRL 0x01
#define I2C_MST_DELAY_CTRL 0x02
#define I2C_SLV0_ADDR 0x03
#define I2C_SLV0_REG 0x04
#define I2C_SLV0_CTRL 0x05
#define I2C_SLV0_DO 0x06
#define I2C_SLV1_ADDR 0x07
#define I2C_SLV1_REG 0x08
#define I2C_SLV1_CTRL 0x09
#define I2C_SLV1_DO 0x0A
#define I2C_SLV2_ADDR 0x0B
#define I2C_SLV2_REG 0x0C
#define I2C_SLV2_CTRL 0x0D
#define I2C_SLV2_DO 0x0E
#define I2C_SLV3_ADDR 0x0F
#define I2C_SLV3_REG 0x10
#define I2C_SLV3_CTRL 0x11
#define I2C_SLV3_DO 0x12
#define I2C_SLV4_ADDR 0x13
#define I2C_SLV4_REG 0x14
#define I2C_SLV4_CTRL 0x15
#define I2C_SLV4_DO 0x16
#define I2C_SLV4_DI 0x17


#define BANK_0 (0)
#define BANK_1 (0b00010000)
#define BANK_2 (0b00100000)
#define BANK_3 (0b00110000)



#define BIT_ACCEL_INTEL_EN      0x02                        /**< Wake-up On Motion enable bit                           */
#define BIT_ACCEL_INTEL_MODE    0x01                        /**< WOM algorithm selection bit         */

#define GYRO_FULLSCALE_250DPS   0x00     /**< Gyro Full Scale = 250 deg/sec  */
#define GYRO_FULLSCALE_500DPS   0x01     /**< Gyro Full Scale = 500 deg/sec  */
#define GYRO_FULLSCALE_1000DPS  0x02     /**< Gyro Full Scale = 1000 deg/sec */
#define GYRO_FULLSCALE_2000DPS  0x03     /**< Gyro Full Scale = 2000 deg/sec */
#define GYRO_BW_12100HZ         0x08       /**< Gyro Bandwidth = 12100 Hz */
#define GYRO_BW_360HZ           0x07       /**< Gyro Bandwidth = 360 Hz   */
#define GYRO_BW_200HZ           0x00       /**< Gyro Bandwidth = 200 Hz   */
#define GYRO_BW_150HZ           0x01       /**< Gyro Bandwidth = 150 Hz   */
#define GYRO_BW_120HZ           0x02       /**< Gyro Bandwidth = 120 Hz   */
#define GYRO_BW_51HZ            0x03       /**< Gyro Bandwidth = 51 Hz    */
#define GYRO_BW_24HZ            0x04       /**< Gyro Bandwidth = 24 Hz    */
#define GYRO_BW_12HZ            0x05       /**< Gyro Bandwidth = 12 Hz    */
#define GYRO_BW_6HZ             0x06       /**< Gyro Bandwidth = 6 Hz     */

#define ACCEL_FULLSCALE_2G      0x00  /**< Accel Full Scale = 2 g  */
#define ACCEL_FULLSCALE_4G      0x01  /**< Accel Full Scale = 4 g  */
#define ACCEL_FULLSCALE_8G      0x02  /**< Accel Full Scale = 8 g  */
#define ACCEL_FULLSCALE_16G     0x03  /**< Accel Full Scale = 16 g */
#define ACCEL_BW_1210HZ         0x08     /**< Accel Bandwidth = 1210 Hz  */
#define ACCEL_BW_470HZ          0x07     /**< Accel Bandwidth = 470 Hz   */
#define ACCEL_BW_246HZ          0x00     /**< Accel Bandwidth = 246 Hz   */
#define ACCEL_BW_111HZ          0x02     /**< Accel Bandwidth = 111 Hz   */
#define ACCEL_BW_50HZ           0x03     /**< Accel Bandwidth = 50 Hz    */
#define ACCEL_BW_24HZ           0x04     /**< Accel Bandwidth = 24 Hz    */
#define ACCEL_BW_12HZ           0x05     /**< Accel Bandwidth = 12 Hz    */
#define ACCEL_BW_6HZ            0x06     /**< Accel Bandwidth = 6 Hz     */
 

// Useful MACRO
#define ICM20948_READ_MASK(x) (x | 0x80)
#define ICM20948_WRITE_MASK(x) (x & 0x7F)

#define MSB_UINT16(a) (((a) >> 8) & 0xFF)
#define LSB_UINT16(a) ((a) & 0xFF)

__extension__ typedef struct USER_CTRLBITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned :1;
  unsigned I2C_MST_RST:1;
  unsigned SRAM_RST:1;
  unsigned DMP_RST:1;
  unsigned I2C_IF_DIS:1;
  unsigned I2C_MST_EN:1;
  unsigned FIFO_EN:1;
  unsigned DMP_EN:1;
  };
};
}USER_CTRLbits;

__extension__ typedef struct LP_CONFIGBITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned :4;
  unsigned GYRO_CYCLE:1;
  unsigned ACCEL_CYCLE:1;
  unsigned I2C_MST_CYCLE:1;
  unsigned :1;
  };
};
}LP_CONFIGbits;

__extension__ typedef struct PWR_MGMT_1BITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned CLKSEL:3;
  unsigned TEMP_DIS:1;
  unsigned :1;
  unsigned LP_EN:1;
  unsigned SLEEP:1;
  unsigned DEVICE_RESET:1;
  };
};
}PWR_MGMT_1bits;

__extension__ typedef struct PWR_MGMT_2BITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned DISABLE_GYRO:3;
  unsigned DISABLE_ACCEL:3;
  unsigned :2;
  };
};
}PWR_MGMT_2bits;

__extension__ typedef struct INT_PIN_CFGBITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned :1;
  unsigned BYPASS_EN:1;
  unsigned FSYNC_INT_MODE_EN:1;
  unsigned ACTL_FSYNC:1;
  unsigned INT_ANYRD_2CLEAR:1;
  unsigned INT1_LATCH_INT_EN:1;
  unsigned INT1_OPEN:1;
  unsigned INT1_ACTL:1;
  };
};
}INT_PIN_CFGbits;

__extension__ typedef struct INT_ENABLEBITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned I2C_MST_INT_EN:1;
  unsigned DMP_INT1_EN:1;
  unsigned PLL_RDY_EN:1;
  unsigned WOM_INT_EN:1;
  unsigned :3;
  unsigned REG_WOF_EN:1;
  };
};
}INT_ENABLEbits;

__extension__ typedef struct FIFO_EN_2BITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned TEMP_FIFO_EN:1;
  unsigned GYRO_X_FIFO_EN:1;
  unsigned GYRO_Y_FIFO_EN:1;
  unsigned GYRO_Z_FIFO_EN:1;
  unsigned ACCEL_FIFO_EN:1;
  unsigned :3;
  };
};
}FIFO_EN_2bits;

//USER BANK 2 STRUCT
__extension__ typedef struct GYRO_CFG_1BITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned GYRO_FCHOICE:1;
  unsigned GYRO_FS_SEL:2;
  unsigned GYRO_DLPFCFG:3;
  unsigned :2;
  };
};
}GYRO_CFG_1bits;

__extension__ typedef struct GYRO_CFG_2BITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned GYRO_AVGCFG:3;
  unsigned ZGYRO_CTEN:1;
  unsigned YGYRO_CTEN:1;
  unsigned XGYRO_CTEN:1;
  unsigned :2;
  };
};
}GYRO_CFG_2bits;

__extension__ typedef struct ACCEL_CONFIGBITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned ACCEL_FCHOICE:1;
  unsigned ACCEL_FS_SEL:2;
  unsigned ACCEL_DLPFCFG:3;
  unsigned XGYRO_CTEN:2;
  };
};
}ACCEL_CONFIGbits;

__extension__ typedef struct ACCEL_CONFIG_2BITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned DEC3_CFG:2;
  unsigned AZ_ST_EN_REG:1;
  unsigned AY_ST_EN_REG:1;
  unsigned AX_ST_EN_REG:1;
  unsigned :3;
  };
};
}ACCEL_CONFIG_2bits;

__extension__ typedef struct FSYNC_CONFIGBITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned EXT_SYNC_SET:4;
  unsigned WOF_EDGE_INT:1;
  unsigned WOF_DEGLITCH_EN:1;
  unsigned :1;
  unsigned DELAY_TIME_EN:1;
  };
};
}FSYNC_CONFIGbits;

//I2C
__extension__ typedef struct I2C_MST_CTRLBITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned I2C_MST_CLK:4;
  unsigned I2C_MST_P_NSR:1;
  unsigned :2;
  unsigned MULT_MST_EN:1;
  };
};
}I2C_MST_CTRLbits;

__extension__ typedef struct I2C_MST_DELAY_CTRLBITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned I2C_SLV_0_DELAY_EN:1;
  unsigned I2C_SLV_1_DELAY_EN:1;
  unsigned I2C_SLV_2_DELAY_EN:1;
  unsigned I2C_SLV_3_DELAY_EN:1;
  unsigned I2C_SLV_4_DELAY_EN:1;
  unsigned :2;
  unsigned DELAY_ES_SHADOW:1;
  };
};
}I2C_MST_DELAY_CTRLbits;

__extension__ typedef struct I2C_SLVx_CTRLBITS{
union {
struct{
  unsigned ALL:8;
   };

struct {
  unsigned I2C_SLVx_LENG:4;
  unsigned I2C_SLVx_GRP:1;
  unsigned I2C_SLVx_REG_DIS:1;
  unsigned I2C_SLVx_BYTE_SW:1;
  unsigned I2C_SLVx_EN:1;
  };
};
}I2C_SLVx_CTRLbits;
//USER BANK 3 STRUCT
#endif	/* XC_HEADER_TEMPLATE_H */

