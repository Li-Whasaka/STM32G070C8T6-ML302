#include "ML302.h"
#include "string.h"
#include "usart.h"
#include "cJSON.h"
char *strx,*extstrx;
ML302 ML302_Status;

/********************************************************************
*********************************************************************
  ************* �豸���������й�ҵ������ƽ̨ ��Ϣ�������� *************
        1��HOST  	 	�����й�ҵ������MQTT��������ַ
        2��PORT		  	MQTT�������Ķ˿ں� һ�㶼Ϊ1883
        3��ClientId     �ͻ���ID Ҫ����Ϊƽ̨�ϴ������豸ID 
        4��username		MQTT���������û���
        5��passwd 		MQTT������������
        6��Pubtopic   	��Ϣ�������⣺�豸->������ƽ̨�� 
				��ʽͳһΪ"/{productId}/{deviceId}/properties/report"  
		7��Subtopic		��Ϣ�������⣺������ƽ̨->�豸�� 
    ע��Ŀǰƽ̨MQTT����������Ҫusername �� passwd  ����4��5����Բ��趨
*********************************************************************
*********************************************************************
*/
#define HOST       "122.189.56.75"
#define PORT       1883
#define ClientId   "ml302"
#define username   ""
#define passwd     ""
#define Pubtopic  "/STM32/ml302/properties/report"  //��������
#define Subtopic  "/STM32/ml302/properties/read"  //��������


void checkCommand(void){
	//char *str_led = NULL;
	/*cJSON * strJson = cJSON_Parse((char*)RxBuffer);   //����JSON�������󣬷���JSON��ʽ�Ƿ���ȷ
	cJSON *ledJson = cJSON_GetObjectItem(strJson,"ML302_LED"); //��ȡ��������Ա
	if(!ledJson){
		Uart1_SendStr("Can not find ML302_LED");
		
	}else{
		if(ledJson->type == cJSON_String && strcmp(strJson->valuestring ,"ON") == 0){
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
		}		
	}*/
	char json_len;
	char* str1 = strstr((char*)RxBuffer,"\"ML302_LED\":\"ON\"");
	char* str2 = strstr((char*)RxBuffer,"\"ML302_LED\":\"OFF\"");
	if(str1){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
		json_len = strlen("{\"properties\":{\"ML302_LED\":\"ON\"}}")/sizeof(char);
		printf("AT+MQTTPUB=\"%s\",1,0,0,%d\r\n",Pubtopic,json_len);
		HAL_Delay(300);
		printf("{\"properties\":{\"ML302_LED\":\"ON\"}}");//�������ݵ�ƽ̨
		HAL_Delay(300);
		IWDG_Feed();
	}else if(str2){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		json_len = strlen("{\"properties\":{\"ML302_LED\":\"OFF\"}}")/sizeof(char);
		printf("AT+MQTTPUB=\"%s\",1,0,0,%d\r\n",Pubtopic,json_len);
		HAL_Delay(300);
		printf("{\"properties\":{\"ML302_LED\":\"OFF\"}}");//�������ݵ�ƽ̨
		HAL_Delay(300);
		IWDG_Feed();
	}
}

