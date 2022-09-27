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
FINAL_IMAGE=${DISTDIR}/Robot_Benguet_Aime_new.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Robot_Benguet_Aime_new.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED=../Robot_Benguet_Aime.X/ChipConfig.c ../Robot_Benguet_Aime.X/IO.c ../Robot_Benguet_Aime.X/main.c ../Robot_Benguet_Aime.X/timer.c Robot..c ToolBox.c PWM.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/806812571/ChipConfig.o ${OBJECTDIR}/_ext/806812571/IO.o ${OBJECTDIR}/_ext/806812571/main.o ${OBJECTDIR}/_ext/806812571/timer.o ${OBJECTDIR}/Robot..o ${OBJECTDIR}/ToolBox.o ${OBJECTDIR}/PWM.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/806812571/ChipConfig.o.d ${OBJECTDIR}/_ext/806812571/IO.o.d ${OBJECTDIR}/_ext/806812571/main.o.d ${OBJECTDIR}/_ext/806812571/timer.o.d ${OBJECTDIR}/Robot..o.d ${OBJECTDIR}/ToolBox.o.d ${OBJECTDIR}/PWM.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/806812571/ChipConfig.o ${OBJECTDIR}/_ext/806812571/IO.o ${OBJECTDIR}/_ext/806812571/main.o ${OBJECTDIR}/_ext/806812571/timer.o ${OBJECTDIR}/Robot..o ${OBJECTDIR}/ToolBox.o ${OBJECTDIR}/PWM.o

# Source Files
SOURCEFILES=../Robot_Benguet_Aime.X/ChipConfig.c ../Robot_Benguet_Aime.X/IO.c ../Robot_Benguet_Aime.X/main.c ../Robot_Benguet_Aime.X/timer.c Robot..c ToolBox.c PWM.c



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
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/Robot_Benguet_Aime_new.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP512GM306
MP_LINKER_FILE_OPTION=,--script=p33EP512GM306.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/806812571/ChipConfig.o: ../Robot_Benguet_Aime.X/ChipConfig.c  .generated_files/flags/default/8d46d4a725127eea31502eb03769539611c3708a .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}/_ext/806812571" 
	@${RM} ${OBJECTDIR}/_ext/806812571/ChipConfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/806812571/ChipConfig.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime.X/ChipConfig.c  -o ${OBJECTDIR}/_ext/806812571/ChipConfig.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/806812571/ChipConfig.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/806812571/IO.o: ../Robot_Benguet_Aime.X/IO.c  .generated_files/flags/default/7cc2256860e039bedaddc43b61d113bd4d46fc9b .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}/_ext/806812571" 
	@${RM} ${OBJECTDIR}/_ext/806812571/IO.o.d 
	@${RM} ${OBJECTDIR}/_ext/806812571/IO.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime.X/IO.c  -o ${OBJECTDIR}/_ext/806812571/IO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/806812571/IO.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/806812571/main.o: ../Robot_Benguet_Aime.X/main.c  .generated_files/flags/default/af0a6494fecd1b114fcf6591240eeecc20b4252e .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}/_ext/806812571" 
	@${RM} ${OBJECTDIR}/_ext/806812571/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/806812571/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime.X/main.c  -o ${OBJECTDIR}/_ext/806812571/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/806812571/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/806812571/timer.o: ../Robot_Benguet_Aime.X/timer.c  .generated_files/flags/default/cd0bf05db2aa5ef0e28ade40f79f94eccd2e51c7 .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}/_ext/806812571" 
	@${RM} ${OBJECTDIR}/_ext/806812571/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/806812571/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime.X/timer.c  -o ${OBJECTDIR}/_ext/806812571/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/806812571/timer.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Robot..o: Robot..c  .generated_files/flags/default/652a321f63f8a65d0dfb733b862352a704a4bdcf .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Robot..o.d 
	@${RM} ${OBJECTDIR}/Robot..o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Robot..c  -o ${OBJECTDIR}/Robot..o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Robot..o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ToolBox.o: ToolBox.c  .generated_files/flags/default/953db56702db8dfbe301de3d8f3a2451b79a1667 .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ToolBox.o.d 
	@${RM} ${OBJECTDIR}/ToolBox.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ToolBox.c  -o ${OBJECTDIR}/ToolBox.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ToolBox.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/PWM.o: PWM.c  .generated_files/flags/default/f2f45e5151240df2be3d9e211b6e51d38522acc1 .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PWM.o.d 
	@${RM} ${OBJECTDIR}/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  PWM.c  -o ${OBJECTDIR}/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/PWM.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
