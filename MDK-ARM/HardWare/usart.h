#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32g0xx_hal.h"
void Error_Handler(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void Uart1_SendStr(char*SendBuf);
void UART1_send_byte(char data);
extern uint8_t RxBuffer[200],Rxcouter;
extern uint8_t Uart1_RxBuff[100];		//��������
extern uint8_t Uart1_Rx_Cnt ;		//��
extern unsigned char uart1_getok;
extern unsigned char uart2_getok; 
#endif


