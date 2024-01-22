#include "UTLN_Button.h"

unsigned char clickSequenceIndex = 0;

#ifdef BUTTON_V1
CLICK_SEQUENCE_bits clickSequence;
CLICK_SEQUENCE_bits lastClickSequence;
#ifdef USE_LONG_CLICK
    CLICK_SEQUENCE_bits longClickSequence;
    unsigned char longClickSequenceIndex = 0;
    CLICK_SEQUENCE_bits lastLongClickSequence;
#endif


    
unsigned int sw1PushedTimeStamp=0;
unsigned int sw1ReleasedTimeStamp=0;
unsigned int sw1LastClickTimeStamp=0;

unsigned int sw2PushedTimeStamp=0;
unsigned int sw2ReleasedTimeStamp=0;
unsigned int sw2LastClickTimeStamp=0;

unsigned int sw3PushedTimeStamp=0;
unsigned int sw3ReleasedTimeStamp=0;
unsigned int sw3LastClickTimeStamp=0;
#endif
#ifdef BUTTON_V2
uint8 lastClickSequence[8];
uint8 clickSequence[8];                     //Tableau correspondant a la sequence en cour
BUTTON_CONFIG_bits buttonConfig[BUTTON_COUNT];
uint8 sequenceCount;                        //Variable representant le nombre de sequences enregistrées
BUTTON_SEQUENCE_bits sequence[SEQ_COUNT];   //Tableau des sequences enregistrées
uint8 buttonRegistered=0;
unsigned int buttonClickInProgress=0;
#ifdef USE_LONG_CLICK
    BUTTON_SEQUENCE_bits longClickSequence[SEQ_COUNT];   //Tableau des sequences de click longs enregistrées
    uint8 longClick!sequenceIndex=0;
#endif

void ButtonRegister(uint8 buttonNum, unsigned int* buttonPort, uint8 buttonPin)
{
    if(buttonNum<=BUTTON_COUNT)
    {
        buttonConfig[buttonNum-1].port=buttonPort;
        buttonConfig[buttonNum-1].pin=buttonPin;
        buttonRegistered++;
    }
}
void ButtonRegisterWithCallBack(uint8 buttonNum, volatile unsigned int* buttonPort, uint8 buttonPin,BUTTON_CallBack callback)
{
    if(buttonNum<=BUTTON_COUNT)
    {
        buttonConfig[buttonNum-1].port=buttonPort;
        buttonConfig[buttonNum-1].pin=buttonPin;
        if(callback!=NULL)
            buttonConfig[buttonNum-1].clickCallback=callback;
        buttonRegistered++;
    }
}
void ButtonClickCallbackRegister(uint8 buttonNum,BUTTON_CallBack callback)
{
    if(buttonNum<=BUTTON_COUNT)
    {
        if(callback!=NULL)
            buttonConfig[buttonNum-1].clickCallback=callback;
    }
}
void ButtonSequenceRegister(uint8 *buttonSequence, uint8 sequenceLength, BUTTON_CallBack callback)
{
    sequence[sequenceCount].sequenceLen=sequenceLength;
    uint8 i;
    for(i=0;i<sequence[sequenceCount].sequenceLen;i++)
        sequence[sequenceCount].seq[i]=buttonSequence[i];
    if(callback!=NULL)
        sequence[sequenceCount].seqCallback=callback;
    sequenceCount++;
}
void ButtonLongClickRegister(uint8 buttonNum, unsigned int pressTime, BUTTON_CallBack callback)
{
    if(buttonNum<=BUTTON_COUNT)
    {
        buttonConfig[buttonNum-1].specialPressTime=pressTime;
        if(callback!=NULL)
            buttonConfig[buttonNum-1].longClickcallBack=callback;
    }
}
void ClickSequenceReset(void)
{
    uint8 i;
    for(i=0;i<8;i++)
    {
        clickSequence[i]=00;
        #ifdef USE_LONG_CLICK
            longClickSequence[i]=0;
        #endif
    }
    clickSequenceIndex = 0;
    #ifdef USE_LONG_CLICK
        longClickSequenceIndex=0;
    #endif
}

