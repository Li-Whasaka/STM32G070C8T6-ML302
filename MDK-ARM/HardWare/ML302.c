#include "ML302.h"
#include "string.h"
#include "usart.h"
#include "cJSON.h"
char *strx,*extstrx;
ML302 ML302_Status;

/********************************************************************
*********************************************************************
  ************* 设备连接咸宁市工业物联网平台 信息参数配置 *************
        1、HOST  	 	咸宁市工业物联网MQTT服务器地址
        2、PORT		  	MQTT服务器的端口号 一般都为1883
        3、ClientId     客户端ID 要设置为平台上创建的设备ID 
        4、username		MQTT服务器的用户名
        5、passwd 		MQTT服务器的密码
        6、Pubtopic   	消息发布主题：设备->物联网平台。 
				格式统一为"/{productId}/{deviceId}/properties/report"  
		7、Subtopic		消息订阅主题：物联网平台->设备。 
    注：目前平台MQTT服务器不需要username 和 passwd  所以4、5项可以不设定
*********************************************************************
*********************************************************************
*/
#define HOST       ""
#define PORT       
#define ClientId   ""
#define username   ""
#define passwd     ""
#define Pubtopic  "/STM32/ml302/properties/report"  //发布主题
#define Subtopic  "/STM32/ml302/properties/read"  //订阅主题


void checkCommand(void){
	//char *str_led = NULL;
	/*cJSON * strJson = cJSON_Parse((char*)RxBuffer);   //创建JSON解析对象，返回JSON格式是否正确
	cJSON *ledJson = cJSON_GetObjectItem(strJson,"ML302_LED"); //获取这个对象成员
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
		printf("{\"properties\":{\"ML302_LED\":\"ON\"}}");//发布数据到平台
		HAL_Delay(300);
		IWDG_Feed();
	}else if(str2){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		json_len = strlen("{\"properties\":{\"ML302_LED\":\"OFF\"}}")/sizeof(char);
		printf("AT+MQTTPUB=\"%s\",1,0,0,%d\r\n",Pubtopic,json_len);
		HAL_Delay(300);
		printf("{\"properties\":{\"ML302_LED\":\"OFF\"}}");//发布数据到平台
		HAL_Delay(300);
		IWDG_Feed();
	}
}

void Clear_Buffer(void)//清空缓存
{
	uint8_t i;
	strcpy((char*)(ML302_Status.recdata),(char*)RxBuffer);
	Uart1_SendStr((char*)RxBuffer);
	checkCommand();
	for(i=0;i<100;i++)
		RxBuffer[i]=0;//缓存
	Rxcouter=0;
	IWDG_Feed();//喂狗
	
}
void ML302_Init(void)
{
	Uart1_SendStr("#######MQTT_Test########\r\n");
	HAL_Delay(300);
    printf("AT\r\n"); 
    HAL_Delay(300);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    Clear_Buffer();	
    while(strx==NULL)
    {
        Clear_Buffer();	
        printf("AT\r\n"); 
        HAL_Delay(300);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    }
    printf("ATE0&W\r\n"); //关闭回显
    HAL_Delay(300);
    printf("AT+MQTTDISC\r\n");//关闭上一次连接
    HAL_Delay(300);
    printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
    HAL_Delay(300);
    strx=strstr((const char*)RxBuffer,(const char*)"460");//返460，表明识别到卡了
    Clear_Buffer();	
    while(strx==NULL){
        Clear_Buffer();	
        printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
        HAL_Delay(300);
        strx=strstr((const char*)RxBuffer,(const char*)"460");//返回OK,说明卡是存在的
    }
	
	printf("AT+CGSN=1\r\n");//获取IMEI。
    HAL_Delay(300);
    strx=strstr((const char*)RxBuffer,(const char*)"+CGSN:");//返460，表明识别到卡了
	memset(ML302_Status.IMEI,'\0',sizeof(ML302_Status.IMEI));
	strncpy(ML302_Status.IMEI,strx+7,15);
    Clear_Buffer();	
    while(strx==NULL){	
		memset(ML302_Status.IMEI,'\0',sizeof(ML302_Status.IMEI));
        Clear_Buffer();	
		printf("AT+CGSN=1\r\n");//获取IMEI。
        HAL_Delay(300);
        strx=strstr((const char*)RxBuffer,(const char*)"+CGSN:");//返回OK,说明卡是存在的
		strncpy(ML302_Status.IMEI,strx+7,15);
    }
    printf("AT+CGDCONT=2,\"IP\",\"CMNET\"\r\n");//设置APN参数
    HAL_Delay(300);
    Clear_Buffer();	
	printf("AT+CGACT=1,2\r\n");//激活网络，PDP
	HAL_Delay(300);
    Clear_Buffer();	
	printf("AT+CSTT=\"CMNET\",\"\",\"\" \r\n");//开启网络
	HAL_Delay(300);
    Clear_Buffer();	
	printf("AT+CIICR\r\n");//启动网络接入
	HAL_Delay(300);
    Clear_Buffer();	
    printf("AT+CGATT?\r\n");//查询激活状态
    HAL_Delay(300);
    strx=strstr((const char*)RxBuffer,(const char*)"+CGATT:1");//返1
    Clear_Buffer();	
	while(strx==NULL)
	{
		Clear_Buffer();	
		printf("AT+CGATT?\r\n");//获取激活状态
		HAL_Delay(300);
		strx=strstr((const char*)RxBuffer,(const char*)"+CGATT:1");//返回1,表明注网成功
	}
	printf("AT+CSQ\r\n");//查询信号
	HAL_Delay(300);
	Clear_Buffer();	
  
}

//初始化MQTT
void ML302_MQTTInit(void)
{
	
	printf("AT+MQTTCFG=\"%s\",%d,\"%s\",30,\"\",\"\",0,\"\",0\r\n",HOST,PORT,ClientId);//设置设备ID和Token值
    HAL_Delay(300);
    Clear_Buffer();	
    printf("AT+MQTTOPEN=0,0,0,0,0,\"\",\"\"\r\n");//连接到ONENET平台
    HAL_Delay(300);
    strx=strstr((const char*)RxBuffer,(const char*)"+MQTTOPEN:OK");//返登录成功
    while(strx==NULL)
    { 
       strx=strstr((const char*)RxBuffer,(const char*)"+MQTTOPEN:OK");//登录到ONENET成功 此时设备会显示在线  
    }
    Clear_Buffer();	
}
//发送数据到服务器
void ML302_MQTTPUBMessage(char temp,char humi){
	 char json[] = "{\"properties\":{\"ML302_Temperature\":%0.1d,\"ML302_Humidity\":%0.1d,\"IMEI\":\"%s\"}}";
	 char t_json[200];
	 char json_len;
	 sprintf(t_json, json,temp,humi,ML302_Status.IMEI);
	 json_len = strlen(t_json)/sizeof(char);
    //printf("AT+MQTTPUB=\"$sys/%s/%s/dp/post/json\",1,0,0,%d\r\n",ProductID,DeviceID,json_len);
	 printf("AT+MQTTPUB=\"%s\",1,0,0,%d\r\n",Pubtopic,json_len);
	 HAL_Delay(300);
	 printf("%s",t_json);//发布数据到平台
     HAL_Delay(300);
	 strx=strstr((const char*)RxBuffer,(const char*)"+MQTTPUBACK:0");//返回发布成功
	 while(strx==NULL)
	 { 
		strx=strstr((const char*)RxBuffer,(const char*)"+MQTTPUBACK:0");//发布数据成功
	 }
	 Clear_Buffer();	
}



