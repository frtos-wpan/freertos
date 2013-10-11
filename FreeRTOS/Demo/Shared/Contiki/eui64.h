/*
 * eui64.c - Provide the node's EUI64 (pre-configured or generated from UI_D)
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

#ifndef EUI64_H
#define	EUI64_H

#include <stdint.h>


extern uint8_t eui64[8];


void platform_eui64(uint8_t *eui);

void init_eui64(void);

#endif /* !EUI64_H */
