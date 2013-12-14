/*
 * platform.h - Platform-specific definitions
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */


#ifndef PLATFORM_H
#define	PLATFORM_H

#include STM32_CONF_H


/* ----- SPI settings ------------------------------------------------------ */

/*
 * SD/MMC pin	atben signal	GPIO
 *				uSD		STM32-E407+odev
 * ----------	------------	-------------------------------
 * DAT2		IRQ		PC10		PG10
 * DAT3		nSEL		PC11		PB9
 * CMD		MOSI		PD2		PC3 / SPI2_MOSI
 * CLK		SLP_TR		PC12		PB8
 * DAT0		MISO		PC8		PC2 / SPI2_MISO
 * DAT1		SCLK		PC9		PB10 / SPI2_SCK
 */


/* ----- uSD slot ---------------------------------------------------------- */


#ifndef ODEV

#define	PORT_IRQ	GPIOC
#define	BIT_IRQ		10
#define	PORT_nSEL	GPIOC
#define	BIT_nSEL	11
#define	PORT_MOSI	GPIOD
#define	BIT_MOSI	2
#define	PORT_SLP_TR	GPIOC
#define	BIT_SLP_TR	12
#define	PORT_MISO	GPIOC
#define	BIT_MISO	8
#define	PORT_SCLK	GPIOC
#define	BIT_SCLK	9

#endif /* !ODEV */


/* ----- ODEV (GPIO or SPI) ------------------------------------------------ */


#ifdef ODEV

#define	PORT_IRQ	GPIOG
#define	BIT_IRQ		10
#define	PORT_nSEL	GPIOB
#define	BIT_nSEL	9
#define	PORT_MOSI	GPIOC
#define	BIT_MOSI	3
#define	PORT_SLP_TR	GPIOB
#define	BIT_SLP_TR	8
#define	PORT_MISO	GPIOC
#define	BIT_MISO	2
#define	PORT_SCLK	GPIOB
#define	BIT_SCLK	10

#endif /* ODEV */


/* @@@ this feature probe is a little ugly ...  */

#include "spi.h"
#ifdef SPI_STM32_H
#define	SPI_DEV_INIT	SPI_STM32_DEV(SPI2, MOSI, MISO, SCLK, nSEL, \
			    SPI_BaudRatePrescaler_8)
			    /* APB1 = 42 MHz; 42 MHz / 8 = 5.25 MHz */
#else
#define	SPI_DEV_INIT	SPI_GPIO_DEV(MOSI, MISO, SCLK, nSEL)
#endif

#endif /* !PLATFORM_H */
