/*
 * gpio.h - GPIO interface
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

/*
 * @@@ WIP: very STM32 specific
 */

#ifndef GPIO_H
#define	GPIO_H

#include <stdbool.h>
#include <stdint.h>

#include STM32_CONF_H


/* ----- GPIO ID (single number to identify pin) --------------------------- */


static inline unsigned gpio_id(int port, int bit)
{
	return port << 5 | bit;
}


static inline unsigned gpio_id_port(unsigned id)
{
	return id >> 5;
}


static inline unsigned gpio_id_bit(unsigned id)
{
	return id & 31;
}


/* ----- Pin operations ---------------------------------------------------- */


extern GPIO_TypeDef *const gpiox[];


void gpio_inout(unsigned id, bool out);
void gpio_r(unsigned id, signed char pull);


static inline void gpio_output(unsigned id)
{
	gpio_inout(id, 1);
}


static inline void gpio_input(unsigned id)
{
	gpio_inout(id, 0);
}


static inline void gpio_set(unsigned id)
{
	GPIO_SetBits(gpiox[gpio_id_port(id)], 1 << gpio_id_bit(id));
}


static inline void gpio_clr(unsigned id)
{
	GPIO_ResetBits(gpiox[gpio_id_port(id)], 1 << gpio_id_bit(id));
}


static inline bool gpio_read(unsigned id)
{
	return GPIO_ReadInputDataBit(gpiox[gpio_id_port(id)],
	    1 << gpio_id_bit(id)) == Bit_SET;
}


/* ----- Setup ------------------------------------------------------------- */


#define	GPIO_ENABLE(pin)		gpio_enable(PORT_##pin, BIT_##pin)


int gpio_num(GPIO_TypeDef *gpio);

void gpio_af(unsigned id, uint8_t af);

unsigned gpio_enable(GPIO_TypeDef *gpio, int bit);
void gpio_disable(unsigned id);

#endif /* !GPIO_H */
