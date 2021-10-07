/*
 * W25Q128fv.h
 *
 *  Created on: Sep 2, 2021
 *      Author: ASUS
 */

#ifndef W25Q128FV_H_
#define W25Q128FV_H_

#include "stm32f407xx.h"

/*Application configurable items */
#define W25Q128_SPI  			SPI2
#define W25Q128_SPI_GPIO_PORT   GPIOB
#define W25Q128_SPI_ALTFUN_MODE  5
#define W25Q128_SPI_MOSI_PIN 	GPIO_PIN_NO_15
#define W25Q128_SPI_MISO_PIN 	GPIO_PIN_NO_14
#define W25Q128_SPI_NSS_PIN 	GPIO_PIN_NO_12
#define	W25Q128_SPI_SCLK_PIN	GPIO_PIN_NO_13
#define W25Q128_SPI_BUS_CONFIG 	SPI_BUS_CONFIG_FD
#define W25Q128_SPI_DEVICE_MODE SPI_DEVICE_MODE_MASTER
#define W25Q128_SPI_SCLK_SPEED	SPI_SCLK_SPEED_DIV8
#define	W25Q128_SPI_CPOL		SPI_CPOL_LOW
#define	W25Q128_SPI_CPHA		SPI_CPHA_LOW
#define	W25Q128_SPI_SSM			SPI_SSM_EN
#define	W25Q128_SPI_CS_PORT		GPIOD
#define	W25Q128_SPI_CS_PIN		GPIO_PIN_NO_0

/*Memory addresses */
/*

#define W25Q128_BASEADDR_BLOCK0 	0x000000U
#define W25Q128_BASEADDR_BLOCK1 	(W25Q128_BASEADDR_BLOCK0 + 0x010000)
#define W25Q128_BASEADDR_BLOCK2 	(W25Q128_BASEADDR_BLOCK0 + 0x020000)

#ifdef W25Q128_BASEADDR_BLOCK0
#define W25Q128_BASEADDR_SECTOR 	W25Q128_BASEADDR_BLOCK0
#endif
#ifdef W25Q128_BASEADDR_BLOCK1
#define W25Q128_BASEADDR_SECTOR 	W25Q128_BASEADDR_BLOCK1
#endif
#ifdef W25Q128_BASEADDR_BLOCK2
#define W25Q128_BASEADDR_SECTOR 	W25Q128_BASEADDR_BLOCK2
#endif


#define	W25Q128_BASEADDR_SECTOR		W25Q128_BASEADDR_BLOCK
#define W25Q128_BASEADDR_SECTOR1 	(W25Q128_BASEADDR_SECTOR + 0x001000)
#define W25Q128_BASEADDR_SECTOR2 	(W25Q128_BASEADDR_SECTOR + 0x002000)
#define W25Q128_BASEADDR_SECTOR3 	(W25Q128_BASEADDR_SECTOR + 0x003000)

*/
//INSTRUCTIONS
#define READ_ID   0x9F  // 1st byte manufacture id ,2nd memory type ,3rd capacity in kB
#define READ_SR1  0x05	//read status register 1
#define READ_SR2  0x35	//read status register 2
#define READ_SR3  0x15	//read status register 3
#define WRITE_EN  0x06	//enable writing into chip
#define WRITE_DI  0x04	//disable writing into chip
#define PAGE_WRT  0x02	//write data into chip
#define ERASE_SECTOR 0x20 // erase sector of memory
#define ERASE_CHIP  0x60	//erase whole chip
#define READ_BYTES  0x03	//read bytes of data
//flags
#define W25Q128fv_FLAG_BUSY  0
#define W25Q128fv_FLAG_WEL   1
//device id
#define W25Q128fv_id  0x17

//Function prototypes
uint8_t W25Q128_init(void);
void W25Q128_writesector();
void W25Q128_readsector();
void W25Q128_erasesector();
static void W25Q128_spi_pin_config(void);
static void W25Q128_spi_config(void);
uint8_t W25Q128_ReadStatusRegister(uint8_t RegSelect);
uint8_t W25Q128fv_Spi(uint8_t dataOrIns);
uint8_t Get_flag_status(uint8_t Reg ,uint8_t flagName);
void W25Q128_EraseChip(void);

typedef struct
{
		uint8_t  UniqID[8];
		uint16_t PageSize;
		uint32_t PageCount;
		uint32_t SectorSize;
		uint32_t SectorCount;
		uint32_t BlockSize;
		uint32_t BlockCount;
		uint32_t CapacityInKiloByte;
		uint8_t StatusRegister1;
		uint8_t StatusRegister2;
		uint8_t StatusRegister3;
		uint8_t  Lock;

}W25Q128Handle_t;








#endif /* W25Q128FV_H_ */
