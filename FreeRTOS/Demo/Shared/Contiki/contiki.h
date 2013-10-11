/*
 * contiki.h - Interface to the Contiki subsystem
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

#ifndef CONTIKI_H
#define	CONTIKI_H


void contiki_main(void);
int serial_line_input_byte(unsigned char c);
void rf230_interrupt(void);

#endif /* !CONTIKI_H */
