/*
 * W25Q128fv.c
 *
 *  Created on: Sep 3, 2021
 *      Author: ASUS
 */
#include<stdint.h>
#include<string.h>
#include<stdio.h>
#include "W25Q128fv.h"
SPI_Handle_t spiW25Q128;
W25Q128Handle_t  W25q128;
uint8_t dummyRead=0;
uint8_t dummyWrite=0xff;

//WRITE ENABLE
uint8_t write_enOrdi(uint8_t EnOrDi)
{
	uint8_t status;
	GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_RESET);
	if(EnOrDi==ENABLE)
	{
		W25Q128fv_Spi(WRITE_EN);
	}
	else
	{
		W25Q128fv_Spi(WRITE_DI);
		EnOrDi=1;
	}
	GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_SET);
	status=(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_WEL)==SET);
	if(~(status ^ EnOrDi))
	{
		return 1;
	}
	else
	{
	return 0;
	}
}


//send and receive data or instruction
uint8_t W25Q128fv_Spi(uint8_t dataOrIns)
{
	uint8_t ret;
	SPI_SendData(W25Q128_SPI,&dataOrIns, 1);
	//clearing RXNE bit
	SPI_ReceiveData(W25Q128_SPI,&ret, 1);
	return ret;
}
//Reading device 24bit Id
uint32_t Read_Device_id(void)
{

	uint32_t temp,temp1,temp2,temp3;
	//cs low
	GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_RESET);

	//Sending read id instruction
	W25Q128fv_Spi(READ_ID);
	//Sending dummy byte to receive id
	temp1 = (uint32_t)W25Q128fv_Spi(dummyWrite);
	temp2 = (uint32_t)W25Q128fv_Spi(dummyWrite);
	temp3 = (uint32_t)W25Q128fv_Spi(dummyWrite);
	//cs high
	GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_SET);
	temp=(temp1<<16)|(temp2<<8)|temp3;
	return temp;
}
//this function sets flash size and all the parameters required to  set W25Q128 Registers
static void SetAndPrintParameters(uint32_t Devid)
{
	W25Q128Handle_t  W25q128;
	W25q128.BlockCount=256;
	W25q128.PageSize=256;
	W25q128.SectorSize=0x1000;
	W25q128.SectorCount=W25q128.BlockCount*16;
	W25q128.PageCount=(W25q128.SectorCount*W25q128.SectorSize)/W25q128.PageSize;
	W25q128.BlockSize=W25q128.SectorSize*16;
	W25q128.CapacityInKiloByte=(W25q128.SectorCount*W25q128.SectorSize)/1024;

	printf("w25q128 Page Size: %u Bytes\r\n",W25q128.PageSize);
	printf("w25q128 Page Count: %lu\r\n",W25q128.PageCount);
	printf("w25q128 Sector Size: %lu Bytes\r\n",W25q128.SectorSize);
	printf("w25q128 Sector Count: %lu\r\n",W25q128.SectorCount);
	printf("w25q128 Block Size: %lu Bytes\r\n",W25q128.BlockSize);
	printf("w25q128 Block Count: %lu\r\n",W25q128.BlockCount);
	printf("w25q128 Capacity: %lu KiloBytes\r\n",W25q128.CapacityInKiloByte);
	printf("w25q128 Init Done\r\n");


}
uint8_t Get_flag_status(uint8_t Reg ,uint8_t flagName)
{
	if(Reg & (1<<flagName))
	{
		return SET;
	}
	return RESET;
}
uint8_t W25Q128_ReadStatusRegister(uint8_t RegSelect)
{
	uint8_t temp=0;
	//cs low
	GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_RESET);
	if(RegSelect==1)
	{
	//Sending instruction
	W25Q128fv_Spi(READ_SR1);
	temp = (uint8_t)W25Q128fv_Spi(dummyWrite);
	W25q128.StatusRegister1=temp;
	}
	else if(RegSelect==2)
	{
	//sending instruction
	W25Q128fv_Spi(READ_SR2);
	temp = (uint8_t)W25Q128fv_Spi(dummyWrite);
	W25q128.StatusRegister2=temp;
	}
	else if(RegSelect==3)
	{
	//sending instruction
	W25Q128fv_Spi(READ_SR3);
	temp = (uint8_t)W25Q128fv_Spi(dummyWrite);
	W25q128.StatusRegister3=temp;
	}
	//cs high
	GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_SET);

	return temp;
}
uint8_t W25Q128_init(void)
{

	//Set a flag to check success and failed condition
	W25q128.Lock =1;
	//initialize gpio pins for spi communication
	W25Q128_spi_pin_config();
	//initialize spi peripheral
	W25Q128_spi_config();
	//this makes NSS signal internally high and avoids MODF error
	SPI_SSIConfig(W25Q128_SPI,ENABLE);
	//Enable the spi peripheral
	SPI_PeripheralControl(W25Q128_SPI, ENABLE);
	//initialize w25q128 chip
	//1.Read id from chip
	GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_SET);

	uint32_t id;
	id=Read_Device_id();
	if((id & (uint32_t)0xFF) == W25Q128fv_id)
	{
		SetAndPrintParameters(id);
		W25Q128_ReadStatusRegister(1);
		W25Q128_ReadStatusRegister(2);
		W25Q128_ReadStatusRegister(3);
		return 1;
	}
	else
	{
		W25q128.Lock=0;
		printf("w25q128 id not Matched\r\n");
		return 0;
	}


}
static void W25Q128_spi_pin_config(void)
{
	GPIO_Handle_t gpioW25Q128,gpioNSS;

	memset(&gpioW25Q128,0,sizeof(gpioW25Q128));
	memset(&gpioNSS,0,sizeof(gpioNSS));

	gpioW25Q128.pGPIOx=W25Q128_SPI_GPIO_PORT;
	gpioW25Q128.GPIO_PinConfig.GPIO_PinAltFunMode=W25Q128_SPI_ALTFUN_MODE;
	gpioW25Q128.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_ALTFN;
	gpioW25Q128.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	gpioW25Q128.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_PIN_PU;
	gpioW25Q128.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_FAST;


	//SCLK
	gpioW25Q128.GPIO_PinConfig.GPIO_PinNumber = W25Q128_SPI_SCLK_PIN;
	GPIO_Init(&gpioW25Q128);

	//MOSI
	gpioW25Q128.GPIO_PinConfig.GPIO_PinNumber = W25Q128_SPI_MOSI_PIN;
	GPIO_Init(&gpioW25Q128);

	//MISO
	gpioW25Q128.GPIO_PinConfig.GPIO_PinNumber = W25Q128_SPI_MISO_PIN;
	GPIO_Init(&gpioW25Q128);


	//NSS
	gpioW25Q128.GPIO_PinConfig.GPIO_PinNumber = W25Q128_SPI_NSS_PIN;
	GPIO_Init(&gpioW25Q128);

	//we are using separate gpio pin to control cs signal for memory chip
	//& We are connecting original NSS pin to vcc to avoid MODF error
	gpioNSS.pGPIOx=W25Q128_SPI_CS_PORT;
	gpioNSS.GPIO_PinConfig.GPIO_PinMode=GPIO_MODE_OUT;
	gpioNSS.GPIO_PinConfig.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	gpioNSS.GPIO_PinConfig.GPIO_PinPuPdControl=GPIO_NO_PUPD;
	gpioNSS.GPIO_PinConfig.GPIO_PinSpeed=GPIO_SPEED_FAST;
	gpioNSS.GPIO_PinConfig.GPIO_PinNumber = W25Q128_SPI_CS_PIN;

	GPIO_Init(&gpioNSS);

}

