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
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Robot_BENGUET_Aime_Final.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Robot_BENGUET_Aime_Final.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../Robot_Benguet_Aime_new.X/ADC.c ../Robot_Benguet_Aime_new.X/CB_RX1.c ../Robot_Benguet_Aime_new.X/CB_TX1.c ../Robot_Benguet_Aime_new.X/ChipConfig.c ../Robot_Benguet_Aime_new.X/IO.c ../Robot_Benguet_Aime_new.X/PWM.c ../Robot_Benguet_Aime_new.X/Robot..c ../Robot_Benguet_Aime_new.X/ToolBox.c ../Robot_Benguet_Aime_new.X/UART.c ../Robot_Benguet_Aime_new.X/main.c ../Robot_Benguet_Aime_new.X/timer.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1180267398/ADC.o ${OBJECTDIR}/_ext/1180267398/CB_RX1.o ${OBJECTDIR}/_ext/1180267398/CB_TX1.o ${OBJECTDIR}/_ext/1180267398/ChipConfig.o ${OBJECTDIR}/_ext/1180267398/IO.o ${OBJECTDIR}/_ext/1180267398/PWM.o ${OBJECTDIR}/_ext/1180267398/Robot..o ${OBJECTDIR}/_ext/1180267398/ToolBox.o ${OBJECTDIR}/_ext/1180267398/UART.o ${OBJECTDIR}/_ext/1180267398/main.o ${OBJECTDIR}/_ext/1180267398/timer.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1180267398/ADC.o.d ${OBJECTDIR}/_ext/1180267398/CB_RX1.o.d ${OBJECTDIR}/_ext/1180267398/CB_TX1.o.d ${OBJECTDIR}/_ext/1180267398/ChipConfig.o.d ${OBJECTDIR}/_ext/1180267398/IO.o.d ${OBJECTDIR}/_ext/1180267398/PWM.o.d ${OBJECTDIR}/_ext/1180267398/Robot..o.d ${OBJECTDIR}/_ext/1180267398/ToolBox.o.d ${OBJECTDIR}/_ext/1180267398/UART.o.d ${OBJECTDIR}/_ext/1180267398/main.o.d ${OBJECTDIR}/_ext/1180267398/timer.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1180267398/ADC.o ${OBJECTDIR}/_ext/1180267398/CB_RX1.o ${OBJECTDIR}/_ext/1180267398/CB_TX1.o ${OBJECTDIR}/_ext/1180267398/ChipConfig.o ${OBJECTDIR}/_ext/1180267398/IO.o ${OBJECTDIR}/_ext/1180267398/PWM.o ${OBJECTDIR}/_ext/1180267398/Robot..o ${OBJECTDIR}/_ext/1180267398/ToolBox.o ${OBJECTDIR}/_ext/1180267398/UART.o ${OBJECTDIR}/_ext/1180267398/main.o ${OBJECTDIR}/_ext/1180267398/timer.o

