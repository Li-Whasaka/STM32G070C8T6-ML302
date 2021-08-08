#include "usart.h"


UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
uint8_t aRxBuffer;			//�������ݴ洢����
uint8_t Uart1_RxBuff[100];		//��������
uint8_t Uart1_Rx_Cnt = 0;		//����
uint8_t RxBuffer[200],Rxcouter;

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
	HAL_UART_Transmit( &huart2 , (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
 void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&Uart1_RxBuff, 1);//���������ж�
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); //����IDLE�ж�
	
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
 void MX_USART2_UART_Init(void)
{
  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
	
  /* USER CODE BEGIN USART2_Init 2 */
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);//���������ж�
  /* USER CODE END USART2_Init 2 */
 __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); //����IDLE�ж�

}


void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

void UART1_send_byte(char data)
{
   while((USART1->ISR&0X40)==0){}   //�ȴ�������� 
   USART1->TDR = (uint8_t) data; 
}

void Uart1_SendStr(char*SendBuf)   //����1��ӡ����
{
	while(*SendBuf)
	{
        while((USART1->ISR&0X40)==0);  //�ȴ�������� 
        USART1->TDR = (uint8_t) *SendBuf; 
				SendBuf++;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//�жϽ��ճ�����д
{
	unsigned char  tmp_flag;
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
	*/
  if(huart->Instance==USART1)
	{
		Uart1_RxBuff[Uart1_Rx_Cnt++]= aRxBuffer;   //�ж��յ������ˣ��洢
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);   //�������������ж�1�ֽڽ����ж�
	}
	if(huart->Instance==USART2)
	{
		RxBuffer[Rxcouter++]= aRxBuffer;   //�ж��յ������ˣ��洢
		HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);   //�������������ж�1�ֽڽ����ж�
	}
}


