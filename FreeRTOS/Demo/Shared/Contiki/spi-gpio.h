/*
 * spi-gpio.h - Bit-banging SPI interface
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

#ifndef SPI_GPIO_H
#define	SPI_GPIO_H

struct spi {
	unsigned mosi, miso, sclk, nsel;
};


#define	SPI_GPIO_DEV(_mosi, _miso, _sclk, _nsel) \
	(struct spi) {				\
		.mosi = GPIO_ENABLE(_mosi),	\
		.miso = GPIO_ENABLE(_miso),	\
		.sclk = GPIO_ENABLE(_sclk),	\
		.nsel = GPIO_ENABLE(_nsel)	\
	}

#endif /* !SPI_GPIO_H */
