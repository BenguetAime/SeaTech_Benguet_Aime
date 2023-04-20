#include <xc.h>
#include "UART_Protocol.h"
#include "UART.h"
#include "CB_TX1.h"
#include "main.h"




unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction prenant entree la trame et sa longueur pour calculer le checksum
    unsigned char checksum = 0;
    checksum ^= 0xFE;
    checksum ^= (unsigned char) (msgFunction >> 8);
    checksum ^= (unsigned char) (msgFunction >> 0);
    checksum ^= (unsigned char) (msgPayloadLength >> 8);
    checksum ^= (unsigned char) (msgPayloadLength >> 0);

    for (int i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];

    }
    return checksum;
}

void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    int msgLength = msgPayloadLength + 6;
    unsigned char msg[msgLength];
    msg[0] = 0xFE;
    msg[1] = (unsigned char) (msgFunction >> 8);
    msg[2] = (unsigned char) (msgFunction >> 0);
    msg[3] = (unsigned char) (msgPayloadLength >> 8);
    msg[4] = (unsigned char) (msgPayloadLength >> 0);

    for (int i = 0; i < msgPayloadLength; i++) {
        msg[5 + i] = msgPayload[i];
    }
    msg[5 + msgPayloadLength] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
    // serialPort1.Write(msg, 0, msg.Length);          

    SendMessage(msg, msgLength);

}

void UartSendStateRobot(int msgFunction, unsigned char numState, unsigned long timestamp) {
    unsigned char payload[5];
    payload[0] = numState;
    payload[1] = (unsigned char) (timestamp >> 24);
    payload[2] = (unsigned char) (timestamp >> 16);
    payload[3] = (unsigned char) (timestamp >> 8);
    payload[4] = (unsigned char) (timestamp >> 0);

    UartEncodeAndSendMessage(0x0050, 5, payload);
}

enum StateReception {
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
unsigned char msgDecodedPayload[256];
int msgDecodedPayloadIndex = 0;
int receivedChecksum;

void UartDecodeMessage(unsigned char c) {
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    switch (rcvState) {
        case Waiting:
            if (c == 0xFE) {
                rcvState = FunctionMSB;

            }
            break;
        case FunctionMSB:
            msgDecodedFunction = c << 8;
            rcvState = FunctionLSB;
            break;

        case FunctionLSB:
            msgDecodedFunction += c << 0;
            rcvState = PayloadLengthMSB;
            break;
        case PayloadLengthMSB:
            msgDecodedPayloadLength = c << 8;
            rcvState = PayloadLengthLSB;
            break;
        case PayloadLengthLSB:
            msgDecodedPayloadLength += c << 0;
            if (msgDecodedPayloadLength == 0)
                rcvState = CheckSum;
            else {
                rcvState = Payload;
                msgDecodedPayloadIndex = 0;
                //unsigned char msgDecodedPayload[msgDecodedPayloadLength];
            }
            break;
        case Payload:

            msgDecodedPayload[msgDecodedPayloadIndex] = c;
            msgDecodedPayloadIndex++;
            if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                rcvState = CheckSum;

            break;


        case CheckSum:
        {
            int calculatedChecksumvaleur = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            int receivedChecksum = c;
            if (calculatedChecksumvaleur == receivedChecksum) {
                UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message au MicroC Valide");
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

            } else
                //textBoxReception.Text += "Message corrompu";                     
                UartEncodeAndSendMessage(0x0080, 26, (unsigned char*) "Message au MicroC corrompu");
            rcvState = Waiting;
        }
            break;
        default:
            rcvState = Waiting;
            break;

    }

}





void UartProcessDecodedMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload)
{
    //Fonction appelee apres le decodage pour executer l?action
    //correspondant au message recu
    switch (msgFunction)
            {
                case 0x0080:
                    UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message envoyé au MicroC Valide");
                    break;
                case 0x0020:
                    if (msgPayload[0] == 1)
                        UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message envoyé au MicroC LED 1");
                    
                    else if (msgPayload[0] == 2)
                    UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message envoyé au MicroC LED 2");
                    else if (msgPayload[0] == 3)
                        UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message envoyé au MicroC LED 3");
                    break;

                case 0x0030:
                    UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message envoyé au MicroC IRG");
                    UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message envoyé au MicroC IRC");
                    UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message envoyé au MicroC IRD");
                    break;
                case 0x0040:
                    UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message envoyé au MicroC VitG");
                    UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message envoyé au MicroC VitD");
                    break;
                
                
                case 0x0050:
                    UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "Message envoyé au MicroC State");
                    break;          
            }
    
            
            
            switch (msgFunction)
            {
            case SET_ROBOT_STATE:
                SetRobotState(msgPayload[0]);
                UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "State mis a jour");
                break;
            case SET_ROBOT_MANUAL_CONTROL:
                SetRobotAutoControlState(msgPayload[0]);
                UartEncodeAndSendMessage(0x0080, 24, (unsigned char*) "State mode mis a jour");
                break;
            default:
                break;
            }

            
}
 

//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/



