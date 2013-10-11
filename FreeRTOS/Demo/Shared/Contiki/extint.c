/*
 * extint.c - External (GPIO) interrupt interface
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

#include <stdbool.h>

#include STM32_CONF_H

#include "gpio.h"
#include "extint.h"


static int irq_n[] = {
	EXTI0_IRQn,	EXTI1_IRQn,	EXTI2_IRQn,	EXTI3_IRQn,	/* 0 */
	EXTI4_IRQn,	EXTI9_5_IRQn,	EXTI9_5_IRQn,	EXTI9_5_IRQn,	/* 4 */
	EXTI9_5_IRQn,	EXTI9_5_IRQn,	EXTI15_10_IRQn,	EXTI15_10_IRQn,	/* 8 */
	EXTI15_10_IRQn,	EXTI15_10_IRQn,	EXTI15_10_IRQn,	EXTI15_10_IRQn	/* 12 */
};


void extint_config(int bit, bool enable)
{
	EXTI_InitTypeDef exti_init = {
		.EXTI_Line	=  1 << bit,
		.EXTI_Mode	= EXTI_Mode_Interrupt,
		.EXTI_Trigger	= EXTI_Trigger_Rising,
		.EXTI_LineCmd	= enable ? ENABLE : DISABLE,
        };

	EXTI_Init(&exti_init);
}


void extint_setup(GPIO_TypeDef *gpio, int bit)
{
	NVIC_InitTypeDef nvic_init = {
		.NVIC_IRQChannel	= irq_n[bit],
		.NVIC_IRQChannelPreemptionPriority = 8,	/* 0-15; @@@ ? */
		.NVIC_IRQChannelSubPriority = 0,	/* not on FreeRTOS */
		.NVIC_IRQChannelCmd	= ENABLE,
	};
	unsigned id;

	id = gpio_enable(gpio, bit);
	gpio_inout(id, 0);	/* make input */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	NVIC_Init(&nvic_init);
	SYSCFG_EXTILineConfig(gpio_num(gpio), bit);
}
