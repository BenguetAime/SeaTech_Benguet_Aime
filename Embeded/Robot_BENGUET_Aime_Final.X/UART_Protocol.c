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
        int msgLength= 5+6;
        unsigned char msg[msgLength];
        //sof
        msg[0] = 0xFE;
        //command
        msg[1] = (unsigned char)(msgFunction>>8);
        msg[2] =(unsigned char)(msgFunction>>0);
        //payload length
        msg[3] =(unsigned char)(5>>8);
        msg[4] =(unsigned char)(5>>0);
        //payload
        msg[5]=(unsigned char)(NumSate>>0);
        msg[6]=(unsigned char)(timestamp>>24);
        msg[7]=(unsigned char)(timestamp>>16);
        msg[8]=(unsigned char)(timestamp>>8);
        msg[9]=(unsigned char)(timestamp>>0);
        //checksum
        unsigned char msgPayload[5]={msg[5],msg[6],msg[7],msg[8],msg[9]};
        msg[10]=UartCalculateChecksum(msgFunction, 5, msgPayload);//OK
        SendMessage(msg, msgLength);
}




enum StateReception
{
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
};



enum StateReception rcvState = Waiting;
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[1];
int msgDecodedPayloadIndex = 0;
int receivedChecksum;


void UartDecodeMessage(unsigned char c)
{
   //Fonction prenant en entree un octet et servant a reconstituer les trames
   switch (rcvState)
            {
                case Waiting:
                    if (c == 0xFE)
                    {
                        rcvState = FunctionMSB;
                        
                    }
                    break;
                case FunctionMSB:
                    msgDecodedFunction = c << 8;
                    rcvState = FunctionLSB;
                    break;

                case FunctionLSB:
                    msgDecodedFunction += c << 0;
                    rcvState =PayloadLengthMSB;
                    break;
                case PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = PayloadLengthLSB;
                    break;
                case PayloadLengthLSB:
                    msgDecodedPayloadLength += c << 0;
                    if (msgDecodedPayloadLength == 0)
                        rcvState = CheckSum;
                    else
                    {
                        rcvState = Payload;
                        msgDecodedPayloadIndex = 0; 
                        unsigned char msgDecodedPayload[msgDecodedPayloadLength];
                    }
                    break;
                case Payload:
                    
                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                        rcvState =CheckSum;
                        
                    break;

                /*
                case CheckSum:                                     
                    int calculatedChecksumvaleur = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload); 
                    int receivedChecksum = c;
                    if (calculatedChecksumvaleur == receivedChecksum)
                    {
                       UartEncodeAndSendMessage(0x0080,24,(unsigned char*)"Message au MicroC Valide");
                       //UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                      
                    }
                    else                       
                        //textBoxReception.Text += "Message corrompu";                     
                        UartEncodeAndSendMessage(0x0080,26,(unsigned char*)"Message au MicroC corrompu");
                        rcvState = Waiting;
                    break;
                default:
                    rcvState = Waiting;
                    break;
                */
            }  
    
}

/*
void UartProcessDecodedMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload)
{
    //Fonction appelee apres le decodage pour executer l?action
    //correspondant au message recu
    switch (msgFunction)
            {
                case 0x0080:
                    textBoxReception.Text += "\n \nMessage text rexçu :\n" + Encoding.UTF8.GetString(msgPayload, 0, msgPayloadLength) +
                        "\nFin message text \n \n";
                    break;
                case 0x0020:
                    if (msgPayload[0] == 1)
                        checkboxLed1.IsChecked = true;
                    
                    else if (msgPayload[0] == 2)
                        checkboxLed1.IsChecked = true;
                    else if (msgPayload[0] == 3)
                        checkboxLed1.IsChecked = true;
                    break;

                case 0x0030:
                    IR_Gauche.Content = msgPayload[0];
                    IR_Centre.Content = msgPayload[1];
                    IR_Droit.Content = msgPayload[2];
                    break;
                case 0x0040:
                    VitesseGauche.Content = msgPayload[0];
                    VitesseDroit.Content = msgPayload[1];
                    break;
                
                
                case 0x0050:
                    int instant = (((int)msgPayload[1]) << 24) + (((int)msgPayload[2]) << 16)
                    + (((int)msgPayload[3]) << 8) + ((int)msgPayload[4]);
                    
                    robot.receivedText += "\nRobot?State?:?" +
                    ((StateRobot)(msgPayload[0])).ToString() +
                    "?-?" + instant.ToString() + "?ms";

                    textBoxReception.Text += robot.receivedText;
                    break;

               
            }
}
*/

//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/



