#ifndef __M5311_H
#define __M5311_H	
#include "usart.h"
#include "stm32g0xx_hal.h"
void Clear_Buffer(void);//��ջ���	
void ML302_Init(void);
void ML302_CreateTCPSokcet(void);
void ML302_TCPSend(uint8_t *data,uint8_t len);
void M5L302_RECData(void);
void ML302_MQTTInit(void);
//void ML302_MQTTPUBMessage(char temp,char *properties);
void ML302_MQTTPUBMessage(char temp,char humi);
void checkCommand(void);
void IWDG_Feed(void);
typedef struct
{
    uint8_t CSQ;    
    uint8_t Socketnum;   //���
    uint8_t reclen;   //��ȡ�����ݵĳ���
    uint8_t res;
	char IMEI[15];	
    uint8_t recdatalen[10];
    uint8_t recdata[200];
    uint8_t netstatus;//����ָʾ��
} ML302;

#endif





