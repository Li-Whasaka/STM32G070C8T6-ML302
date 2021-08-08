#ifndef __BSP_I2C_H
#define	__BSP_I2C_H

#include "stm32g0xx_hal.h"

/*********************���IICʹ�õĺ�****************************/
#define Soft_I2C_SDA 		GPIO_PIN_7
#define Soft_I2C_SCL 		GPIO_PIN_6
#define Soft_I2C_PORT       GPIOC
//  
#define Soft_I2C_SCL_0 		HAL_GPIO_WritePin(Soft_I2C_PORT, Soft_I2C_SCL,GPIO_PIN_RESET)
#define Soft_I2C_SCL_1 		HAL_GPIO_WritePin(Soft_I2C_PORT, Soft_I2C_SCL,GPIO_PIN_SET)
#define Soft_I2C_SDA_0 		HAL_GPIO_WritePin(Soft_I2C_PORT, Soft_I2C_SDA,GPIO_PIN_RESET)
#define Soft_I2C_SDA_1    HAL_GPIO_WritePin(Soft_I2C_PORT, Soft_I2C_SDA,GPIO_PIN_SET)

/*�ȴ���ʱʱ��*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
#define  I2C_Direction_Transmitter      ((uint8_t)0x00)
#define  I2C_Direction_Receiver         ((uint8_t)0x01)
void I2C_Bus_Init(void);

void Set_I2C_Retry(unsigned short ml_sec);
unsigned short Get_I2C_Retry(void);
int Sensors_I2C_ReadRegister(unsigned char Address, unsigned char RegisterAddr, 
                                          unsigned short RegisterLen, unsigned char *RegisterValue);
int Sensors_I2C_WriteRegister(unsigned char Address, unsigned char RegisterAddr, 
                                           unsigned short RegisterLen, const unsigned char *RegisterValue);

#endif
