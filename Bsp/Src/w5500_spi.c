#include "stm32g4xx_hal.h"
#include "main.h"//for GPIO names
#include "wizchip_conf.h"
#include "stdio.h"

extern SPI_HandleTypeDef hspi_w5500;

uint8_t SPIReadWrite(uint8_t data)
{
	uint8_t tx_buffer[1];
	uint8_t rx_buffer[1];

	tx_buffer[0]=data;

	HAL_SPI_TransmitReceive(&hspi_w5500, tx_buffer, rx_buffer, 1, 1);

	return rx_buffer[0];
}

void  wizchip_select(void)
{
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
}

void  wizchip_deselect(void)
{
	HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

uint8_t wizchip_read()
{
	uint8_t rb;
	rb=SPIReadWrite(0x00);
	return rb;
}

void  wizchip_write(uint8_t wb)
{
	SPIReadWrite(wb);
}

void wizchip_readburst(uint8_t* pBuf, uint16_t len)
{
	HAL_SPI_Receive(&hspi_w5500, pBuf, len, 100);
}

void  wizchip_writeburst(uint8_t* pBuf, uint16_t len)
{
	HAL_SPI_Transmit(&hspi_w5500, pBuf, len, 100);
}

void w5500_enter_critical(void)
{
	__disable_irq();
}

void w5500_exit_critical(void)
{
	__enable_irq();
}

void W5500Init()
{
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);//CS high by default

	//Send a pulse on reset pin
	HAL_GPIO_WritePin(RST_W5500_GPIO_Port, RST_W5500_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(RST_W5500_GPIO_Port, RST_W5500_Pin, GPIO_PIN_SET);
	HAL_Delay(2);

	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);
	reg_wizchip_cris_cbfunc(w5500_enter_critical, w5500_exit_critical);

	/* WIZChip Initialize*/
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1) {
		printf("WIZCHIP Initialized Failed.\r\n");
		while (1);
	}
	printf("WIZCHIP Initialization Success.\r\n");

	//Check if SPI is working
	printf("Auto-detecting Ethernet Chip on SPI Port...");
	if(WIZCHIP_READ(VERSIONR)==0x04)
	{
		printf("W5500 Found on SPI\r\n");
	}
	else
	{
		printf("W5500 NOT Found on SPI\r\n");
		printf("Double Check SPI Cables\r\n");
		while(1);
	}
}