void ClickSequenceAdd(uint8 numButton)
{
    clickSequence[clickSequenceIndex++]=numButton;
}
#ifdef USE_LONG_CLICK    
    void LongClickSequenceAdd(unsigned char numButton)
    {
        longClickSequence[longClickSequenceIndex++]=numButton;
    }
#endif
    
void SwPushed(uint8 buttonNum)
{
    buttonConfig[buttonNum-1].swLastState=1;
        if(((unsigned int)g_longTimeStamp + 30 > buttonConfig[buttonNum-1].swReleasedTimeStamp) || (buttonConfig[buttonNum-1].swReleasedTimeStamp > (unsigned int)g_longTimeStamp))
        {
            buttonClickInProgress |= 1<<(buttonNum-1);
            buttonConfig[buttonNum-1].swPushedTimeStamp = (volatile unsigned int)g_longTimeStamp;
            buttonConfig[buttonNum-1].swReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            //200 ms se sont passées depuis le relachement bouton, c'est un appui valide
        }
}

void SwReleased(uint8 buttonNum)
{    
        if(((((unsigned int)g_longTimeStamp + 30) > (buttonConfig[buttonNum-1].swPushedTimeStamp + 700)) || (buttonConfig[buttonNum-1].swReleasedTimeStamp > (unsigned int)g_longTimeStamp)) && flagInterface.ButtonClickSequenceInProgress)
        {
            buttonConfig[buttonNum-1].swReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            #ifdef USE_LONG_CLICK
                LongClickSequenceAdd(i);
            #endif
        }
        else if((((unsigned int)g_longTimeStamp + 30 > buttonConfig[buttonNum-1].swPushedTimeStamp)|| (buttonConfig[buttonNum-1].swReleasedTimeStamp > (unsigned int)g_longTimeStamp)) && buttonClickInProgress & 1<<(buttonNum-1))
        {
            buttonConfig[buttonNum-1].swReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            ClickSequenceAdd(buttonNum);
            buttonConfig[buttonNum-1].swLastState=0;
        }
        else    //Sinon c'est qu'on a relaché le bouton alors qu'on est passé en sleep
        {
//            if(PowerManagementStateMachineGetState() == PM_STATE_SLEEP)
//                flagInterface.GoBackToSleepEvent = TRUE;
        }
}
    
    
void IsSequenceFinished(void)
{            
    uint8 i;
    for(i=0;i<buttonRegistered;i++)
    {
        if(buttonClickInProgress & 1<<i && ( (*buttonConfig[i].port)&(1U << buttonConfig[i].pin) !=SW1_ACTIVE_STATE))
        {
            //On a une séquence en cours
            if((((unsigned int)g_longTimeStamp - buttonConfig[i].swReleasedTimeStamp) > 800) && buttonConfig[i].swLastState==0/*&& flagInterface.Sw2LastState == 0*/ )
            {
                //La séquence est terminée
                AnalyzeClickSequence();
                buttonClickInProgress &= ~(1<<i);
            }
        }
        else
        {
            if(buttonClickInProgress != 0 && clickSequenceIndex==0 && buttonConfig[i].specialPressTime!=0 && (((unsigned int)g_longTimeStamp - buttonConfig[i].swReleasedTimeStamp) > buttonConfig[i].specialPressTime))
            {
                ClickSequenceReset();
                buttonConfig[i].swReleasedTimeStamp=buttonConfig[i].swPushedTimeStamp=g_longTimeStamp;
                if(buttonConfig[i].longClickcallBack!=NULL)
                    (*buttonConfig[i].longClickcallBack)();      //Appel du callback enregistré precedemment
            }
        }
    }
}

