
/*****************************************************************/
//this code is to communicate between two stm32 boards and external button
/****************************************************************/
/*extbutn---A3 to stmf407 -----A0 to    stmf429-------- E1
 stmf429 A0   to stmf407 A1
 */


#include<string.h>
#include "stm32f407xx.h"

#define HIGH 1
#define LOW 0
#define BTN_PRESSED LOW

void delay(void)
{
	// this will introduce ~200ms delay when system clock is 16MHz
	for(uint32_t i = 0 ; i < 500000/6 ; i ++);
}
void delay2(void)
{
	// this will introduce ~200ms delay when system clock is 16MHz
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}

int main(void)
{

	GPIO_Handle_t GpioLed, GPIOBtn,GPIOsend,GPIOrtrn,GPIOExtBtn;


	memset(&GpioLed,0,sizeof(GpioLed));
	memset(&GPIOBtn,0,sizeof(GPIOBtn));
	memset(&GPIOsend,0,sizeof(GpioLed));
	memset(&GPIOrtrn,0,sizeof(GPIOrtrn));

	//led structure
	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
	//GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_Init(&GpioLed);

	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_Init(&GpioLed);

	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	GPIO_Init(&GpioLed);

	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	GPIO_Init(&GpioLed);

	// send intrupt structure
	GPIOsend.pGPIOx = GPIOD;
	GPIOsend.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOsend.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIOsend.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
	GPIOsend.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIOsend.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GPIOsend);

	//receive interupt structure
	GPIOrtrn.pGPIOx = GPIOA;
	GPIOrtrn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	GPIOrtrn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GPIOrtrn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOrtrn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;
	GPIO_Init(&GPIOrtrn);

	//this is btn gpio configuration
	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;
	GPIO_Init(&GPIOBtn);


	//this is exeternal btn gpio configuration
	GPIOExtBtn.pGPIOx = GPIOA;
	GPIOExtBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIOExtBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GPIOExtBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOExtBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIO_Init(&GPIOExtBtn);




	GPIO_WriteToOutputPin(GPIOD,GPIO_PIN_NO_13,GPIO_PIN_RESET);

	//IRQ configurations
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0,NVIC_IRQ_PRI1);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0,ENABLE);

	GPIO_IRQPriorityConfig(IRQ_NO_EXTI1,NVIC_IRQ_PRI0);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI1,ENABLE);



	GPIO_IRQPriorityConfig(IRQ_NO_EXTI3,NVIC_IRQ_PRI15);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI3,ENABLE);

    while(1);

}


void EXTI0_IRQHandler(void)
{
    delay(); //200ms . wait till button de-bouncing gets over
    GPIO_WriteToOutputPin(GPIOD, GPIO_PIN_NO_0,HIGH);
    delay();
    GPIO_WriteToOutputPin(GPIOD, GPIO_PIN_NO_0,LOW);
    GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_15);
    delay();
    GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_14);
    delay();
    GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_13);
    delay();
    GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
    delay();
    GPIO_IRQHandling(GPIO_PIN_NO_0); //clear the pending event from exti line

}
void EXTI1_IRQHandler(void)
{
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
	delay();
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_13);
	delay();
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_14);
	delay();
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_15);
	delay();
	 GPIO_IRQHandling(GPIO_PIN_NO_1);
}

void EXTI3_IRQHandler(void)
{
	delay();
	GPIO_WriteToOutputPin(GPIOD, GPIO_PIN_NO_0,HIGH);
	delay();
	GPIO_WriteToOutputPin(GPIOD, GPIO_PIN_NO_0,LOW);
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
	delay();
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_13);
	delay();
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_14);
	delay();
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_15);
	GPIO_IRQHandling(GPIO_PIN_NO_3);
}

