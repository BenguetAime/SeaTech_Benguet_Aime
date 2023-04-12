#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

unsigned char UartCalculateChecksum(int ,int , unsigned char* );
void UartEncodeAndSendMessage(int,int, unsigned char*);
void UartSendStateRobot(int,int,unsigned long);
//void UartDecodeMessage(unsigned char );
//void UartProcessDecodedMessage(int,int, unsigned char*)

#endif	/* UART_PROTOCOL_H */