void AnalyzeClickSequence(void)
{
    uint8 i;
    for(i=0;i<8;i++)
        lastClickSequence[i] = clickSequence[i];
    #ifdef USE_LONG_CLICK
        lastLongClickSequence = longClickSequence;
    #endif
    ClickSequenceReset();

    #ifdef HW_SW1
        //uint8 i;
        for(i=0;i<buttonRegistered;i++)
        {
            if((lastClickSequence[0] == i+1) && (lastClickSequence[1] == 0))
            {
                //Click simple sur bouton 1
                if(buttonConfig[i].clickCallback!=NULL)
                    (*buttonConfig[i].clickCallback)();
            }
        }
    
        for(i=0;i<sequenceCount;i++)
        {
            uint8 j,match=0;
            for(j=0;j<sequence[i].sequenceLen;j++)
            {
                if(lastClickSequence[j]==sequence[i].seq[j])
                    match++;
            }
            if(match==sequence[i].sequenceLen)
                if(sequence[i].seqCallback!=NULL)
                    (*sequence[i].seqCallback)();
            
        }
        #ifdef USE_LONG_CLICK
        for(i=0;i<longClickSequenceIndex;i++)
        {
            uint8 j,match=0;
            for(j=0;j<longClickSequence[i].sequenceLen;j++)
            {
                if(lastLongClickSequence[j]==sequence[i].seq[j])
                    match++;
            }
            if(match==longClickSequence[i].sequenceLen)
                if(longClickSequence[i].seqCallback!=NULL)
                    (*longClickSequence[i].seqCallback)();
        }
        #endif       
    #endif 
}
#endif

#ifdef BUTTON_V1
void ClickSequenceReset(void)
{
    clickSequence.ALL = 0x0000;
    clickSequenceIndex = 0;
    #ifdef USE_LONG_CLICK
        longClickSequence.ALL=0;
        longClickSequenceIndex=0;
    #endif
}

void ClickSequenceAdd(unsigned char numButton)
{
    switch (clickSequenceIndex)
    {
        case 0: clickSequence.click1 = numButton; clickSequenceIndex++; break;
        case 1: clickSequence.click2 = numButton; clickSequenceIndex++; break;
        case 2: clickSequence.click3 = numButton; clickSequenceIndex++; break;
        case 3: clickSequence.click4 = numButton; clickSequenceIndex++; break;
        case 4: clickSequence.click5 = numButton; clickSequenceIndex++; break;
        case 5: clickSequence.click6 = numButton; clickSequenceIndex++; break;
        case 6: clickSequence.click7 = numButton; clickSequenceIndex++; break;
        case 7: clickSequence.click8 = numButton; clickSequenceIndex++; break;
        default: break;
    }
}
#ifdef USE_LONG_CLICK    
    void LongClickSequenceAdd(unsigned char numButton)
    {
        switch (longClickSequenceIndex)
        {
            case 0: longClickSequence.click1 = numButton; longClickSequenceIndex++; break;
            case 1: longClickSequence.click2 = numButton; longClickSequenceIndex++; break;
            case 2: longClickSequence.click3 = numButton; longClickSequenceIndex++; break;
            case 3: longClickSequence.click4 = numButton; longClickSequenceIndex++; break;
            case 4: longClickSequence.click5 = numButton; longClickSequenceIndex++; break;
            case 5: longClickSequence.click6 = numButton; longClickSequenceIndex++; break;
            case 6: longClickSequence.click7 = numButton; longClickSequenceIndex++; break;
            case 7: longClickSequence.click8 = numButton; longClickSequenceIndex++; break;
            default: break;
        }
    }
#endif


