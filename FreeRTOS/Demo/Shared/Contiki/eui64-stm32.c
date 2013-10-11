/*
 * eui64-stm32.c - Generate EUI64 from UI_D (STM32)
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "md5/md5.h"
#include "eui64.h"


/*
 * Since we can't make any assumptions about the structure of U_ID, e.g., how
 * many bits differ from chip to chip or where these bits are located, we hash
 * all 96 bits with a reasonably good hash algorithm.
 *
 * The hash algorithm doesn't have to be "secure" (i.e., computationally
 * expensive to reverse), just shuffle the bits well. MD5 is therefore a good
 * enough choice for our purpose, even though its use is deprecated for
 * security-related applications.
 */

/*
 * To verify the use of MD5, if the output is, say
 *
 * U_ID 0x00280023 0x32314718 0x39313739
 *
 * then
 *
 * % echo -e -n '\x23\x00\x28\x00\x18\x47\x31\x32\x39\x37\x31\x39' | md5sum
 *
 * would yield the MD5 digest 899d2cc0a49e8492d1423cbf0b135b1d
 * which becomes the EUI64 8a:9d:2c:c0:a4:9e:84:92
 */

void platform_eui64(uint8_t *eui)
{
	uint32_t *u_id = (void *) 0x1fff7a10;
	MD5_CTX md5;
	unsigned char digest[16];

	printf("U_ID 0x%08x 0x%08x 0x%08x\n",
	    (unsigned) u_id[0], (unsigned) u_id[1], (unsigned) u_id[2]);

	MD5_Init(&md5);
	MD5_Update(&md5, u_id, 12);
	MD5_Final(digest, &md5);

	/*
	 * We use a 64 bit substring from the hash (any substring will do),
	 * then zero the multicast bit (i.e., the address is unicast) and set
	 * the local bit (i.e., the address is not allocated under an OUI.)
	 */
	memcpy(eui64, digest, 8);
	eui64[0] &= 0xfe;	/* unicast */
	eui64[0] |= 2;		/* local */
}
