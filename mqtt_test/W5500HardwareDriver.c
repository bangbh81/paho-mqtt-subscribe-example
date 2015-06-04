/*Include: STM Library*/
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_exti.h"
#include "misc.h"

/*Include: Header file*/
#include "W5500HardwareDriver.h"

/*Include: W5500 Library*/
#include "wizchip_conf.h"

/*include: Standard IO library*/
#include <stdio.h>

#include "IoTEVB.h"

extern led_ctrl led1,led2;
extern wiz_NetInfo gWIZNETINFO;
void W5500HardwareInitilize(void)
{
	/*Initialize Structure*/
	GPIO_InitTypeDef	GPIO_InitStructure;
	SPI_InitTypeDef		SPI_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	/*Enable clock related Periphs*/
	RCC_APB2PeriphClockCmd(W5500_SPI_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(W5500_GPIO_RCC,ENABLE);

	/*Initialize CLK Pin*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = W5500_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(W5500_CLK_PORT,&GPIO_InitStructure);

	/*Initialize MOSI Pin*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = W5500_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(W5500_MOSI_PORT,&GPIO_InitStructure);

	/*Initialize MISO Pin*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = W5500_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(W5500_MISO_PORT,&GPIO_InitStructure);

	/*Initialize CS Pin*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = W5500_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(W5500_CS_PORT,&GPIO_InitStructure);

	/*Initialize Reset Pin*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = W5500_RESET_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(W5500_RESET_PORT,&GPIO_InitStructure);

	/*Initialize INT Pin*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = W5500_INT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(W5500_INT_PORT,&GPIO_InitStructure);

	/*Initialize SPI*/
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(W5500_SPI, &SPI_InitStructure);

	SPI_Cmd(W5500_SPI, ENABLE);

	W5500HardwareReset();
}

void W5500Initialze(void)
{
	unsigned char temp;
	unsigned char W5500FifoSize[2][8] = {{2,2,2,2,2,2,2,2,},{2,2,2,2,2,2,2,2}};

	W5500DeSelect();

	/* spi function register */
	reg_wizchip_spi_cbfunc(W5500ReadByte, W5500WriteByte);

	/* CS function register */
	reg_wizchip_cs_cbfunc(W5500Select,W5500DeSelect);

	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)W5500FifoSize) == -1)
	{
		printf("W5500 initialized fail.\r\n");
	}

	do{//check phy status.
		if(ctlwizchip(CW_GET_PHYLINK,(void*)&temp) == -1){
			printf("Unknown PHY link status.\r\n");
		}
	}while(temp == PHY_LINK_OFF);

}

void W5500HardwareReset(void)
{
	int i,j,k;
	k=0;
	GPIO_ResetBits(W5500_RESET_PORT,W5500_RESET_PIN);
	for(i = 0 ;i < 100 ; i++)
	{
		for(j = 0 ;j < 100 ; j++)
		{
			k++;
		}
	}
	GPIO_SetBits(W5500_RESET_PORT,W5500_RESET_PIN);
}

void W5500WriteByte(unsigned char byte)
{
	while (SPI_I2S_GetFlagStatus(W5500_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(W5500_SPI, byte);
	while (SPI_I2S_GetFlagStatus(W5500_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_I2S_ReceiveData(W5500_SPI);
}

unsigned char W5500ReadByte(void)
{
	while (SPI_I2S_GetFlagStatus(W5500_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(W5500_SPI, 0xff);
	while (SPI_I2S_GetFlagStatus(W5500_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(W5500_SPI);
}

void W5500Select(void)
{
	GPIO_ResetBits(W5500_CS_PORT,W5500_CS_PIN);
}

void W5500DeSelect(void)
{
	GPIO_SetBits(W5500_CS_PORT,W5500_CS_PIN);
}


