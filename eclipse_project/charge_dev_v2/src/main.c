#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "misc.h"

#include "lcd_1100/nokia1100_lcd_lib.h"
#include "display/images_printing.h"
#include "common/common.h"
#include "adc/adc.h"
#include "timers/timers.h"
#include "encoder/encoder.h"
#include "led/led.h"

void initPorts() {
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

void changeAmperageMenu() {
	nlcd_Clear();
	printInitChangeAmperageMenu();
	printChangeAmperageMenuDigits(amperage);
	uint64_t timeStampMenu = time_ms;
	uint64_t timeStampDisplayRefr = time_ms;
	while (!isMenuEncoderPressed() && time_ms - timeStampMenu < 5000) {
		if (time_ms - timeStampDisplayRefr > 500) {
			printChangeAmperageMenuDigits(amperage);
			timeStampDisplayRefr = time_ms;
		}
		if (encoderIsActive) {
			timeStampMenu = time_ms;
			if (getEncoderDirection() == FORWARD) {
				if (BATTARY_POWER_OUT > 95) {
					BATTARY_POWER_OUT = 100;
				} else {
					BATTARY_POWER_OUT += 5;
				}
			} else if (getEncoderDirection() == BACKWARD) {
				if (BATTARY_POWER_OUT < 1) {
					BATTARY_POWER_OUT = 0;
				} else {
					BATTARY_POWER_OUT -= 5;
				}
			}
			printChangeAmperageImage(batteryPower / 10);
		}
	}
	while (isMenuEncoderPressed()) {
		__NOP();
	}
	nlcd_Clear();
}

void additinalMenu() {
	while (isMenuEncoderPressed()) {
		__NOP();
	}
	nlcd_Clear();
	printAdditionalMenuDigits(amperage);
	char animationIdx = 0;
	while (!isMenuEncoderPressed()) {
		if (++animationIdx > 5) {
			animationIdx = 0;
		}
		printAdditionalMenuAnimation(animationIdx);
		delayMs(500);
	}
	while (isMenuEncoderPressed()) {
		__NOP();
	}
	nlcd_Clear();
}

void testOutputs() {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_ODR_ODR6 | GPIO_ODR_ODR14
			| GPIO_ODR_ODR15 | GPIO_ODR_ODR10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIOB->ODR |= GPIO_ODR_ODR6;
//	delayMs(200);
//	GPIOB->ODR |= GPIO_ODR_ODR10;
//	delayMs(200);
	GPIOB->ODR |= GPIO_ODR_ODR14;
//	delayMs(200);
//	GPIOB->ODR |= GPIO_ODR_ODR15;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//	GPIO_InitStructure.GPIO_Pin = GPIO_ODR_ODR6 | GPIO_ODR_ODR7 | GPIO_ODR_ODR8
//			| GPIO_ODR_ODR9;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
////	delayMs(200);
////	GPIOC->ODR |= GPIO_ODR_ODR6;
////	delayMs(200);
//	GPIOC->ODR |= GPIO_ODR_ODR7;
////	delayMs(200);
////	GPIOC->ODR |= GPIO_ODR_ODR8;
////	delayMs(200);
////	GPIOC->ODR |= GPIO_ODR_ODR9;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	GPIO_InitStructure.GPIO_Pin = GPIO_ODR_ODR8 | GPIO_ODR_ODR9 | GPIO_ODR_ODR10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	GPIOA->ODR |= GPIO_ODR_ODR8;
//	GPIOA->ODR |= GPIO_ODR_ODR9;
//	GPIOA->ODR |= GPIO_ODR_ODR10;
}

void testDisplPins() {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_PORT, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SCLK_LCD_PIN | SDA_LCD_PIN | CS_LCD_PIN
			| RST_LCD_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_LCD, &GPIO_InitStructure);
	PORT_LCD |= SCLK_LCD_PIN;
//	PORT_LCD |= SDA_LCD_PIN;
//	PORT_LCD |= CS_LCD_PIN;
//	PORT_LCD |= RST_LCD_PIN;
}

int main(void) {
	SystemCoreClockUpdate();
	__enable_irq();
	initTimerCounter();
	initPorts();
	nlcd_Init();
//	bootMessage();
	initADC();
	initPWM4();
	initEncoder();
	initLeds();
	nlcd_Clear();
	clearMeasuringBuffers();
	printMainImageTemplate();

	while (1) {
		checkIfBattaryIsConnected();
		if (encoderIsActive) {
			changeAmperageMenu();
			printMainImageTemplate();
		}
		printMainImageDigits(1, uToBattery);
		printMainImageDigits(2, uFromBattery);
	}
	return 0;
}