static void W25Q128_spi_config(void)
{

	spiW25Q128.pSPIx=W25Q128_SPI;
	spiW25Q128.SPIConfig.SPI_BusConfig=W25Q128_SPI_BUS_CONFIG;
	spiW25Q128.SPIConfig.SPI_DeviceMode=W25Q128_SPI_DEVICE_MODE;
	spiW25Q128.SPIConfig.SPI_DFF=SPI_DFF_8BITS;
	spiW25Q128.SPIConfig.SPI_SSM=W25Q128_SPI_SSM;
	spiW25Q128.SPIConfig.SPI_SclkSpeed=W25Q128_SPI_SCLK_SPEED;
	spiW25Q128.SPIConfig.SPI_CPHA=SPI_CPOL_LOW;
	spiW25Q128.SPIConfig.SPI_CPOL=SPI_CPHA_LOW;

	SPI_Init(&spiW25Q128);
}

void W25Q128_WriteByte(uint8_t pTxbuf,uint32_t writeAddr)
{
	while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);
	uint8_t status=write_enOrdi(ENABLE);
	if(status)
	{
	//CS LOW
	GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_RESET);
	W25Q128fv_Spi(PAGE_WRT);
	W25Q128fv_Spi((uint32_t)writeAddr);
	W25Q128fv_Spi(pTxbuf);
	//CS LOW
	GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_SET);
	}
	else
	{
		printf("Writing byte failed\r\n");
		while(1);
	}

}

