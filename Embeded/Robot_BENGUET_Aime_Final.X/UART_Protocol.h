#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H
#define SET_ROBOT_STATE 0x0051
#define SET_ROBOT_MANUAL_CONTROL 0x0052


unsigned char UartCalculateChecksum(int ,int , unsigned char* );
void UartEncodeAndSendMessage(int,int, unsigned char*);
void UartSendStateRobot(int msgFunction, unsigned char numState, unsigned long timestamp);
void UartDecodeMessage(unsigned char );

void UartProcessDecodedMessage(int,int, unsigned char*);


#endif	/* UART_PROTOCOL_H */

