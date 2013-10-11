/*
 * spi.h - General SPI interface
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

#ifndef SPI_H
#define	SPI_H

#include <stdint.h>


/* include $(SPI).h */

#define	__HDR_2(file)	#file
#define	__HDR_1(name)	__HDR_2(name.h)
#define	__HDR		__HDR_1(SPI)

#include __HDR

#undef	__HDR
#undef	__HDR_1
#undef	__HDR_2


void spi_begin(const struct spi *spi);
void spi_end(const struct spi *spi);
void spi_send(const struct spi *spi, uint8_t v);
void spi_begin_rx(const struct spi *spi);
uint8_t spi_recv(const struct spi *spi);
void spi_init(const struct spi *spi);

#endif /* !SPI_H */