# Source Files
SOURCEFILES=../Robot_Benguet_Aime_new.X/ADC.c ../Robot_Benguet_Aime_new.X/CB_RX1.c ../Robot_Benguet_Aime_new.X/CB_TX1.c ../Robot_Benguet_Aime_new.X/ChipConfig.c ../Robot_Benguet_Aime_new.X/IO.c ../Robot_Benguet_Aime_new.X/PWM.c ../Robot_Benguet_Aime_new.X/Robot..c ../Robot_Benguet_Aime_new.X/ToolBox.c ../Robot_Benguet_Aime_new.X/UART.c ../Robot_Benguet_Aime_new.X/main.c ../Robot_Benguet_Aime_new.X/timer.c



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
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/Robot_BENGUET_Aime_Final.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP512GM306
MP_LINKER_FILE_OPTION=,--script=p33EP512GM306.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1180267398/ADC.o: ../Robot_Benguet_Aime_new.X/ADC.c  .generated_files/flags/default/7ee1a5e200d2bc95f91b4249da05d855a2950541 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/ADC.c  -o ${OBJECTDIR}/_ext/1180267398/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/ADC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/CB_RX1.o: ../Robot_Benguet_Aime_new.X/CB_RX1.c  .generated_files/flags/default/23000cb64be37247d58b30477fa42a9c5854ca78 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/CB_RX1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/CB_RX1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/CB_RX1.c  -o ${OBJECTDIR}/_ext/1180267398/CB_RX1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/CB_RX1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/CB_TX1.o: ../Robot_Benguet_Aime_new.X/CB_TX1.c  .generated_files/flags/default/ae7bd2423eb8f40a9b5b0741c7fd41033476e3c0 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/CB_TX1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/CB_TX1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/CB_TX1.c  -o ${OBJECTDIR}/_ext/1180267398/CB_TX1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/CB_TX1.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/ChipConfig.o: ../Robot_Benguet_Aime_new.X/ChipConfig.c  .generated_files/flags/default/a1f50cfb2ea7d2f177747f13abb8a49a7b3ca963 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ChipConfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ChipConfig.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/ChipConfig.c  -o ${OBJECTDIR}/_ext/1180267398/ChipConfig.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/ChipConfig.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/IO.o: ../Robot_Benguet_Aime_new.X/IO.c  .generated_files/flags/default/225f604a01a2fd18a9d4c97e612d33d180e65809 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/IO.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/IO.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/IO.c  -o ${OBJECTDIR}/_ext/1180267398/IO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/IO.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/PWM.o: ../Robot_Benguet_Aime_new.X/PWM.c  .generated_files/flags/default/859a9c316472c3685f9eed03ea6c56306c8a77b6 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/PWM.c  -o ${OBJECTDIR}/_ext/1180267398/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/PWM.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/Robot..o: ../Robot_Benguet_Aime_new.X/Robot..c  .generated_files/flags/default/836bbab92884deaadddd4c8bc49d66d554c4ecd8 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/Robot..o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/Robot..o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/Robot..c  -o ${OBJECTDIR}/_ext/1180267398/Robot..o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/Robot..o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/ToolBox.o: ../Robot_Benguet_Aime_new.X/ToolBox.c  .generated_files/flags/default/228842efb96a6c7667b5b29381afd097dd228f64 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ToolBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ToolBox.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/ToolBox.c  -o ${OBJECTDIR}/_ext/1180267398/ToolBox.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/ToolBox.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/UART.o: ../Robot_Benguet_Aime_new.X/UART.c  .generated_files/flags/default/a0cc69c03e9d975d174438719238532608dd811c .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/UART.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/UART.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/UART.c  -o ${OBJECTDIR}/_ext/1180267398/UART.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/UART.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/main.o: ../Robot_Benguet_Aime_new.X/main.c  .generated_files/flags/default/78a32e5a93a9be17a3c2df0ae5dc6e54ab9b5f26 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/main.c  -o ${OBJECTDIR}/_ext/1180267398/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/timer.o: ../Robot_Benguet_Aime_new.X/timer.c  .generated_files/flags/default/785b586b842e12b0515878d844337954beadc7d3 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/timer.c  -o ${OBJECTDIR}/_ext/1180267398/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/timer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/_ext/1180267398/ADC.o: ../Robot_Benguet_Aime_new.X/ADC.c  .generated_files/flags/default/e239642298be50420f580ff57592f5481d07c767 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ADC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/ADC.c  -o ${OBJECTDIR}/_ext/1180267398/ADC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/ADC.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/CB_RX1.o: ../Robot_Benguet_Aime_new.X/CB_RX1.c  .generated_files/flags/default/a819dbf5388f1b0773454183aa0d76db9cfc8b8e .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/CB_RX1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/CB_RX1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/CB_RX1.c  -o ${OBJECTDIR}/_ext/1180267398/CB_RX1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/CB_RX1.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/CB_TX1.o: ../Robot_Benguet_Aime_new.X/CB_TX1.c  .generated_files/flags/default/b60ed6389b01c7ce90518b960c4e87f239db45e7 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/CB_TX1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/CB_TX1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/CB_TX1.c  -o ${OBJECTDIR}/_ext/1180267398/CB_TX1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/CB_TX1.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/ChipConfig.o: ../Robot_Benguet_Aime_new.X/ChipConfig.c  .generated_files/flags/default/bed9d5c4d8142a1d6e4d6cf980debd9c705be755 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ChipConfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ChipConfig.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/ChipConfig.c  -o ${OBJECTDIR}/_ext/1180267398/ChipConfig.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/ChipConfig.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/IO.o: ../Robot_Benguet_Aime_new.X/IO.c  .generated_files/flags/default/25ba57eef9d22b7ef477442fa476b263deb0f987 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/IO.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/IO.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/IO.c  -o ${OBJECTDIR}/_ext/1180267398/IO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/IO.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/PWM.o: ../Robot_Benguet_Aime_new.X/PWM.c  .generated_files/flags/default/494aa22c8a34b81e0ecca8e0a75142a86ced38d6 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/PWM.c  -o ${OBJECTDIR}/_ext/1180267398/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/PWM.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/Robot..o: ../Robot_Benguet_Aime_new.X/Robot..c  .generated_files/flags/default/e2b214549e474270104359ca89dce356417b8344 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/Robot..o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/Robot..o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/Robot..c  -o ${OBJECTDIR}/_ext/1180267398/Robot..o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/Robot..o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/ToolBox.o: ../Robot_Benguet_Aime_new.X/ToolBox.c  .generated_files/flags/default/61bbfa4e09ed50243a82860e3f5f406fb2776756 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ToolBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/ToolBox.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/ToolBox.c  -o ${OBJECTDIR}/_ext/1180267398/ToolBox.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/ToolBox.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/UART.o: ../Robot_Benguet_Aime_new.X/UART.c  .generated_files/flags/default/86f3f424aedefe4b171a6a99815dbef1ef836593 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/UART.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/UART.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/UART.c  -o ${OBJECTDIR}/_ext/1180267398/UART.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/UART.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/main.o: ../Robot_Benguet_Aime_new.X/main.c  .generated_files/flags/default/dc3ef548606ba8deba3317da487a80fa0c901953 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/main.c  -o ${OBJECTDIR}/_ext/1180267398/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/1180267398/timer.o: ../Robot_Benguet_Aime_new.X/timer.c  .generated_files/flags/default/45fc1d3b6ade6edb08b2f6364ae41fa342b805f8 .generated_files/flags/default/54850bb106185342aa34b1544b5c9533308813ce
	@${MKDIR} "${OBJECTDIR}/_ext/1180267398" 
	@${RM} ${OBJECTDIR}/_ext/1180267398/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1180267398/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime_new.X/timer.c  -o ${OBJECTDIR}/_ext/1180267398/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/1180267398/timer.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
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
${DISTDIR}/Robot_BENGUET_Aime_Final.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/Robot_BENGUET_Aime_Final.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	
else
${DISTDIR}/Robot_BENGUET_Aime_Final.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/Robot_BENGUET_Aime_Final.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	${MP_CC_DIR}\\xc16-bin2hex ${DISTDIR}/Robot_BENGUET_Aime_Final.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp="${DFP_DIR}/xc16" 
	
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
