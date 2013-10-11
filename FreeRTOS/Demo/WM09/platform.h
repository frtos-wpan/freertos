/*
 * platform.h - Platform-specific definitions
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */


#ifndef PLATFORM_H
#define	PLATFORM_H


#include STM32_CONF_H


/* ----- DEV9 settings ----------------------------------------------------- */


/*
 * SD/MMC pin	atben signal	GPIO (WM09+dev9)
 * ----------	------------	---------------	---------------
 * DAT2		IRQ		PA0
 * DAT3		nSEL		PC5
 * CMD		MOSI		PA7 / SPI1_MOSI
 * CLK		SLP_TR		PA3
 * DAT0		MISO		PA6 / SPI1_MISO
 * DAT1		SCLK		PA5 / SPI1_SCK
 */


#define	PORT_IRQ	GPIOA
#define	BIT_IRQ		0
#define	PORT_nSEL	GPIOC
#define	BIT_nSEL	5
#define	PORT_MOSI	GPIOA
#define	BIT_MOSI	7
#define	PORT_SLP_TR	GPIOA
#define	BIT_SLP_TR	3
#define	PORT_MISO	GPIOA
#define	BIT_MISO	6
#define	PORT_SCLK	GPIOA
#define	BIT_SCLK	5

/* @@@ this feature probe is a little ugly ...  */

#include "spi.h"
#ifdef SPI_STM32_H
#define	SPI_DEV_INIT	SPI_STM32_DEV(SPI1, MOSI, MISO, SCLK, nSEL, \
			    SPI_BaudRatePrescaler_8)
			    /* APB2 = 60 MHz; 60 MHz / 8 = 7.5 MHz */
#else
#define	SPI_DEV_INIT	SPI_GPIO_DEV(MOSI, MISO, SCLK, nSEL)
#endif

#endif /* !PLATFORM_H */
