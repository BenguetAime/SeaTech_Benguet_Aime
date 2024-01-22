#include "UTLN_CoilGun.h"
#include "HardwareProfile_CM_V9.h"

TirParameters tirParams;
extern unsigned volatile long g_longTimeStampTirUs;
//void GenerateTir1Coil(int msCoil1)
//{
//    //Si on a pas deja une commande de tir en cour:
//    if(!tirParams.tirInProgress)
//    {
//        if(msCoil1!=0)
//        {
//            tirParams.msCoil1=msCoil1;
//            tirParams.tirInProgress=true;
//        }
//    }
//}
//
//void GenerateTir2Coil(int msCoil1, int msCoil2, int msInterCoil2)
//{
//    //Si on a pas deja une commande de tir en cour:
//    if(!tirParams.tirInProgress)
//    {
//        tirParams.tirInProgress=true;
//        tirParams.msOffsetForCoil2=msInterCoil2;
//        tirParams.msCoil2=msCoil2;
//    }
//}
//
//void GenerateTir3Coil(uint16_t msCoil1, uint16_t msCoil2,  uint16_t msCoil3, uint16_t msInterCoil2,uint16_t msInterCoil3)
//{
//    //Si on a pas deja une commande de tir en cour:
//    if(!tirParams.tirInProgress)
//    {
//        tirParams.tirInProgress=true;
//        tirParams.msOffsetForCoil2=msInterCoil2;
//        tirParams.msOffsetForCoil3=msInterCoil3;
//        tirParams.msCoil2=msCoil2;
//        tirParams.msCoil3=msCoil3;
//    }
//}

void GenerateTir4Coil(uint16_t usCoil1, uint16_t usCoil2,  uint16_t usCoil3, uint16_t usCoil4, uint16_t usInterCoil1, uint16_t usInterCoil2, uint16_t usInterCoil3, uint16_t usInterCoil4)
{
    //Si on a pas deja une commande de tir en cour:
    if(!tirParams.tirInProgress)
    {    
        tirParams.usOffsetForCoil1=usInterCoil1;
        tirParams.msOffsetForCoil2=usInterCoil2;
        tirParams.msOffsetForCoil3=usInterCoil3;
        tirParams.msOffsetForCoil4=usInterCoil4;
        tirParams.msCoil1=usCoil1;
        tirParams.msCoil2=usCoil2;
        tirParams.msCoil3=usCoil3;
        tirParams.msCoil4=usCoil4;
        
        tirParams.tsDebutTirCoil1 = (g_longTimeStampTirUs + usInterCoil1);
        tirParams.tsFinTirCoil1 = g_longTimeStampTirUs + usCoil1 + usInterCoil1;
        tirParams.tsDebutTirCoil2 = g_longTimeStampTirUs + usInterCoil2;
        tirParams.tsFinTirCoil2 = g_longTimeStampTirUs + usCoil2+usInterCoil2;
        tirParams.tsDebutTirCoil3 = g_longTimeStampTirUs + usInterCoil3;
        tirParams.tsFinTirCoil3 = g_longTimeStampTirUs + usCoil3+usInterCoil3;
        tirParams.tsDebutTirCoil4 = g_longTimeStampTirUs + usInterCoil4;
        tirParams.tsFinTirCoil4 = g_longTimeStampTirUs + usCoil4+usInterCoil4;
        tirParams.tirInProgress=true;
               
        
        if(usCoil1==0)
        {
            //On desactive la bobine 1, et on passe directement a l'etape attente(InterCoil2)
            tirParams.msCoil1=0;
        }
        else
        {
            tirParams.msCoil1=usCoil1;
            
        }
    }
}

bool toggle=false;
bool totoInterne = 0;
inline void TirStateMachine(void)
{    
    
//TODO: Bug bizarre lorsqu'on touche a une autre led (par exemple _LED_B1) dans la boucle principale,
//On a une sorte de re-entrance, ou une valeur qui ne correspond pas    
//    TIR_1 = !TIR_1;
//    TIR_2 = !TIR_2;
//    TIR_3 = !TIR_3;
//    TIR_4 = !TIR_4;
//    //ENABLE_TIR=!ENABLE_TIR;
//    if(TIR_2!=TIR_4)
//    {
//        ENABLE_TIR=!ENABLE_TIR;
//    }
    if(tirParams.tirInProgress)
    {
        if(g_longTimeStampTirUs >= tirParams.tsDebutTirCoil1 && g_longTimeStampTirUs < tirParams.tsFinTirCoil1)
            TIR_1=1;
        else
            TIR_1=0;
        if(g_longTimeStampTirUs >= tirParams.tsDebutTirCoil2 && g_longTimeStampTirUs < tirParams.tsFinTirCoil2)
            TIR_2=1;
        else
            TIR_2=0;
        if(g_longTimeStampTirUs >= tirParams.tsDebutTirCoil3 && g_longTimeStampTirUs < tirParams.tsFinTirCoil3)
            TIR_3=1;
        else
            TIR_3=0;
        if(g_longTimeStampTirUs >= tirParams.tsDebutTirCoil4 && g_longTimeStampTirUs < tirParams.tsFinTirCoil4)
            TIR_4=1;
        else
            TIR_4=0;
    }
    else
    {
        TIR_1=0;
        TIR_2=0;
        TIR_3=0;
        TIR_4=0;        
    }
    
    //Test pour savoir si on a terminé une séquence de tir
    if(g_longTimeStampTirUs > tirParams.tsFinTirCoil1 &&
            g_longTimeStampTirUs > tirParams.tsFinTirCoil2 &&
            g_longTimeStampTirUs > tirParams.tsFinTirCoil3 &&
            g_longTimeStampTirUs > tirParams.tsFinTirCoil4)
    {
        tirParams.tirInProgress=false;
    }
}