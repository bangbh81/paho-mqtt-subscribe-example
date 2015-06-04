/*Include: STM Library*/
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

/*Include: Header file*/
#include "IoTEVB.h"
#include "wizchip_conf.h"

extern wiz_NetInfo gWIZNETINFO;

void I2C1Initialize(void)
{
	/*i2c clock enable*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);

	/*i2c gpio initialize*/
	GPIO_InitTypeDef	GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =  MACEEP_SCL_Pin | MACEEP_SDA_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(MACEEP_PORT, &GPIO_InitStructure);

	/*i2c initialize*/
	I2C_InitTypeDef  I2C_InitStructure;

	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = MACEEP_Address;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_Init(MACEPP_I2C, &I2C_InitStructure);

	I2C_Cmd(MACEPP_I2C, ENABLE);
}

void MACEEP_Read(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
    /* While the bus is busy */
    while(I2C_GetFlagStatus(MACEPP_I2C, I2C_FLAG_BUSY));

    /* Send START condition */
    I2C_GenerateSTART(MACEPP_I2C, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(MACEPP_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(MACEPP_I2C, MACEEP_Address, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(MACEPP_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Send the EEPROM's internal address to read from: LSB of the address */
    I2C_SendData(MACEPP_I2C, (uint8_t)(ReadAddr & 0x00FF));

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(MACEPP_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STRAT condition a second time */
    I2C_GenerateSTART(MACEPP_I2C, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(MACEPP_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for read */
    I2C_Send7bitAddress(MACEPP_I2C, MACEEP_Address, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(MACEPP_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* While there is data to be read */
    while(NumByteToRead)
    {
        if(NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(MACEPP_I2C, DISABLE);

            /* Send STOP Condition */
            I2C_GenerateSTOP(MACEPP_I2C, ENABLE);
        }

    /* Test on EV7 and clear it */
        if(I2C_CheckEvent(MACEPP_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            /* Read a byte from the EEPROM */
            *pBuffer = I2C_ReceiveData(MACEPP_I2C);

            /* Point to the next location where the byte read will be saved */
            pBuffer++;

            /* Decrement the read bytes counter */
            NumByteToRead--;
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(MACEPP_I2C, ENABLE);
}


void USART1Initialze(void)
{
	/* GPIO for USART clock enable */

	RCC_APB2PeriphClockCmd(USART1_GPIO_RCC,ENABLE);

	/* USART1 clock enable */
	RCC_APB2PeriphClockCmd(USART1_RCC,ENABLE);

	/*GPIO initialize configuration structure*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*GPIO for USART RX configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_RX_PORT,&GPIO_InitStructure);

	/*GPIO for USART TX configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_TX_PORT,&GPIO_InitStructure);

	/*USART initialize configuration structure*/
	USART_InitTypeDef USART_InitStructure;

	/* USART1 configuration */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;

	/* USART1 intialize */
	USART_Init(USART1, &USART_InitStructure);

	/* USART1 Enable */
	USART_Cmd(USART1, ENABLE);
}

void led_initialize(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

	GPIO_InitTypeDef	GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED1_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED2_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED2_PORT, &GPIO_InitStructure);
}

inline void led1Ctrl(led_ctrl on_off)
{
	if(on_off == ON)
	{
		GPIO_SetBits(LED1_PORT,LED1_Pin);
	}
	else
	{
		GPIO_ResetBits(LED1_PORT,LED1_Pin);
	}
}

inline void led2Ctrl(led_ctrl on_off)
{
	if(on_off == ON)
	{
		GPIO_SetBits(LED2_PORT,LED2_Pin);
	}
	else
	{
		GPIO_ResetBits(LED2_PORT,LED2_Pin);
	}
}

void print_network_information(void)
{
	wizchip_getnetinfo(&gWIZNETINFO);
	printf("Mac address: %02x:%02x:%02x:%02x:%02x:%02x\n\r",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("IP address : %d.%d.%d.%d\n\r",gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("SM Mask	   : %d.%d.%d.%d\n\r",gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("Gate way   : %d.%d.%d.%d\n\r",gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("DNS Server : %d.%d.%d.%d\n\r",gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
}
