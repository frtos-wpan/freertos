/*
 * rf230-hal.c - Substitute for cpu/avr/radio/rf230bb/halbb.c
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include STM32_CONF_H

/* @@@ contiki/contiki/ because we're on top of contiki-outoftree */
#include "contiki/contiki/cpu/avr/radio/rf230bb/at86rf230_registermap.h"
#include "contiki/hal.h"

#include "contiki.h"

#include "platform.h"
#include "gpio.h"
#include "extint.h"
#include "spi.h"


static struct spi spi;
static unsigned slp_tr;


/* ----- Transceiver commands and bits ------------------------------------- */


#define	AT86RF230_REG_READ	0x80
#define	AT86RF230_REG_WRITE	0xc0
#define	AT86RF230_BUF_READ	0x20
#define	AT86RF230_BUF_WRITE	0x60

#define	IRQ_TRX_END		0x08


/* ----- Items shared with rf230bb ----------------------------------------- */


extern hal_rx_frame_t rxframe[RF230_CONF_RX_BUFFERS];
extern uint8_t rxframe_head, rxframe_tail;


/* ----- Control signals --------------------------------------------------- */


void hal_set_rst_low(void)
{
	/* not supported by hardware */
	hal_register_read(RG_IRQ_STATUS);
}


void hal_set_rst_high(void)
{
	/* not supported by hardware */
}


void hal_set_slptr_high(void)
{
	gpio_set(slp_tr);
}


void hal_set_slptr_low(void)
{
	gpio_clr(slp_tr);
}


bool hal_get_slptr(void)
{
	return gpio_read(slp_tr);
}


/* ----- Register access --------------------------------------------------- */


static uint8_t register_read_unsafe(uint8_t address)
{
	uint8_t res;

	spi_begin(&spi);
	spi_send(&spi, AT86RF230_REG_READ | address);
	spi_begin_rx(&spi);
	res = spi_recv(&spi);
	spi_end(&spi);
	return res;
}


uint8_t hal_register_read(uint8_t address)
{
	uint8_t res;

	HAL_ENTER_CRITICAL_REGION();
	res = register_read_unsafe(address);
	HAL_LEAVE_CRITICAL_REGION();
	return res;
}


static void register_write_unsafe(uint8_t address, uint8_t value)
{
	spi_begin(&spi);
	spi_send(&spi, AT86RF230_REG_WRITE | address);
	spi_send(&spi, value);
	spi_end(&spi);
}


void hal_register_write(uint8_t address, uint8_t value)
{
	HAL_ENTER_CRITICAL_REGION();
	register_write_unsafe(address, value);
	HAL_LEAVE_CRITICAL_REGION();
}


static uint8_t subregister_read_unsafe(uint8_t address, uint8_t mask,
    uint8_t position)
{
	return (register_read_unsafe(address) & mask) >> position;
}


uint8_t hal_subregister_read(uint8_t address, uint8_t mask, uint8_t position)
{
	return (hal_register_read(address) & mask) >> position;
}


void hal_subregister_write(uint8_t address, uint8_t mask, uint8_t position,
    uint8_t value)
{
	uint8_t reg;

	HAL_ENTER_CRITICAL_REGION();
	reg = register_read_unsafe(address);
	reg = (reg & ~mask) | (value << position);
	hal_register_write(address, reg);
	HAL_LEAVE_CRITICAL_REGION();
}


/* ----- Buffer access ----------------------------------------------------- */


/*
 * Note that frame_read_unsafe can block for up to about 500 us.
 *
 * 131 Bytes * 3.6 us/Byte = 472 us
 *
 * The 3.6 us/Byte rate was obtained by measurement, see
 * frtos-wpan/lab/atben-spi/spi-buf-read.png
 */