void IsSequenceFinished(void)
{
    if(flagInterface.ButtonClickSequenceInProgress == TRUE && SW1!=SW1_ACTIVE_STATE)
    {
        //On a une séquence en cours
        #if ((defined HW_SW1) && (defined HW_SW2))
            if((((unsigned int)g_longTimeStamp - sw1ReleasedTimeStamp) > 800) && (((unsigned int)g_longTimeStamp - sw2ReleasedTimeStamp) > 800) && (((unsigned int)g_longTimeStamp - sw3ReleasedTimeStamp) > 800) && flagInterface.Sw1LastState == 0 && flagInterface.Sw2LastState == 0 && flagInterface.Sw3LastState == 0)
        #endif
        #if ((defined HW_SW1) && (!defined HW_SW2))
            if((((unsigned int)g_longTimeStamp - sw1ReleasedTimeStamp) > 800))
        #endif
        {
            //La séquence est terminée
            AnalyzeClickSequence();
        }
    }
    else
    {
        if(flagInterface.ButtonClickSequenceInProgress == TRUE && clickSequenceIndex==0 && (((unsigned int)g_longTimeStamp - sw1ReleasedTimeStamp) > 5000))
            {
                ClickSequenceReset();
                sw1ReleasedTimeStamp=sw1PushedTimeStamp=g_longTimeStamp;
                flagInterface.Sw1LongPressEvent=TRUE;
            }

        #ifdef SWITCH_LONG_PRESS_SLEEPWAKEUP
            if(flagInterface.ButtonClickSequenceInProgress == TRUE && clickSequenceIndex==0 && (((unsigned int)g_longTimeStamp - sw1ReleasedTimeStamp) > 1500))
            {
                ClickSequenceReset();
                sw1ReleasedTimeStamp=sw1PushedTimeStamp=g_longTimeStamp;
                flagInterface.Sw1LongPressEvent=TRUE;
            }
        #endif
        //Special case: 5x click and maintain 5th for 1 sec to entering bootloader
        #ifdef SWITCH_SPECIAL_CLICK_ENTER_BOOTLOADER
            if(flagInterface.ButtonClickSequenceInProgress == TRUE && clickSequenceIndex==4 && (((unsigned int)g_longTimeStamp - sw1ReleasedTimeStamp) > 1000))
            {
                #ifdef USE_ZIGBEE
                    CC2530ModuleReset();          //On reset le CC
                #endif
                #ifdef PROGRAMMABLE_WITH_USB_HID_BOOTLOADER
                    EnterBootloader();
                #endif
            }
        #endif
    }
}

    void AnalyzeClickSequence(void)
    {
        lastClickSequence = clickSequence;
        #ifdef USE_LONG_CLICK
            lastLongClickSequence = longClickSequence;
        #endif
        ClickSequenceReset();

        #ifdef HW_SW1
            #ifdef USE_LONG_CLICK
                if((lastLongClickSequence.click1 == 1) && (lastLongClickSequence.click2 == 0))
                {
                    //Click long sur bouton 1
                    Sw1OnLongClick();
                }
                else
            #endif
            if((lastClickSequence.click1 == 1) && (lastClickSequence.click2 == 0))
            {
                //Click simple sur bouton 1
                Sw1OnClick();
            }
            else if((lastClickSequence.click1 == 1) && (lastClickSequence.click2 == 1) && (lastClickSequence.click3 == 0))
            {
                //Double click simple sur bouton 1
                Sw1OnDoubleClick();
            }
            else if((lastClickSequence.click1 == 1) && (lastClickSequence.click2 == 1) && (lastClickSequence.click3 == 1) && (lastClickSequence.click4 == 0))
            {
                //Triple click simple sur bouton 1
                Sw1OnTripleClick();
            }
            else if((lastClickSequence.click1 == 1) && (lastClickSequence.click2 == 1) && (lastClickSequence.click3 == 1) && (lastClickSequence.click4 == 1) && (lastClickSequence.click5 == 0))
            {
                //Quadruple click simple sur bouton 1
                Sw1OnQuadrupleClick();
            }
        else if((lastClickSequence.click1 == 1) && (lastClickSequence.click2 == 1) && (lastClickSequence.click3 == 1) && (lastClickSequence.click4 == 1) && (lastClickSequence.click5 == 1)&& (lastClickSequence.click6 == 0))
            {
                //5 click simple sur bouton 1
                Sw1OnFiveClick();
            }
        else if((lastClickSequence.click1 == 1) && (lastClickSequence.click2 == 1) && (lastClickSequence.click3 == 1) && (lastClickSequence.click4 == 1) && (lastClickSequence.click5 == 1)&& (lastClickSequence.click6 == 1)&& (lastClickSequence.click7 == 0))
            {
                //6 click simple sur bouton 1
                Sw1OnSixtClick();
            }
        else if((lastClickSequence.click1 == 1) && (lastClickSequence.click2 == 1) && (lastClickSequence.click3 == 1) && (lastClickSequence.click4 == 1) && (lastClickSequence.click5 == 1) &&
                (lastClickSequence.click6 == 1) && (lastClickSequence.click7 == 1)&& (lastClickSequence.click8 == 0))
            {
                //7 click simple sur bouton 1
                Sw1OnSeventhClick();
            }
        #endif
        
        #ifdef HW_SW2
            if((lastClickSequence.click1 == 2) && (lastClickSequence.click2 == 0))
            {
                //Click simple sur bouton 1
                Sw2OnClick();
            }
            else if((lastClickSequence.click1 == 2) && (lastClickSequence.click2 == 2) && (lastClickSequence.click3 == 0))
            {
                //Double click simple sur bouton 1
                Sw2OnDoubleClick();
            }
            else if((lastClickSequence.click1 == 2) && (lastClickSequence.click2 == 2) && (lastClickSequence.click3 == 2) && (lastClickSequence.click4 == 0))
            {
                //Triple click simple sur bouton 1
                Sw2OnTripleClick();
            }
        #endif

        #ifdef HW_SW3
            if((lastClickSequence.click1 == 3) && (lastClickSequence.click2 == 0))
            {
                //Click simple sur bouton 3
                Sw3OnClick();
            }
            else if((lastClickSequence.click1 == 3) && (lastClickSequence.click2 == 3) && (lastClickSequence.click3 == 0))
            {
                //Double click simple sur bouton 3
                Sw3OnDoubleClick();
            }
            else if((lastClickSequence.click1 == 3) && (lastClickSequence.click2 == 3) && (lastClickSequence.click3 == 3) && (lastClickSequence.click4 == 0))
            {
                //Triple click simple sur bouton 3
                Sw3OnTripleClick();
            }
        #endif
    }



    void InitSW1(void)
    {
        //SW1
        PADCONbits.IOCON=1;         //Interrupt on Change enable bit
        IOCPAbits.IOCPA15=1;        //Enable Low to High transition on RA15
        IEC1bits.CNIE=1;
        INTCON2bits.GIE=1;          //Enable Global interrupt
    }

    void Sw1Pushed(void)
    {
        if(((unsigned int)g_longTimeStamp + 30 > sw1ReleasedTimeStamp) || (sw1ReleasedTimeStamp > (unsigned int)g_longTimeStamp))
        {
            flagInterface.ButtonClickSequenceInProgress = TRUE;
            sw1PushedTimeStamp = (unsigned int)g_longTimeStamp;
            sw1ReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            //200 ms se sont passées depuis le relachement bouton, c'est un appui valide
        }
    }

    void Sw1Released(void)
    {
        if(((((unsigned int)g_longTimeStamp + 30) > (sw1PushedTimeStamp + 700)) || (sw1ReleasedTimeStamp > (unsigned int)g_longTimeStamp)) && flagInterface.ButtonClickSequenceInProgress)
        {
            sw1ReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            #ifdef USE_LONG_CLICK
                LongClickSequenceAdd(1);
            #endif
        }
        else if((((unsigned int)g_longTimeStamp + 30 > sw1PushedTimeStamp)|| (sw1ReleasedTimeStamp > (unsigned int)g_longTimeStamp)) && flagInterface.ButtonClickSequenceInProgress)
        {
            sw1ReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            ClickSequenceAdd(1);
        }
        else    //Sinon c'est qu'on a relaché le bouton alors qu'on est passé en sleep
        {
//            if(PowerManagementStateMachineGetState() == PM_STATE_SLEEP)
//                flagInterface.GoBackToSleepEvent = TRUE;
        }
    }

    void Sw1OnClick(void)
    {
        flagInterface.Sw1ClickEvent = TRUE;
    }
    void Sw1OnLongClick(void)
    {
        flagInterface.Sw1LongClickEvent = TRUE;
    }
    void Sw1OnDoubleClick(void)
    {
        flagInterface.Sw1DoubleClickEvent=TRUE;
    }
    void Sw1OnTripleClick(void)
    {
        switch(PowerManagementStateMachineGetState()){
            case PM_STATE_ACTIVE:
                //Si on est en état actif
                flagInterface.Sw1TripleClickEvent = TRUE;
                break;
            default:
                break;
        }
    }

    void Sw1OnQuadrupleClick(void)
    {
        switch(PowerManagementStateMachineGetState()){
            case PM_STATE_ACTIVE:
                //Si on est en état actif
                flagInterface.Sw1QuadrupleClickEvent = TRUE;
                break;
            default:
                break;
        }
    }
    void Sw1OnFiveClick(void)
    {
        flagInterface.Sw1FiveClickEvent=TRUE;
    }
    void Sw1OnSixtClick(void)
    {
        flagInterface.Sw1SixtClickEvent=TRUE;
    }
    void Sw1OnSeventhClick(void)
    {
        flagInterface.Sw1SeventhClickEvent=TRUE;
    }



    void InitSW2(void)
    {
        //SW1
        PADCONbits.IOCON=1;         //Interrupt on Change enable bit
        IOCPAbits.IOCPA14=1;        //Enable Low to High transition on RA15
        IEC1bits.CNIE=1;
        INTCON2bits.GIE=1;          //Enable Global interrupt
    }

    void Sw2Pushed(void)
    {
        flagInterface.ButtonClickSequenceInProgress = TRUE;
        if(((unsigned int)g_longTimeStamp + 30 > sw2ReleasedTimeStamp) || (sw2ReleasedTimeStamp > (unsigned int)g_longTimeStamp))
        {
            sw2PushedTimeStamp = (unsigned int)g_longTimeStamp;
            sw2ReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            //200 ms se sont passées depuis le relachement bouton, c'est un appui valide
        }
    }

    void Sw2Released(void)
    {
        if(((((unsigned int)g_longTimeStamp + 30) > (sw1PushedTimeStamp + 5000)) || (sw1ReleasedTimeStamp > (unsigned int)g_longTimeStamp)) && flagInterface.ButtonClickSequenceInProgress)
        {
            sw1ReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            #ifdef USE_LONG_CLICK
                LongClickSequenceAdd(1);
            #endif
        }
        else if(((unsigned int)g_longTimeStamp + 30 > sw2PushedTimeStamp) || (sw2PushedTimeStamp > (unsigned int)g_longTimeStamp))
        {
            sw2ReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            ClickSequenceAdd(2);
        }
    }

    void Sw2OnClick(void)
    {
        flagInterface.Sw2ClickEvent = TRUE;
    }

    void Sw2OnLongClick(void)
    {
        flagInterface.Sw2LongClickEvent = TRUE;
    }
    void Sw2OnDoubleClick(void)
    {

    }

    void Sw2OnTripleClick(void)
    {
        switch(PowerManagementStateMachineGetState()){
            case PM_STATE_ACTIVE:
                //Si on est en état actif
                flagInterface.Sw2TripleClickEvent = TRUE;
                break;
            default:
                break;
        }
    }
    
    #ifdef HW_SW3
    void Sw3Pushed(void)
    {
        flagInterface.ButtonClickSequenceInProgress = TRUE;
        if(((unsigned int)g_longTimeStamp + 30 > sw3ReleasedTimeStamp) || (sw3ReleasedTimeStamp > (unsigned int)g_longTimeStamp))
        {
            sw3PushedTimeStamp = (unsigned int)g_longTimeStamp;
            sw3ReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            //200 ms se sont passées depuis le relachement bouton, c'est un appui valide
        }
    }

    void Sw3Released(void)
    {
        if(((unsigned int)g_longTimeStamp + 30 > sw3PushedTimeStamp) || (sw3PushedTimeStamp > (unsigned int)g_longTimeStamp))
        {
            sw3ReleasedTimeStamp = (unsigned int)g_longTimeStamp;
            ClickSequenceAdd(3);
        }
    }
    void Sw3OnClick(void)
    {
        flagInterface.Sw3ClickEvent = TRUE;
    }
    void Sw3OnLongClick(void)
    {
        flagInterface.Sw3LongClickEvent = TRUE;
    }
    void Sw3OnDoubleClick(void)
    {
        flagInterface.Sw3DoubleClickEvent=TRUE;
    }
    void Sw3OnTripleClick(void)
    {
        switch(PowerManagementStateMachineGetState()){
            case PM_STATE_ACTIVE:
                //Si on est en état actif
                flagInterface.Sw3TripleClickEvent = TRUE;
                break;
            default:
                break;
        }
    }

    void InitSW3(void)
    {
        //SW1
        PADCONbits.IOCON=1;         //Interrupt on Change enable bit
        IOCPCbits.IOCPC1=1;        //Enable Low to High transition on Rc1
        IEC1bits.CNIE=1;
        INTCON2bits.GIE=1;          //Enable Global interrupt
    }
    #endif
    
    
#endif