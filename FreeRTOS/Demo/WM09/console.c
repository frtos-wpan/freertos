/*
 * console.c - Serial console
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */


#include "FreeRTOS.h"

#include "task.h"
//#include "partest.h"
#include "flash.h"

#include STM32_CONF_H

#include "console.h"


void (*console_recv)(const char *buf, unsigned n) = NULL;


void console_send_isr(const char *buf, unsigned len)
{
	while (len--) {
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, *buf++);
	}
}


void console_send(const char *buf, unsigned len)
{
	console_send_isr(buf, len);
}


void USART1_IRQHandler(void)
{
	char ch;

	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
		ch = USART_ReceiveData(USART1);
		if (console_recv)
			console_recv(&ch, 1);
	}
}


void console_init(void)
{
	static USART_InitTypeDef uart_init = {
		.USART_BaudRate		= 115200,
		.USART_WordLength	= USART_WordLength_8b,
		.USART_StopBits		= USART_StopBits_1,
		.USART_Parity		= USART_Parity_No,
		.USART_Mode		= USART_Mode_Tx | USART_Mode_Rx,
		.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
	};
	static GPIO_InitTypeDef gpio_init = {
		.GPIO_Pin	= GPIO_Pin_9 | GPIO_Pin_10,
		.GPIO_Mode	= GPIO_Mode_AF,
		.GPIO_Speed	= GPIO_Speed_2MHz,
		.GPIO_OType	= GPIO_OType_PP,
		.GPIO_PuPd	= GPIO_PuPd_NOPULL,
	};
	static NVIC_InitTypeDef nvic_init = {
		.NVIC_IRQChannel = USART1_IRQn,
		.NVIC_IRQChannelPreemptionPriority = 8, /* 0-15; @@@ ? */
		.NVIC_IRQChannelSubPriority = 0,	/* not on FreeRTOS */
		.NVIC_IRQChannelCmd = ENABLE,
	};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	GPIO_Init(GPIOA, &gpio_init);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_Init(USART1, &uart_init);
	USART_Cmd(USART1, ENABLE);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_GetITStatus(USART1, USART_IT_RXNE);	/* why ? @@@ */
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);

	NVIC_Init(&nvic_init);
}