static void frame_read_unsafe(hal_rx_frame_t *rx_frame)
{
	uint8_t *buf = rx_frame->data;
	uint8_t i;

	spi_begin(&spi);
	spi_send(&spi, AT86RF230_BUF_READ);
	spi_begin_rx(&spi);
	rx_frame->length = spi_recv(&spi);
	if (rx_frame->length > HAL_MAX_FRAME_LENGTH)
		rx_frame->length = HAL_MAX_FRAME_LENGTH;
	for (i = 0; i != rx_frame->length; i++)
		*(uint8_t *) buf++ = spi_recv(&spi);
	rx_frame->lqi = spi_recv(&spi);
        spi_end(&spi);
	rx_frame->crc = true;	/* checked by hardware */
}


void hal_frame_read(hal_rx_frame_t *rx_frame)
{
	HAL_ENTER_CRITICAL_REGION();
	frame_read_unsafe(rx_frame);
	HAL_LEAVE_CRITICAL_REGION();
}


/*
 * Note that hal_frame_write can block for up to about 200 us:
 *
 * 130 Bytes * 8 / 5.25 Mbps = 198 us
 */

void hal_frame_write(uint8_t *write_buffer, uint8_t length)
{
	HAL_ENTER_CRITICAL_REGION();
	spi_begin(&spi);
	spi_send(&spi, AT86RF230_BUF_WRITE);
	spi_send(&spi, length);
	while (length--)
		spi_send(&spi, *(uint8_t *) write_buffer++);
	spi_end(&spi);
	HAL_LEAVE_CRITICAL_REGION();
}


void hal_sram_read(uint8_t address, uint8_t length, uint8_t *data)
{
	/* not used */
}


void hal_sram_write(uint8_t address, uint8_t length, uint8_t *data)
{
	/* not used */
}


/* ----- Interrupts -------------------------------------------------------- */


void hal_enable_trx_interrupt(void)
{
	EXTINT_ENABLE(IRQ);
}


void hal_disable_trx_interrupt(void)
{
	EXTINT_DISABLE(IRQ);
}


IRQ_HANDLER(IRQ)
{
	uint8_t irq, state;

	EXTI_ClearITPendingBit(1 << BIT_IRQ);
	irq = register_read_unsafe(RG_IRQ_STATUS);

	if (!(irq & IRQ_TRX_END))
		return;

	/* @@@ record RSSI ? */
	/* @@@ check power level ? */
	/* @@@ make BAT_LOW one-shot ? */

	state = subregister_read_unsafe(SR_TRX_STATUS);
	if (state == BUSY_RX_AACK || state == RX_ON || state == BUSY_RX ||
	     state == RX_AACK_ON) {
		frame_read_unsafe(&rxframe[rxframe_tail]);
		rxframe_tail++;
		if (rxframe_tail >= RF230_CONF_RX_BUFFERS)
			rxframe_tail = 0;
		rf230_interrupt();
	}
}


/* ----- Critical sections (general) --------------------------------------- */



void HAL_ENTER_CRITICAL_REGION(void)
{
	taskENTER_CRITICAL();
}


void HAL_LEAVE_CRITICAL_REGION(void)
{
	taskEXIT_CRITICAL();
}


/* ----- Initialization ---------------------------------------------------- */


void hal_init(void)
{
	spi = SPI_DEV_INIT;
	spi_init(&spi);

	slp_tr = GPIO_ENABLE(SLP_TR);
	gpio_clr(slp_tr);
	gpio_output(slp_tr);

	hal_register_read(RG_IRQ_STATUS);
	EXTINT_SETUP(IRQ);

	hal_enable_trx_interrupt();

	/*
	 * rf230bb.c will force the transceiver into TRX_OFF, so we're probably
	 * good despite not being able to do a proper hardware reset.
	 */
}


void hal_test(void)
{
	uint8_t pn, vn;
	uint8_t m0, m1;

	hal_init();
	pn = hal_register_read(RG_PART_NUM);
	vn = hal_register_read(RG_VERSION_NUM);
	m0 = hal_register_read(RG_MAN_ID_0);
	m1 = hal_register_read(RG_MAN_ID_1);
	printf("part 0x%02x revision 0x%02x manufacturer xxxx%02x%02x\n",
	    pn, vn, m1, m0);
}