void Clear_Buffer(void)//��ջ���
{
	uint8_t i;
	strcpy((char*)(ML302_Status.recdata),(char*)RxBuffer);
	Uart1_SendStr((char*)RxBuffer);
	checkCommand();
	for(i=0;i<100;i++)
		RxBuffer[i]=0;//����
	Rxcouter=0;
	IWDG_Feed();//ι��
	
}
void ML302_Init(void)
{
	Uart1_SendStr("#######MQTT_Test########\r\n");
	HAL_Delay(300);
    printf("AT\r\n"); 
    HAL_Delay(300);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    Clear_Buffer();	
    while(strx==NULL)
    {
        Clear_Buffer();	
        printf("AT\r\n"); 
        HAL_Delay(300);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    }
    printf("ATE0&W\r\n"); //�رջ���
    HAL_Delay(300);
    printf("AT+MQTTDISC\r\n");//�ر���һ������
    HAL_Delay(300);
    printf("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
    HAL_Delay(300);
    strx=strstr((const char*)RxBuffer,(const char*)"460");//��460������ʶ�𵽿���
    Clear_Buffer();	
    while(strx==NULL){
        Clear_Buffer();	
        printf("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
        HAL_Delay(300);
        strx=strstr((const char*)RxBuffer,(const char*)"460");//����OK,˵�����Ǵ��ڵ�
    }
	
	printf("AT+CGSN=1\r\n");//��ȡIMEI��
    HAL_Delay(300);
    strx=strstr((const char*)RxBuffer,(const char*)"+CGSN:");//��460������ʶ�𵽿���
	memset(ML302_Status.IMEI,'\0',sizeof(ML302_Status.IMEI));
	strncpy(ML302_Status.IMEI,strx+7,15);
    Clear_Buffer();	
    while(strx==NULL){	
		memset(ML302_Status.IMEI,'\0',sizeof(ML302_Status.IMEI));
        Clear_Buffer();	
		printf("AT+CGSN=1\r\n");//��ȡIMEI��
        HAL_Delay(300);
        strx=strstr((const char*)RxBuffer,(const char*)"+CGSN:");//����OK,˵�����Ǵ��ڵ�
		strncpy(ML302_Status.IMEI,strx+7,15);
    }
    printf("AT+CGDCONT=2,\"IP\",\"CMNET\"\r\n");//����APN����
    HAL_Delay(300);
    Clear_Buffer();	
	printf("AT+CGACT=1,2\r\n");//�������磬PDP
	HAL_Delay(300);
    Clear_Buffer();	
	printf("AT+CSTT=\"CMNET\",\"\",\"\" \r\n");//��������
	HAL_Delay(300);
    Clear_Buffer();	
	printf("AT+CIICR\r\n");//�����������
	HAL_Delay(300);
    Clear_Buffer();	
    printf("AT+CGATT?\r\n");//��ѯ����״̬
    HAL_Delay(300);
    strx=strstr((const char*)RxBuffer,(const char*)"+CGATT:1");//��1
    Clear_Buffer();	
	while(strx==NULL)
	{
		Clear_Buffer();	
		printf("AT+CGATT?\r\n");//��ȡ����״̬
		HAL_Delay(300);
		strx=strstr((const char*)RxBuffer,(const char*)"+CGATT:1");//����1,����ע���ɹ�
	}
	printf("AT+CSQ\r\n");//��ѯ�ź�
	HAL_Delay(300);
	Clear_Buffer();	
  
}

//��ʼ��MQTT
void ML302_MQTTInit(void)
{
	
	printf("AT+MQTTCFG=\"%s\",%d,\"%s\",30,\"\",\"\",0,\"\",0\r\n",HOST,PORT,ClientId);//�����豸ID��Tokenֵ
    HAL_Delay(300);
    Clear_Buffer();	
    printf("AT+MQTTOPEN=0,0,0,0,0,\"\",\"\"\r\n");//���ӵ�ONENETƽ̨
    HAL_Delay(300);
    strx=strstr((const char*)RxBuffer,(const char*)"+MQTTOPEN:OK");//����¼�ɹ�
    while(strx==NULL)
    { 
       strx=strstr((const char*)RxBuffer,(const char*)"+MQTTOPEN:OK");//��¼��ONENET�ɹ� ��ʱ�豸����ʾ����  
    }
    Clear_Buffer();	
}
//�������ݵ�������
void ML302_MQTTPUBMessage(char temp,char humi){
	 char json[] = "{\"properties\":{\"ML302_Temperature\":%0.1d,\"ML302_Humidity\":%0.1d,\"IMEI\":\"%s\"}}";
	 char t_json[200];
	 char json_len;
	 sprintf(t_json, json,temp,humi,ML302_Status.IMEI);
	 json_len = strlen(t_json)/sizeof(char);
    //printf("AT+MQTTPUB=\"$sys/%s/%s/dp/post/json\",1,0,0,%d\r\n",ProductID,DeviceID,json_len);
	 printf("AT+MQTTPUB=\"%s\",1,0,0,%d\r\n",Pubtopic,json_len);
	 HAL_Delay(300);
	 printf("%s",t_json);//�������ݵ�ƽ̨
     HAL_Delay(300);
	 strx=strstr((const char*)RxBuffer,(const char*)"+MQTTPUBACK:0");//���ط����ɹ�
	 while(strx==NULL)
	 { 
		strx=strstr((const char*)RxBuffer,(const char*)"+MQTTPUBACK:0");//�������ݳɹ�
	 }
	 Clear_Buffer();	
}



