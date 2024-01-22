#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-CM_V10.mk)" "nbproject/Makefile-local-CM_V10.mk"
include nbproject/Makefile-local-CM_V10.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=CM_V10
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Carte_Moteur_V9.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Carte_Moteur_V9.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../../mla/v2018_11_26/framework/usb/src/usb_device.c ../../../mla/v2018_11_26/framework/usb/src/usb_device_generic.c ../../../mla/v2018_11_26/framework/usb/src/usb_hal_16bit.c USB/usb_descriptors.c USB/usb_events.c UTLN_Timers.c UTLN_PWM.c UTLN_adc.c ustv_i2c_interrupt.c UTLN_uart.c UTLN_Led.c UTLN_QEI.c UTLN_SPI.c UTLN_LS7366R.c UTLN_ICM20948.c Utilities.c UTLN_OutputCompare.c UTLN_CAN.c ChipConfig.c systemInterrupt.c HardwareDependant/system.c HardwareProfile_CM_V9.c OSAL_Memory.c OSAL.c OSAL_Timers.c UTLN_Communication.c UTLN_Typedefs.c main.c UTLN_OSAL.c UTLN_Flag.c RobotObj.c traps.c UTLN_Odometrie.c UTLN_POLOLU_Servo.c Asservissement.c USB/app_device_vendor_basic.c UTLN_CircularBuffer.c UTLN_CoilGun.c TrajectoryGeneratorSingleMotor.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1116702204/usb_device.o ${OBJECTDIR}/_ext/1116702204/usb_device_generic.o ${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o ${OBJECTDIR}/USB/usb_descriptors.o ${OBJECTDIR}/USB/usb_events.o ${OBJECTDIR}/UTLN_Timers.o ${OBJECTDIR}/UTLN_PWM.o ${OBJECTDIR}/UTLN_adc.o ${OBJECTDIR}/ustv_i2c_interrupt.o ${OBJECTDIR}/UTLN_uart.o ${OBJECTDIR}/UTLN_Led.o ${OBJECTDIR}/UTLN_QEI.o ${OBJECTDIR}/UTLN_SPI.o ${OBJECTDIR}/UTLN_LS7366R.o ${OBJECTDIR}/UTLN_ICM20948.o ${OBJECTDIR}/Utilities.o ${OBJECTDIR}/UTLN_OutputCompare.o ${OBJECTDIR}/UTLN_CAN.o ${OBJECTDIR}/ChipConfig.o ${OBJECTDIR}/systemInterrupt.o ${OBJECTDIR}/HardwareDependant/system.o ${OBJECTDIR}/HardwareProfile_CM_V9.o ${OBJECTDIR}/OSAL_Memory.o ${OBJECTDIR}/OSAL.o ${OBJECTDIR}/OSAL_Timers.o ${OBJECTDIR}/UTLN_Communication.o ${OBJECTDIR}/UTLN_Typedefs.o ${OBJECTDIR}/main.o ${OBJECTDIR}/UTLN_OSAL.o ${OBJECTDIR}/UTLN_Flag.o ${OBJECTDIR}/RobotObj.o ${OBJECTDIR}/traps.o ${OBJECTDIR}/UTLN_Odometrie.o ${OBJECTDIR}/UTLN_POLOLU_Servo.o ${OBJECTDIR}/Asservissement.o ${OBJECTDIR}/USB/app_device_vendor_basic.o ${OBJECTDIR}/UTLN_CircularBuffer.o ${OBJECTDIR}/UTLN_CoilGun.o ${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1116702204/usb_device.o.d ${OBJECTDIR}/_ext/1116702204/usb_device_generic.o.d ${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o.d ${OBJECTDIR}/USB/usb_descriptors.o.d ${OBJECTDIR}/USB/usb_events.o.d ${OBJECTDIR}/UTLN_Timers.o.d ${OBJECTDIR}/UTLN_PWM.o.d ${OBJECTDIR}/UTLN_adc.o.d ${OBJECTDIR}/ustv_i2c_interrupt.o.d ${OBJECTDIR}/UTLN_uart.o.d ${OBJECTDIR}/UTLN_Led.o.d ${OBJECTDIR}/UTLN_QEI.o.d ${OBJECTDIR}/UTLN_SPI.o.d ${OBJECTDIR}/UTLN_LS7366R.o.d ${OBJECTDIR}/UTLN_ICM20948.o.d ${OBJECTDIR}/Utilities.o.d ${OBJECTDIR}/UTLN_OutputCompare.o.d ${OBJECTDIR}/UTLN_CAN.o.d ${OBJECTDIR}/ChipConfig.o.d ${OBJECTDIR}/systemInterrupt.o.d ${OBJECTDIR}/HardwareDependant/system.o.d ${OBJECTDIR}/HardwareProfile_CM_V9.o.d ${OBJECTDIR}/OSAL_Memory.o.d ${OBJECTDIR}/OSAL.o.d ${OBJECTDIR}/OSAL_Timers.o.d ${OBJECTDIR}/UTLN_Communication.o.d ${OBJECTDIR}/UTLN_Typedefs.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/UTLN_OSAL.o.d ${OBJECTDIR}/UTLN_Flag.o.d ${OBJECTDIR}/RobotObj.o.d ${OBJECTDIR}/traps.o.d ${OBJECTDIR}/UTLN_Odometrie.o.d ${OBJECTDIR}/UTLN_POLOLU_Servo.o.d ${OBJECTDIR}/Asservissement.o.d ${OBJECTDIR}/USB/app_device_vendor_basic.o.d ${OBJECTDIR}/UTLN_CircularBuffer.o.d ${OBJECTDIR}/UTLN_CoilGun.o.d ${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1116702204/usb_device.o ${OBJECTDIR}/_ext/1116702204/usb_device_generic.o ${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o ${OBJECTDIR}/USB/usb_descriptors.o ${OBJECTDIR}/USB/usb_events.o ${OBJECTDIR}/UTLN_Timers.o ${OBJECTDIR}/UTLN_PWM.o ${OBJECTDIR}/UTLN_adc.o ${OBJECTDIR}/ustv_i2c_interrupt.o ${OBJECTDIR}/UTLN_uart.o ${OBJECTDIR}/UTLN_Led.o ${OBJECTDIR}/UTLN_QEI.o ${OBJECTDIR}/UTLN_SPI.o ${OBJECTDIR}/UTLN_LS7366R.o ${OBJECTDIR}/UTLN_ICM20948.o ${OBJECTDIR}/Utilities.o ${OBJECTDIR}/UTLN_OutputCompare.o ${OBJECTDIR}/UTLN_CAN.o ${OBJECTDIR}/ChipConfig.o ${OBJECTDIR}/systemInterrupt.o ${OBJECTDIR}/HardwareDependant/system.o ${OBJECTDIR}/HardwareProfile_CM_V9.o ${OBJECTDIR}/OSAL_Memory.o ${OBJECTDIR}/OSAL.o ${OBJECTDIR}/OSAL_Timers.o ${OBJECTDIR}/UTLN_Communication.o ${OBJECTDIR}/UTLN_Typedefs.o ${OBJECTDIR}/main.o ${OBJECTDIR}/UTLN_OSAL.o ${OBJECTDIR}/UTLN_Flag.o ${OBJECTDIR}/RobotObj.o ${OBJECTDIR}/traps.o ${OBJECTDIR}/UTLN_Odometrie.o ${OBJECTDIR}/UTLN_POLOLU_Servo.o ${OBJECTDIR}/Asservissement.o ${OBJECTDIR}/USB/app_device_vendor_basic.o ${OBJECTDIR}/UTLN_CircularBuffer.o ${OBJECTDIR}/UTLN_CoilGun.o ${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o

# Source Files
SOURCEFILES=../../../mla/v2018_11_26/framework/usb/src/usb_device.c ../../../mla/v2018_11_26/framework/usb/src/usb_device_generic.c ../../../mla/v2018_11_26/framework/usb/src/usb_hal_16bit.c USB/usb_descriptors.c USB/usb_events.c UTLN_Timers.c UTLN_PWM.c UTLN_adc.c ustv_i2c_interrupt.c UTLN_uart.c UTLN_Led.c UTLN_QEI.c UTLN_SPI.c UTLN_LS7366R.c UTLN_ICM20948.c Utilities.c UTLN_OutputCompare.c UTLN_CAN.c ChipConfig.c systemInterrupt.c HardwareDependant/system.c HardwareProfile_CM_V9.c OSAL_Memory.c OSAL.c OSAL_Timers.c UTLN_Communication.c UTLN_Typedefs.c main.c UTLN_OSAL.c UTLN_Flag.c RobotObj.c traps.c UTLN_Odometrie.c UTLN_POLOLU_Servo.c Asservissement.c USB/app_device_vendor_basic.c UTLN_CircularBuffer.c UTLN_CoilGun.c TrajectoryGeneratorSingleMotor.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-CM_V10.mk ${DISTDIR}/Carte_Moteur_V9.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP512MU814
MP_LINKER_FILE_OPTION=,--script=p33EP512MU814.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1116702204/usb_device.o: ../../../mla/v2018_11_26/framework/usb/src/usb_device.c  .generated_files/flags/CM_V10/f36026b247ba17fe2002d89cdf3a486d99728d4 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1116702204" 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../mla/v2018_11_26/framework/usb/src/usb_device.c  -o ${OBJECTDIR}/_ext/1116702204/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1116702204/usb_device.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1116702204/usb_device_generic.o: ../../../mla/v2018_11_26/framework/usb/src/usb_device_generic.c  .generated_files/flags/CM_V10/ee4644e62d5534fcdeb8d8a8b6d15e7916d36862 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1116702204" 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_device_generic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_device_generic.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../mla/v2018_11_26/framework/usb/src/usb_device_generic.c  -o ${OBJECTDIR}/_ext/1116702204/usb_device_generic.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1116702204/usb_device_generic.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o: ../../../mla/v2018_11_26/framework/usb/src/usb_hal_16bit.c  .generated_files/flags/CM_V10/8a5492df95127de76ca04d9acee69be52a6d9001 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1116702204" 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../mla/v2018_11_26/framework/usb/src/usb_hal_16bit.c  -o ${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/USB/usb_descriptors.o: USB/usb_descriptors.c  .generated_files/flags/CM_V10/bd3afb76e4cba521a95ccd2bf86b2faa35a0d0ab .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/USB/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  USB/usb_descriptors.c  -o ${OBJECTDIR}/USB/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/USB/usb_descriptors.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/USB/usb_events.o: USB/usb_events.c  .generated_files/flags/CM_V10/61d9c75ddaed02c1f9ef4983118acbeea7c4bf1b .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/usb_events.o.d 
	@${RM} ${OBJECTDIR}/USB/usb_events.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  USB/usb_events.c  -o ${OBJECTDIR}/USB/usb_events.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/USB/usb_events.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Timers.o: UTLN_Timers.c  .generated_files/flags/CM_V10/3c80900e8eba29577c907ff7cc828b00fab22379 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Timers.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Timers.c  -o ${OBJECTDIR}/UTLN_Timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Timers.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_PWM.o: UTLN_PWM.c  .generated_files/flags/CM_V10/4bec89f127c986dc8ada49a40a58145fc0ba5528 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_PWM.o.d 
	@${RM} ${OBJECTDIR}/UTLN_PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_PWM.c  -o ${OBJECTDIR}/UTLN_PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_PWM.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_adc.o: UTLN_adc.c  .generated_files/flags/CM_V10/44117f011acaeb1aa259e526f42c9c7eb74186b9 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_adc.o.d 
	@${RM} ${OBJECTDIR}/UTLN_adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_adc.c  -o ${OBJECTDIR}/UTLN_adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_adc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ustv_i2c_interrupt.o: ustv_i2c_interrupt.c  .generated_files/flags/CM_V10/303ad18ea19d6ce294eff851b7c0cf01a2c7d518 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ustv_i2c_interrupt.o.d 
	@${RM} ${OBJECTDIR}/ustv_i2c_interrupt.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ustv_i2c_interrupt.c  -o ${OBJECTDIR}/ustv_i2c_interrupt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ustv_i2c_interrupt.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_uart.o: UTLN_uart.c  .generated_files/flags/CM_V10/1add053a8bec68fa0926b3b7faa60c2a71927d0 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_uart.o.d 
	@${RM} ${OBJECTDIR}/UTLN_uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_uart.c  -o ${OBJECTDIR}/UTLN_uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_uart.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Led.o: UTLN_Led.c  .generated_files/flags/CM_V10/58788ca78bdca16877300adb3be21da97a87e30c .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Led.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Led.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Led.c  -o ${OBJECTDIR}/UTLN_Led.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Led.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_QEI.o: UTLN_QEI.c  .generated_files/flags/CM_V10/11f821b27c890fefc9b1c6d0a63e3ac3cd1496a2 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_QEI.o.d 
	@${RM} ${OBJECTDIR}/UTLN_QEI.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_QEI.c  -o ${OBJECTDIR}/UTLN_QEI.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_QEI.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_SPI.o: UTLN_SPI.c  .generated_files/flags/CM_V10/bfca754f20962022a2f4969781babb686f599943 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_SPI.o.d 
	@${RM} ${OBJECTDIR}/UTLN_SPI.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_SPI.c  -o ${OBJECTDIR}/UTLN_SPI.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_SPI.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_LS7366R.o: UTLN_LS7366R.c  .generated_files/flags/CM_V10/d1ab7cc9bb2815a8fc41117767e9bd87bd9844c5 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_LS7366R.o.d 
	@${RM} ${OBJECTDIR}/UTLN_LS7366R.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_LS7366R.c  -o ${OBJECTDIR}/UTLN_LS7366R.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_LS7366R.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_ICM20948.o: UTLN_ICM20948.c  .generated_files/flags/CM_V10/8b7ec3ed5c56fa74e5acce3dfd382b56e3782021 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_ICM20948.o.d 
	@${RM} ${OBJECTDIR}/UTLN_ICM20948.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_ICM20948.c  -o ${OBJECTDIR}/UTLN_ICM20948.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_ICM20948.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Utilities.o: Utilities.c  .generated_files/flags/CM_V10/8af9ef65d775058dcb1325184e90905292d5d3e8 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Utilities.o.d 
	@${RM} ${OBJECTDIR}/Utilities.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Utilities.c  -o ${OBJECTDIR}/Utilities.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Utilities.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_OutputCompare.o: UTLN_OutputCompare.c  .generated_files/flags/CM_V10/5bbdbbfc004c563bc6faf7e631f97945aadf4790 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_OutputCompare.o.d 
	@${RM} ${OBJECTDIR}/UTLN_OutputCompare.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_OutputCompare.c  -o ${OBJECTDIR}/UTLN_OutputCompare.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_OutputCompare.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_CAN.o: UTLN_CAN.c  .generated_files/flags/CM_V10/fcbbe88bc2bb05c8df684d9067ce41b2193b4538 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_CAN.o.d 
	@${RM} ${OBJECTDIR}/UTLN_CAN.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_CAN.c  -o ${OBJECTDIR}/UTLN_CAN.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_CAN.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ChipConfig.o: ChipConfig.c  .generated_files/flags/CM_V10/a36763535406ab31c98687ddf51a46488f78f795 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ChipConfig.o.d 
	@${RM} ${OBJECTDIR}/ChipConfig.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ChipConfig.c  -o ${OBJECTDIR}/ChipConfig.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ChipConfig.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/systemInterrupt.o: systemInterrupt.c  .generated_files/flags/CM_V10/6ea26231b1516b0b71ee6c6550b920b90efdc1f9 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/systemInterrupt.o.d 
	@${RM} ${OBJECTDIR}/systemInterrupt.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  systemInterrupt.c  -o ${OBJECTDIR}/systemInterrupt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/systemInterrupt.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/HardwareDependant/system.o: HardwareDependant/system.c  .generated_files/flags/CM_V10/ecde159c1adc70816499258ff51fda43cda3792d .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/HardwareDependant" 
	@${RM} ${OBJECTDIR}/HardwareDependant/system.o.d 
	@${RM} ${OBJECTDIR}/HardwareDependant/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  HardwareDependant/system.c  -o ${OBJECTDIR}/HardwareDependant/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/HardwareDependant/system.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/HardwareProfile_CM_V9.o: HardwareProfile_CM_V9.c  .generated_files/flags/CM_V10/35153759abe997f3f517466b01b1ad30a07bc1a0 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/HardwareProfile_CM_V9.o.d 
	@${RM} ${OBJECTDIR}/HardwareProfile_CM_V9.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  HardwareProfile_CM_V9.c  -o ${OBJECTDIR}/HardwareProfile_CM_V9.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/HardwareProfile_CM_V9.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/OSAL_Memory.o: OSAL_Memory.c  .generated_files/flags/CM_V10/55fc523d3b8c8f2ef95835a94e3ba7f8c3586187 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/OSAL_Memory.o.d 
	@${RM} ${OBJECTDIR}/OSAL_Memory.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  OSAL_Memory.c  -o ${OBJECTDIR}/OSAL_Memory.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/OSAL_Memory.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/OSAL.o: OSAL.c  .generated_files/flags/CM_V10/72f9828ce7735f27daf463e0a6a6507463f98acf .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/OSAL.o.d 
	@${RM} ${OBJECTDIR}/OSAL.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  OSAL.c  -o ${OBJECTDIR}/OSAL.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/OSAL.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/OSAL_Timers.o: OSAL_Timers.c  .generated_files/flags/CM_V10/8ae3928396d778a7bac23c09f690318b211a8f43 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/OSAL_Timers.o.d 
	@${RM} ${OBJECTDIR}/OSAL_Timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  OSAL_Timers.c  -o ${OBJECTDIR}/OSAL_Timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/OSAL_Timers.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Communication.o: UTLN_Communication.c  .generated_files/flags/CM_V10/69c0a759dbc82304936e3acb0f71df00f0d1dbfe .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Communication.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Communication.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Communication.c  -o ${OBJECTDIR}/UTLN_Communication.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Communication.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Typedefs.o: UTLN_Typedefs.c  .generated_files/flags/CM_V10/ec84db8154724331f5778a6ce61ecc30164c2432 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Typedefs.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Typedefs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Typedefs.c  -o ${OBJECTDIR}/UTLN_Typedefs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Typedefs.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/main.o: main.c  .generated_files/flags/CM_V10/ad2a8e315d4b902b367eb136de03bdf2b5d0c944 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_OSAL.o: UTLN_OSAL.c  .generated_files/flags/CM_V10/98cbb2d67099bb8d34f0a55c2f29a0d48baaee7a .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_OSAL.o.d 
	@${RM} ${OBJECTDIR}/UTLN_OSAL.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_OSAL.c  -o ${OBJECTDIR}/UTLN_OSAL.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_OSAL.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Flag.o: UTLN_Flag.c  .generated_files/flags/CM_V10/fa9418ddd8ef628900a687042f964b042176637b .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Flag.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Flag.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Flag.c  -o ${OBJECTDIR}/UTLN_Flag.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Flag.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/RobotObj.o: RobotObj.c  .generated_files/flags/CM_V10/9a046599da6c72ccc2e6d87b12eeb3000a5b359c .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RobotObj.o.d 
	@${RM} ${OBJECTDIR}/RobotObj.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  RobotObj.c  -o ${OBJECTDIR}/RobotObj.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/RobotObj.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/traps.o: traps.c  .generated_files/flags/CM_V10/550f11c1367da48a88f0dc46ead8f8f326331096 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/traps.o.d 
	@${RM} ${OBJECTDIR}/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  traps.c  -o ${OBJECTDIR}/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/traps.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Odometrie.o: UTLN_Odometrie.c  .generated_files/flags/CM_V10/31b1146e867c0678cae633e314ebd35d12edca57 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Odometrie.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Odometrie.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Odometrie.c  -o ${OBJECTDIR}/UTLN_Odometrie.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Odometrie.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_POLOLU_Servo.o: UTLN_POLOLU_Servo.c  .generated_files/flags/CM_V10/8f16d12f1252d4d46c7724add477de1e37564f83 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_POLOLU_Servo.o.d 
	@${RM} ${OBJECTDIR}/UTLN_POLOLU_Servo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_POLOLU_Servo.c  -o ${OBJECTDIR}/UTLN_POLOLU_Servo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_POLOLU_Servo.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Asservissement.o: Asservissement.c  .generated_files/flags/CM_V10/8d98a99a57bb7ff37ef9d67c336f695b466535d1 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Asservissement.o.d 
	@${RM} ${OBJECTDIR}/Asservissement.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Asservissement.c  -o ${OBJECTDIR}/Asservissement.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Asservissement.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/USB/app_device_vendor_basic.o: USB/app_device_vendor_basic.c  .generated_files/flags/CM_V10/704b2c32b99f5c650b065282d517859d67bff97d .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/app_device_vendor_basic.o.d 
	@${RM} ${OBJECTDIR}/USB/app_device_vendor_basic.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  USB/app_device_vendor_basic.c  -o ${OBJECTDIR}/USB/app_device_vendor_basic.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/USB/app_device_vendor_basic.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_CircularBuffer.o: UTLN_CircularBuffer.c  .generated_files/flags/CM_V10/a17044e756c147d1f15b06750d7ae6cfb9de001b .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_CircularBuffer.o.d 
	@${RM} ${OBJECTDIR}/UTLN_CircularBuffer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_CircularBuffer.c  -o ${OBJECTDIR}/UTLN_CircularBuffer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_CircularBuffer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_CoilGun.o: UTLN_CoilGun.c  .generated_files/flags/CM_V10/b71345ef87367ef68b8c2610e5626b7d5659c907 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_CoilGun.o.d 
	@${RM} ${OBJECTDIR}/UTLN_CoilGun.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_CoilGun.c  -o ${OBJECTDIR}/UTLN_CoilGun.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_CoilGun.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o: TrajectoryGeneratorSingleMotor.c  .generated_files/flags/CM_V10/988f39ff1e6857af03d306de82ce7c1586774bc6 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o.d 
	@${RM} ${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  TrajectoryGeneratorSingleMotor.c  -o ${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/_ext/1116702204/usb_device.o: ../../../mla/v2018_11_26/framework/usb/src/usb_device.c  .generated_files/flags/CM_V10/4e4e53d091e1536dbe695d0805b84583d37b0c44 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1116702204" 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_device.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../mla/v2018_11_26/framework/usb/src/usb_device.c  -o ${OBJECTDIR}/_ext/1116702204/usb_device.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1116702204/usb_device.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1116702204/usb_device_generic.o: ../../../mla/v2018_11_26/framework/usb/src/usb_device_generic.c  .generated_files/flags/CM_V10/73ebd8e64dc81d77a37cf556d77c9d6ffcc080bd .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1116702204" 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_device_generic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_device_generic.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../mla/v2018_11_26/framework/usb/src/usb_device_generic.c  -o ${OBJECTDIR}/_ext/1116702204/usb_device_generic.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1116702204/usb_device_generic.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o: ../../../mla/v2018_11_26/framework/usb/src/usb_hal_16bit.c  .generated_files/flags/CM_V10/72759e578e861e899e809eef8a6b36b110e728d5 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1116702204" 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o.d 
	@${RM} ${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../../mla/v2018_11_26/framework/usb/src/usb_hal_16bit.c  -o ${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1116702204/usb_hal_16bit.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/USB/usb_descriptors.o: USB/usb_descriptors.c  .generated_files/flags/CM_V10/3a6db4200ff41e6df9f509bd314beb6937e1829e .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/usb_descriptors.o.d 
	@${RM} ${OBJECTDIR}/USB/usb_descriptors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  USB/usb_descriptors.c  -o ${OBJECTDIR}/USB/usb_descriptors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/USB/usb_descriptors.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/USB/usb_events.o: USB/usb_events.c  .generated_files/flags/CM_V10/429b453f2deeabd418ede7a028fbfd67af011019 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/usb_events.o.d 
	@${RM} ${OBJECTDIR}/USB/usb_events.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  USB/usb_events.c  -o ${OBJECTDIR}/USB/usb_events.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/USB/usb_events.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Timers.o: UTLN_Timers.c  .generated_files/flags/CM_V10/919cdc00b0192205ec525b7831b9f25fc20d50b5 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Timers.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Timers.c  -o ${OBJECTDIR}/UTLN_Timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Timers.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_PWM.o: UTLN_PWM.c  .generated_files/flags/CM_V10/85e46068fe822635d0164b0da1c62d47f4896699 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_PWM.o.d 
	@${RM} ${OBJECTDIR}/UTLN_PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_PWM.c  -o ${OBJECTDIR}/UTLN_PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_PWM.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_adc.o: UTLN_adc.c  .generated_files/flags/CM_V10/b6102bdc7b0ea0f5669c7a3fa2ccbec2f4e72b5c .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_adc.o.d 
	@${RM} ${OBJECTDIR}/UTLN_adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_adc.c  -o ${OBJECTDIR}/UTLN_adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_adc.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ustv_i2c_interrupt.o: ustv_i2c_interrupt.c  .generated_files/flags/CM_V10/1be018cbf7102afdece02e423ac48d63443f883f .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ustv_i2c_interrupt.o.d 
	@${RM} ${OBJECTDIR}/ustv_i2c_interrupt.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ustv_i2c_interrupt.c  -o ${OBJECTDIR}/ustv_i2c_interrupt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ustv_i2c_interrupt.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_uart.o: UTLN_uart.c  .generated_files/flags/CM_V10/9a00e7ad91b25634d0c98ad63065a26822ff1ff1 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_uart.o.d 
	@${RM} ${OBJECTDIR}/UTLN_uart.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_uart.c  -o ${OBJECTDIR}/UTLN_uart.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_uart.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Led.o: UTLN_Led.c  .generated_files/flags/CM_V10/ce08693878ea6b571cb11b1fec91512b300a3630 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Led.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Led.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Led.c  -o ${OBJECTDIR}/UTLN_Led.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Led.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_QEI.o: UTLN_QEI.c  .generated_files/flags/CM_V10/536cdad245b49f9475c934bc492cc8cb814711e1 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_QEI.o.d 
	@${RM} ${OBJECTDIR}/UTLN_QEI.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_QEI.c  -o ${OBJECTDIR}/UTLN_QEI.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_QEI.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_SPI.o: UTLN_SPI.c  .generated_files/flags/CM_V10/aaaca21b35a8ad47a35e6454a136a603b775f05d .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_SPI.o.d 
	@${RM} ${OBJECTDIR}/UTLN_SPI.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_SPI.c  -o ${OBJECTDIR}/UTLN_SPI.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_SPI.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_LS7366R.o: UTLN_LS7366R.c  .generated_files/flags/CM_V10/69bb25ae4f6fce6e9910b37b669579df87f60651 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_LS7366R.o.d 
	@${RM} ${OBJECTDIR}/UTLN_LS7366R.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_LS7366R.c  -o ${OBJECTDIR}/UTLN_LS7366R.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_LS7366R.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_ICM20948.o: UTLN_ICM20948.c  .generated_files/flags/CM_V10/e1468a7d2f014f016cba4d91af60f6f05682848b .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_ICM20948.o.d 
	@${RM} ${OBJECTDIR}/UTLN_ICM20948.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_ICM20948.c  -o ${OBJECTDIR}/UTLN_ICM20948.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_ICM20948.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Utilities.o: Utilities.c  .generated_files/flags/CM_V10/b45ee877da9f0e43bb5a27e7bd4cb08ac777646d .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Utilities.o.d 
	@${RM} ${OBJECTDIR}/Utilities.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Utilities.c  -o ${OBJECTDIR}/Utilities.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Utilities.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_OutputCompare.o: UTLN_OutputCompare.c  .generated_files/flags/CM_V10/a45c16f01f169914efbaa4dd1a5b2fe2d21fd319 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_OutputCompare.o.d 
	@${RM} ${OBJECTDIR}/UTLN_OutputCompare.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_OutputCompare.c  -o ${OBJECTDIR}/UTLN_OutputCompare.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_OutputCompare.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_CAN.o: UTLN_CAN.c  .generated_files/flags/CM_V10/592673e4776616eafafe4e4e4822b4222ad9838e .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_CAN.o.d 
	@${RM} ${OBJECTDIR}/UTLN_CAN.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_CAN.c  -o ${OBJECTDIR}/UTLN_CAN.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_CAN.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ChipConfig.o: ChipConfig.c  .generated_files/flags/CM_V10/6fa7381a1659b179c13a8792b0c9ecd2f076e74a .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ChipConfig.o.d 
	@${RM} ${OBJECTDIR}/ChipConfig.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ChipConfig.c  -o ${OBJECTDIR}/ChipConfig.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ChipConfig.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/systemInterrupt.o: systemInterrupt.c  .generated_files/flags/CM_V10/1d4dacd63b2f4f7b76e120cad3624e956cbb5e1f .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/systemInterrupt.o.d 
	@${RM} ${OBJECTDIR}/systemInterrupt.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  systemInterrupt.c  -o ${OBJECTDIR}/systemInterrupt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/systemInterrupt.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/HardwareDependant/system.o: HardwareDependant/system.c  .generated_files/flags/CM_V10/c520b3ed153df73247976bfcb572cd722b82ca4b .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/HardwareDependant" 
	@${RM} ${OBJECTDIR}/HardwareDependant/system.o.d 
	@${RM} ${OBJECTDIR}/HardwareDependant/system.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  HardwareDependant/system.c  -o ${OBJECTDIR}/HardwareDependant/system.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/HardwareDependant/system.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/HardwareProfile_CM_V9.o: HardwareProfile_CM_V9.c  .generated_files/flags/CM_V10/dd5d901433fde90ab53815786ccb0cb4925b554 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/HardwareProfile_CM_V9.o.d 
	@${RM} ${OBJECTDIR}/HardwareProfile_CM_V9.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  HardwareProfile_CM_V9.c  -o ${OBJECTDIR}/HardwareProfile_CM_V9.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/HardwareProfile_CM_V9.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/OSAL_Memory.o: OSAL_Memory.c  .generated_files/flags/CM_V10/5c0781b6eab55de671232a3e685cb102ae9af916 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/OSAL_Memory.o.d 
	@${RM} ${OBJECTDIR}/OSAL_Memory.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  OSAL_Memory.c  -o ${OBJECTDIR}/OSAL_Memory.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/OSAL_Memory.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/OSAL.o: OSAL.c  .generated_files/flags/CM_V10/16a1c330e6027c0dee2635528138db6aacfeeb50 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/OSAL.o.d 
	@${RM} ${OBJECTDIR}/OSAL.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  OSAL.c  -o ${OBJECTDIR}/OSAL.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/OSAL.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/OSAL_Timers.o: OSAL_Timers.c  .generated_files/flags/CM_V10/6b1ca6e2761b1c4d8361f39b331bd3f0a36f365e .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/OSAL_Timers.o.d 
	@${RM} ${OBJECTDIR}/OSAL_Timers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  OSAL_Timers.c  -o ${OBJECTDIR}/OSAL_Timers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/OSAL_Timers.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Communication.o: UTLN_Communication.c  .generated_files/flags/CM_V10/94d51811b54b1ae24c04eb1e514179ac487de742 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Communication.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Communication.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Communication.c  -o ${OBJECTDIR}/UTLN_Communication.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Communication.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Typedefs.o: UTLN_Typedefs.c  .generated_files/flags/CM_V10/ffdc9a16dac47d46ce8e20df9dab08cfc04d99c6 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Typedefs.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Typedefs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Typedefs.c  -o ${OBJECTDIR}/UTLN_Typedefs.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Typedefs.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/main.o: main.c  .generated_files/flags/CM_V10/e58adadff3e7ed511120b5acc986f629acd37e5a .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/main.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_OSAL.o: UTLN_OSAL.c  .generated_files/flags/CM_V10/e6c93250ba05dc4e603390600535cf856f715973 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_OSAL.o.d 
	@${RM} ${OBJECTDIR}/UTLN_OSAL.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_OSAL.c  -o ${OBJECTDIR}/UTLN_OSAL.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_OSAL.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Flag.o: UTLN_Flag.c  .generated_files/flags/CM_V10/2126c916c5fdaf05eaae91f6ceb902975a60409f .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Flag.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Flag.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Flag.c  -o ${OBJECTDIR}/UTLN_Flag.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Flag.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/RobotObj.o: RobotObj.c  .generated_files/flags/CM_V10/fe25b040cb4548f3666947b27d864cd23ffd3728 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RobotObj.o.d 
	@${RM} ${OBJECTDIR}/RobotObj.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  RobotObj.c  -o ${OBJECTDIR}/RobotObj.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/RobotObj.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/traps.o: traps.c  .generated_files/flags/CM_V10/823ed52a0e73f56cc1db6d642dd94beb263902c4 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/traps.o.d 
	@${RM} ${OBJECTDIR}/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  traps.c  -o ${OBJECTDIR}/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/traps.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_Odometrie.o: UTLN_Odometrie.c  .generated_files/flags/CM_V10/273943cdf277ef7d3462f86237cefcc94abb1e03 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_Odometrie.o.d 
	@${RM} ${OBJECTDIR}/UTLN_Odometrie.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_Odometrie.c  -o ${OBJECTDIR}/UTLN_Odometrie.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_Odometrie.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_POLOLU_Servo.o: UTLN_POLOLU_Servo.c  .generated_files/flags/CM_V10/2cb1ba7f434cf88da724adab37e47c83d685131d .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_POLOLU_Servo.o.d 
	@${RM} ${OBJECTDIR}/UTLN_POLOLU_Servo.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_POLOLU_Servo.c  -o ${OBJECTDIR}/UTLN_POLOLU_Servo.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_POLOLU_Servo.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Asservissement.o: Asservissement.c  .generated_files/flags/CM_V10/124ff9c4dbceb4926848821db3722b8504827535 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Asservissement.o.d 
	@${RM} ${OBJECTDIR}/Asservissement.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Asservissement.c  -o ${OBJECTDIR}/Asservissement.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Asservissement.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/USB/app_device_vendor_basic.o: USB/app_device_vendor_basic.c  .generated_files/flags/CM_V10/b934e24c64c3ed39a37a99ff4f35fbdb81a7c1ca .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/USB" 
	@${RM} ${OBJECTDIR}/USB/app_device_vendor_basic.o.d 
	@${RM} ${OBJECTDIR}/USB/app_device_vendor_basic.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  USB/app_device_vendor_basic.c  -o ${OBJECTDIR}/USB/app_device_vendor_basic.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/USB/app_device_vendor_basic.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_CircularBuffer.o: UTLN_CircularBuffer.c  .generated_files/flags/CM_V10/54eeee6d80e4624803221dbe04ca3d6f715c8579 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_CircularBuffer.o.d 
	@${RM} ${OBJECTDIR}/UTLN_CircularBuffer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_CircularBuffer.c  -o ${OBJECTDIR}/UTLN_CircularBuffer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_CircularBuffer.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/UTLN_CoilGun.o: UTLN_CoilGun.c  .generated_files/flags/CM_V10/6b687f67636af7b75db62f007bfa447143bfb9f .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/UTLN_CoilGun.o.d 
	@${RM} ${OBJECTDIR}/UTLN_CoilGun.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  UTLN_CoilGun.c  -o ${OBJECTDIR}/UTLN_CoilGun.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/UTLN_CoilGun.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o: TrajectoryGeneratorSingleMotor.c  .generated_files/flags/CM_V10/95fcbc2c3c4a8edad800b28219f0eadc37f10f0 .generated_files/flags/CM_V10/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o.d 
	@${RM} ${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  TrajectoryGeneratorSingleMotor.c  -o ${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/TrajectoryGeneratorSingleMotor.o.d"      -mno-eds-warn  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)     -mdfp="${DFP_DIR}/xc16"
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/Carte_Moteur_V9.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/Carte_Moteur_V9.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_REAL_ICE=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	
else
${DISTDIR}/Carte_Moteur_V9.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/Carte_Moteur_V9.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_CM_V10=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	${MP_CC_DIR}\\xc16-bin2hex ${DISTDIR}/Carte_Moteur_V9.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp="${DFP_DIR}/xc16" 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
