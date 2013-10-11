/*
 * console.h - Serial console
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

/*
 * The serial console functions have to be implemented by the respective
 * platform code, but since newlib calls these functions we need to have the 
 * declarations where newlib can see them, i.e., here.
 */

#ifndef CONSOLE_H
#define	CONSOLE_H


/*
 * Receive handler, to be set by subsystem/application processing serial input.
 * The handker may be invoked from an interrupt handler and/or with interrupts
 * disabled.
 */

extern void (*console_recv)(const char *buf, unsigned n);


/*
 * Initialize the serial console (UART). Must be run before calling any of the
 * other console_* functions.
 */

void console_init(void);

/*
 * Output characters on the serial console. This function is safe to run from
 * an * interrupt handler and/or with interrupts disabled, but may loop for a
 * while.
 */

void console_send_isr(const char *buf, unsigned len);

/*
 * Like console_send_isr, but not safe to call from an interrupt handler or
 * with interrupts disabled. May defer the actual output and thus return faster
 * than an equivalent call to console_send_isr.
 */

void console_send(const char *buf, unsigned len);

#endif /* !CONSOLE_H */
