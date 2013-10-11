/*
 * eui64.c - Provide the node's EUI64 (pre-configured or generated from UI_D)
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "md5/md5.h"
#include "eui64.h"


#ifdef EUI64
uint8_t eui64[8] = { EUI64 };
#else
uint8_t eui64[8];
#endif


void init_eui64(void)
{
#ifndef EUI64
	platform_eui64(eui64);
#endif /* !EUI64 */

	printf("EUI64 %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
	    eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5],
	    eui64[6], eui64[7]);
}