//This function sends 24bit instruction using spi
void W25Q128fv_SpiADDR(uint32_t Addr)
{
	uint8_t NewAddr;
	NewAddr=(uint8_t)((Addr>>16) & 0xFF);
	W25Q128fv_Spi(NewAddr);
	NewAddr=(uint8_t)((Addr>>8) & 0xFF);
	W25Q128fv_Spi(NewAddr);
	NewAddr=(uint8_t)((Addr>>0) & 0xFF);
	W25Q128fv_Spi(NewAddr);

}

void W25Q128_WritePage(uint8_t *pTxbuf,uint32_t writeAddr,uint32_t NoofbytestoWrite,uint32_t offsetInBytes)
{
	uint32_t NewAddr=0;
	if(NoofbytestoWrite>W25q128.PageSize || NoofbytestoWrite==0)
	{
		NoofbytestoWrite=W25q128.PageSize-offsetInBytes;
	}
	while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);
	uint8_t status=write_enOrdi(ENABLE);
	if(status)
	{

		//CS LOW
		GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_RESET);
		W25Q128fv_Spi(PAGE_WRT);
		/*************************************************/
		//Sending Address
		if(NoofbytestoWrite>=W25q128.PageSize)
		{
			//making lsb 0 to avoid overwriting of page
			NewAddr= (writeAddr & ~((uint32_t)0xFF));
			W25Q128fv_SpiADDR(NewAddr);
		}
		else
		{
			NewAddr=writeAddr;
			W25Q128fv_SpiADDR(NewAddr);
		}
		/*************************************************/
		//Sending Data
			for(uint16_t i=0;i<NoofbytestoWrite;i++)
			{
				W25Q128fv_Spi(*pTxbuf);
				pTxbuf++;
				NoofbytestoWrite--;
			}
		/*************************************************/
		//CS high
		GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_SET);
		while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);
	}
	else
	{
		printf("Writing Page failed\r\n");
		while(1);
	}

}

//This function write sector
//If you are writing from middle of the sector then offset is given to select that memory location to write.
//NumofBytetoWrite is number bytes to be written into chip from write_addr
void  W25Q128_WriteSector(uint8_t *pTxbuf,uint32_t writeAddr,uint32_t NumofBytestowrite,uint32_t offsetInBytes)
{
	if(NumofBytestowrite>=W25q128.SectorSize || NumofBytestowrite=0)
	{
		NumofBytestowrite=W25q128.SectorSize;
	}
	if(offsetInBytes>W25q128.SectorSize)
	{
		printf("Sector write failed");
		return;
	}
	uint32_t NoofbytestoWrite_page=0;
	uint32_t writeAddr_page=W25qxx_SectorToPage(writeAddr)+(offsetInBytes/W25q128.PageSize);//pageaddr + 1 out of 16 page
	uint32_t offsetInBytes_page=offsetInBytes % W25q128.PageSize;
	if((offsetInBytes+NumofBytestowrite)>W25q128.SectorSize)
	{
		NoofbytestoWrite_page=W25q128.SectorSize-offsetInBytes;
	}
	else
	{
		NoofbytestoWrite_page=NoofbytestoWrite;
	}
		while(NoofbytestoWrite_page>0)
		{
			//here we are writing a page on which offset is lying
			//after writing this page if Noofbytestowrite are not zero then we are again writing next page.
			W25Q128_WritePage(pTxbuf, writeAddr_page, NoofbytestoWrite_page, offsetInBytes_page);
			NoofbytestoWrite_page=W25q128.PageSize-offsetInBytes_page;
			writeAddr_page++;
			pTxbuf=pTxbuf+NoofbytestoWrite_page;
			offsetInBytes_page=0;
		}
}

