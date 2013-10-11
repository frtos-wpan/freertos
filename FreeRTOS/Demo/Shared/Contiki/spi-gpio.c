/*
 * spi-gpio.c - Bit-banging SPI interface
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */


#include <stdint.h>

#include "gpio.h"
#include "spi-gpio.h"


void spi_begin(const struct spi *spi)
{
	gpio_clr(spi->nsel);
}


void spi_end(const struct spi *spi)
{
	gpio_set(spi->nsel);
}


void spi_send(const struct spi *spi, uint8_t v)
{
	uint8_t mask;

	for (mask = 0x80; mask; mask >>= 1) {
		if (v & mask)
			gpio_set(spi->mosi);
		else
			gpio_clr(spi->mosi);
		gpio_set(spi->sclk);
		gpio_set(spi->sclk);
		gpio_set(spi->sclk);
		gpio_clr(spi->sclk);
	}
}


void spi_begin_rx(const struct spi *spi)
{
}


uint8_t spi_recv(const struct spi *spi)
{
	uint8_t res = 0;
	uint8_t mask;

	for (mask = 0x80; mask; mask >>= 1) {
		if (gpio_read(spi->miso))
			res |= mask;
		gpio_set(spi->sclk);
		gpio_set(spi->sclk);
		gpio_set(spi->sclk);
		gpio_clr(spi->sclk);
	}
	return res;
}


void spi_init(const struct spi *spi)
{
	gpio_clr(spi->sclk);
	gpio_set(spi->nsel);

	gpio_output(spi->mosi);
	gpio_input(spi->miso);
	gpio_output(spi->sclk);
	gpio_output(spi->nsel);
}
