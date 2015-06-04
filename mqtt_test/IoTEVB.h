#ifndef __IOTEVB_H_
#define __IOTEVB_H_

typedef enum{
	ON = 0,
	OFF = 1
}led_ctrl;

#define USART1_GPIO_RCC RCC_APB2Periph_GPIOA
#define USART1_RCC		RCC_APB2Periph_USART1
#define USART1_RX_PORT	GPIOA
#define USART1_RX_PIN	GPIO_Pin_10
#define USART1_TX_PORT	GPIOA
#define USART1_TX_PIN	GPIO_Pin_9



#define LED_RCC		RCC_APB2Periph_GPIOE
#define LED1_PORT	GPIOE
#define LED1_Pin	GPIO_Pin_0
#define LED2_PORT	GPIOE
#define LED2_Pin	GPIO_Pin_1


#define MACEPP_I2C				I2C1
#define MACEEP_Address			0xA0
#define MACEEP_MacAddresss		0xFA
#define MACEEP_I2C_RCC			RCC_APB1Periph_I2C1
#define MACEEP_GPIO_RCC			RCC_APB2Periph_GPIOB
#define MACEEP_PORT				GPIOB
#define MACEEP_SCL_Pin			GPIO_Pin_6
#define MACEEP_SDA_Pin			GPIO_Pin_7


void I2C1Initialize(void);
void USART1Initialze(void);
void led_initialize(void);
void led1Ctrl(led_ctrl on_off);
void led2Ctrl(led_ctrl on_off);
void print_network_information(void);
#endif