void W25Q128_SectorErase(uint32_t Sector_Addr)
{
	while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);
	uint8_t status=write_enOrdi(ENABLE);
	if(status)
	{
		//CS LOW
		GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_RESET);
		W25Q128fv_Spi(ERASE_SECTOR);
		/*************************************************/
		//Sending Address
		W25Q128fv_SpiADDR((uint32_t)Sector_Addr);

		//CS high
		GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_SET);

		//wait for instruction to finish up
		while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);

	}
	else
	{
		printf("Sector erasing failed\r\n");
		while(1);
	}


}

void W25Q128_EraseChip(void)
{
	while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);
		uint8_t status=write_enOrdi(ENABLE);
		if(status)
		{
			//CS LOW
			GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_RESET);
			W25Q128fv_Spi(ERASE_CHIP);

			//CS high
			GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_SET);

			//wait for instruction to finish up
			while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);

		}
		else
		{
			printf("Chip erasing failed\r\n");
			while(1);
		}

}

void W25qxx_ReadBytes(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{

	uint8_t temp;
	while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);
			uint8_t status=write_enOrdi(ENABLE);
			if(status)
			{
				//CS LOW
				GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_RESET);
				W25Q128fv_Spi(READ_BYTES);

				W25Q128fv_SpiADDR(ReadAddr);

				for(uint32_t i=0; i<NumByteToRead;i++)
				{
					//Sending dummy byte to receive databyte
					temp = (uint32_t)W25Q128fv_Spi(dummyWrite);
					*pBuffer=temp;
					 pBuffer++;

				}
				//CS High
				GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_SET);
				while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);

			}
			else
			{
				printf("Read Bytes failed");
			}
}


void W25qxx_ReadPage(uint8_t* pBuffer, uint32_t ReadAddr,uint8_t offset, uint32_t NumByteToRead_upto_page_size)
{
		uint8_t temp;
		while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);
				uint8_t status=write_enOrdi(ENABLE);
				if(status)
				{
					//CS LOW
					GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_RESET);
					W25Q128fv_Spi(READ_BYTES);

					W25Q128fv_SpiADDR(ReadAddr);

					for(uint32_t i=0; i<NumByteToRead_upto_page_size;i++)
					{
						//Sending dummy byte to receive databyte
						temp = (uint32_t)W25Q128fv_Spi(dummyWrite);
						*pBuffer=temp;
						 pBuffer++;

					}
					//CS High
					GPIO_WriteToOutputPin(W25Q128_SPI_CS_PORT, W25Q128_SPI_CS_PIN, GPIO_PIN_SET);
					while(Get_flag_status(W25Q128_ReadStatusRegister(1),W25Q128fv_FLAG_BUSY)!=RESET);

				}
				else
				{
					printf("Read Bytes failed");
				}


}
//###################################################################################################################
uint32_t	W25qxx_PageToSector(uint32_t	PageAddress)
{
	return ((PageAddress*w25qxx.PageSize)/w25qxx.SectorSize);
}
//###################################################################################################################
uint32_t	W25qxx_PageToBlock(uint32_t	PageAddress)
{
	return ((PageAddress*w25qxx.PageSize)/w25qxx.BlockSize);
}
//###################################################################################################################
uint32_t	W25qxx_SectorToBlock(uint32_t	SectorAddress)
{
	return ((SectorAddress*w25qxx.SectorSize)/w25qxx.BlockSize);
}
//###################################################################################################################
uint32_t	W25qxx_SectorToPage(uint32_t	SectorAddress)
{
	return (SectorAddress*w25qxx.SectorSize)/w25qxx.PageSize;
}
//###################################################################################################################
uint32_t	W25qxx_BlockToPage(uint32_t	BlockAddress)
{
	return (BlockAddress*w25qxx.BlockSize)/w25qxx.PageSize;
}
__weak void W25Q128_ApplicationEventCallback(SPI_Handle_t *pSPIHandle,uint8_t AppEv)
{

	//This is a weak implementation . the user application may override this function.
}