else
${OBJECTDIR}/_ext/806812571/ChipConfig.o: ../Robot_Benguet_Aime.X/ChipConfig.c  .generated_files/flags/default/b29414eeb4763148a3f99a7799363b8391beee18 .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}/_ext/806812571" 
	@${RM} ${OBJECTDIR}/_ext/806812571/ChipConfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/806812571/ChipConfig.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime.X/ChipConfig.c  -o ${OBJECTDIR}/_ext/806812571/ChipConfig.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/806812571/ChipConfig.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/806812571/IO.o: ../Robot_Benguet_Aime.X/IO.c  .generated_files/flags/default/f446ec67199fa99223c298f80bcc073617507ac0 .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}/_ext/806812571" 
	@${RM} ${OBJECTDIR}/_ext/806812571/IO.o.d 
	@${RM} ${OBJECTDIR}/_ext/806812571/IO.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime.X/IO.c  -o ${OBJECTDIR}/_ext/806812571/IO.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/806812571/IO.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/806812571/main.o: ../Robot_Benguet_Aime.X/main.c  .generated_files/flags/default/4b8c18104556124388deca920b3d9e22b69514a8 .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}/_ext/806812571" 
	@${RM} ${OBJECTDIR}/_ext/806812571/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/806812571/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime.X/main.c  -o ${OBJECTDIR}/_ext/806812571/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/806812571/main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/_ext/806812571/timer.o: ../Robot_Benguet_Aime.X/timer.c  .generated_files/flags/default/6f701dd44f748fbdd8f94357306fcdd3f7584efb .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}/_ext/806812571" 
	@${RM} ${OBJECTDIR}/_ext/806812571/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/806812571/timer.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Robot_Benguet_Aime.X/timer.c  -o ${OBJECTDIR}/_ext/806812571/timer.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/_ext/806812571/timer.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/Robot..o: Robot..c  .generated_files/flags/default/e3578bac55f623c4c5a6613821a0091bf88471d9 .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Robot..o.d 
	@${RM} ${OBJECTDIR}/Robot..o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Robot..c  -o ${OBJECTDIR}/Robot..o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/Robot..o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/ToolBox.o: ToolBox.c  .generated_files/flags/default/1f7397668b26d8a4c4343210e5934a3778e28a47 .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ToolBox.o.d 
	@${RM} ${OBJECTDIR}/ToolBox.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ToolBox.c  -o ${OBJECTDIR}/ToolBox.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/ToolBox.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
${OBJECTDIR}/PWM.o: PWM.c  .generated_files/flags/default/98fd80c4b3837f70b74ca50fad853a9960047ab4 .generated_files/flags/default/63bb65385b9bcfd57ff63c9d0ae2a7c08640c179
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/PWM.o.d 
	@${RM} ${OBJECTDIR}/PWM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  PWM.c  -o ${OBJECTDIR}/PWM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MP -MMD -MF "${OBJECTDIR}/PWM.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -O0 -msmart-io=1 -Wall -msfr-warn=off    -mdfp="${DFP_DIR}/xc16"
	
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
${DISTDIR}/Robot_Benguet_Aime_new.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/Robot_Benguet_Aime_new.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_ICD4=1  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_ICD4=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	
else
${DISTDIR}/Robot_Benguet_Aime_new.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o ${DISTDIR}/Robot_Benguet_Aime_new.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,${DISTDIR}/memoryfile.xml$(MP_EXTRA_LD_POST)  -mdfp="${DFP_DIR}/xc16" 
	${MP_CC_DIR}\\xc16-bin2hex ${DISTDIR}/Robot_Benguet_Aime_new.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf   -mdfp="${DFP_DIR}/xc16" 
	
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
