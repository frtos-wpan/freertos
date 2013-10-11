/*
 * extint.h - External (GPIO) interrupt interface
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

#ifndef EXTINT_H
#define	EXTINT_H

#include <stdbool.h>

#include STM32_CONF_H


#define	__IRQ_0		EXTI0_IRQ
#define	__IRQ_1		EXTI1_IRQ
#define	__IRQ_2		EXTI2_IRQ
#define	__IRQ_3		EXTI3_IRQ
#define	__IRQ_4		EXTI4_IRQ
#define	__IRQ_5		EXTI9_5_IRQ
#define	__IRQ_6		EXTI9_5_IRQ
#define	__IRQ_7		EXTI9_5_IRQ
#define	__IRQ_8		EXTI9_5_IRQ
#define	__IRQ_9		EXTI9_5_IRQ
#define	__IRQ_10	EXTI15_10_IRQ
#define	__IRQ_11	EXTI15_10_IRQ
#define	__IRQ_12	EXTI15_10_IRQ
#define	__IRQ_13	EXTI15_10_IRQ
#define	__IRQ_14	EXTI15_10_IRQ
#define	__IRQ_15	EXTI15_10_IRQ

/*
 * In the following  helper macros, we first compose a new token, expand it in
 * the next level, then compose the next token, and so on.
 *
 * For example, IRQ_HANDLER(IRQ) with BIT_IRQ defined as 0 becomes
 *   __IRQ_HANDLER_1(BIT_IRQ)
 *   __IRQ_HANDLER_2(0)
 *   __IRQ_HANDLER_3(__IRQ_0)
 *   __IRQ_HANDLER_4(EXTI0_IRQ)
 * and finally yields
 *   EXTI0_IRQHandler
 */

#define	__IRQ_HANDLER_4(irq)	irq##Handler
#define	__IRQ_HANDLER_3(irq)	__IRQ_HANDLER_4(irq)
#define	__IRQ_HANDLER_2(bit)	__IRQ_HANDLER_3(__IRQ_##bit)
#define	__IRQ_HANDLER_1(bit)	__IRQ_HANDLER_2(bit)
#define	IRQ_HANDLER(pin)	void __IRQ_HANDLER_1(BIT_##pin)(void)


#define	EXTINT_ENABLE(pin)	extint_config(BIT_##pin, 1)
#define	EXTINT_DISABLE(pin)	extint_config(BIT_##pin, 0)
#define	EXTINT_SETUP(pin)	extint_setup(PORT_##pin, BIT_##pin)

void extint_config(int bit, bool enable);
void extint_setup(GPIO_TypeDef *gpio, int bit);

#endif /* !EXTINT_H */
