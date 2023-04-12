#include <xc.h>
#include "UART_Protocol.h"
#include "UART.h"
#include "CB_TX1.h"
#include "main.h"


unsigned char UartCalculateChecksum(int msgFunction,int msgPayloadLength, unsigned char* msgPayload)
{
            //Fonction prenant entree la trame et sa longueur pour calculer le checksum
            unsigned char checksum = 0;
            checksum ^= 0xFE;
            checksum ^= (unsigned char)(msgFunction>>8);
            checksum ^= (unsigned char)(msgFunction >>0);
            checksum ^= (unsigned char) (msgPayloadLength>>8);
            checksum ^= (unsigned char)(msgPayloadLength>>0);
            
            for (int i=0;i<msgPayloadLength;i++){
                checksum ^= msgPayload[i];
              
            }
            return checksum;
}


void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload)
{
            int msgLength= msgPayloadLength+6;
            unsigned char msg[msgLength];
            msg[0] = 0xFE;
            msg[1]=(unsigned char)(msgFunction >> 8);
            msg[2] =(unsigned char)(msgFunction >> 0);
            msg[3]= (unsigned char)(msgPayloadLength >> 8);
            msg[4] = (unsigned char)(msgPayloadLength >> 0);
            
            for(int i=0; i< msgPayloadLength; i++)
            {
                msg[5 + i] = msgPayload[i];
            }
            msg[5+msgPayloadLength] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
           // serialPort1.Write(msg, 0, msg.Length);          
            
            SendMessage(msg, msgLength);
           
}



void UartSendStateRobot(int msgFunction, int NumSate, unsigned long timestamp ){
        int msgLength= 5+6+1;//timestamp
        unsigned char msg[msgLength];
        
        //sof
        msg[0] = 0xFE;
        //command
        msg[1] = (unsigned char)(msgFunction>>8);
        msg[2] =(unsigned char)(msgFunction>>0);
        //payload length
        msg[3] =(unsigned char)(2>>8);
        msg[4] =(unsigned char)(2>>0);
        //payload
        msg[5]=(unsigned char)(NumSate);
        msg[6]=(unsigned char)(timestamp>>24);
        msg[7]=(unsigned char)(timestamp>>16);
        msg[8]=(unsigned char)(timestamp>>8);
        msg[9]=(unsigned char)(timestamp>>0);
        //checksum
        unsigned char* msgPayload[5]={msg[5],msg[6],msg[7],msg[8],msg[9]};
        msg[10]=UartCalculateChecksum(msgFunction, 5, msgPayload);
}
/*
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;


void UartDecodeMessage(unsigned char c)
{
//Fonction prenant en entree un octet et servant a reconstituer les trames
...
}


void UartProcessDecodedMessage(int function,int payloadLength, unsigned char* payload)
{
//Fonction appelee apres le decodage pour executer l?action
//correspondant au message recu
...
}
*/

//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/


