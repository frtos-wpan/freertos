/*
 * gpio.c - GPIO interface
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */


#include <stdbool.h>
#include <stdint.h>

#include STM32_CONF_H

#include "platform.h"
#include "gpio.h"


/* ----- GPIOx value (by GPIO bank index) ---------------------------------- */


GPIO_TypeDef *const gpiox[] = {
#ifdef GPIOA
	GPIOA,
#endif
#ifdef GPIOB
	GPIOB,
#endif
#ifdef GPIOC
	GPIOC,
#endif
#ifdef GPIOD
	GPIOD,
#endif
#ifdef GPIOE
	GPIOE,
#endif
#ifdef GPIOF
	GPIOF,
#endif
#ifdef GPIOG
	GPIOG,
#endif
#ifdef GPIOH
	GPIOH,
#endif
#ifdef GPIOI
	GPIOI,
#endif
};


/* ----- RCC_AHB1Periph_GPIOx value (by GPIO bank index) ------------------- */


#define	GPIO_BANKS	(sizeof(gpio_rcc) / sizeof(*gpio_rcc))


static const uint32_t gpio_rcc[] = {
#ifdef GPIOA
	RCC_AHB1Periph_GPIOA,
#endif
#ifdef GPIOB
	RCC_AHB1Periph_GPIOB,
#endif
#ifdef GPIOC
	RCC_AHB1Periph_GPIOC,
#endif
#ifdef GPIOD
	RCC_AHB1Periph_GPIOD,
#endif
#ifdef GPIOE
	RCC_AHB1Periph_GPIOE,
#endif
#ifdef GPIOF
	RCC_AHB1Periph_GPIOF,
#endif
#ifdef GPIOG
	RCC_AHB1Periph_GPIOG,
#endif
#ifdef GPIOH
	RCC_AHB1Periph_GPIOH,
#endif
#ifdef GPIOI
	RCC_AHB1Periph_GPIOI,
#endif
};


/* ----- In/out/function configuration ------------------------------------- */


#define	NUM_GPIO		(GPIO_BANKS*32)


static signed char pull_state[NUM_GPIO];	/* default to no pull */
static bool out_state[NUM_GPIO];		/* default to input */


void gpio_update(unsigned id)
{
	GPIO_InitTypeDef gpio_init = {
		.GPIO_Pin       = 1 << gpio_id_bit(id),
		.GPIO_Mode      = out_state[id] ? GPIO_Mode_OUT : GPIO_Mode_IN,
		.GPIO_Speed     = GPIO_Speed_25MHz,
		.GPIO_OType     = GPIO_OType_PP,
		.GPIO_PuPd      = pull_state[id] ?
				    pull_state[id] > 0 ?
				      GPIO_PuPd_UP : GPIO_PuPd_DOWN :
				    GPIO_PuPd_NOPULL,
	};

	GPIO_Init(gpiox[gpio_id_port(id)], &gpio_init);
}


void gpio_inout(unsigned id, bool out)
{
	out_state[id] = out;
	gpio_update(id);
}


void gpio_r(unsigned id, signed char pull)
{
	pull_state[id] = pull;
	gpio_update(id);
}


void gpio_af(unsigned id, uint8_t af)
{
	GPIO_InitTypeDef gpio_init = {
		.GPIO_Pin	= 1 << gpio_id_bit(id),
		.GPIO_Mode	= GPIO_Mode_AF,
		.GPIO_Speed	= GPIO_Speed_25MHz,
		.GPIO_OType	= GPIO_OType_PP,
		.GPIO_PuPd	= GPIO_PuPd_DOWN,
	};

	GPIO_PinAFConfig(gpiox[gpio_id_port(id)], gpio_id_bit(id), af);
	GPIO_Init(gpiox[gpio_id_port(id)], &gpio_init);
}


/* ----- GPIO enable/disable ----------------------------------------------- */


/* Number of uses of a GPIO bank. 0 if disabled.  */

static int gpio_enabled[GPIO_BANKS];


int gpio_num(GPIO_TypeDef *gpio)
{
#ifdef GPIOA
	if (gpio == GPIOA)
		return 0;
#endif
#ifdef GPIOB
	if (gpio == GPIOB)
		return 1;
#endif
#ifdef GPIOC
	if (gpio == GPIOC)
		return 2;
#endif
#ifdef GPIOD
	if (gpio == GPIOD)
		return 3;
#endif
#ifdef GPIOE
	if (gpio == GPIOE)
		return 4;
#endif
#ifdef GPIOF
	if (gpio == GPIOF)
		return 5;
#endif
#ifdef GPIOG
	if (gpio == GPIOG)
		return 6;
#endif
#ifdef GPIOH
	if (gpio == GPIOH)
		return 7;
#endif
#ifdef GPIOI
	if (gpio == GPIOI)
		return 8;
#endif
	return -1;
}


unsigned gpio_enable(GPIO_TypeDef *gpio, int bit)
{
	int n;

	n = gpio_num(gpio);
	if (!gpio_enabled[n]++)
		RCC_AHB1PeriphClockCmd(gpio_rcc[n], ENABLE);
	return gpio_id(n, bit);
}


void gpio_disable(unsigned id)
{
	unsigned n;

	n = gpio_id_port(id);
	if (!--gpio_enabled[n])
		RCC_AHB1PeriphClockCmd(gpio_rcc[n], DISABLE);
}
