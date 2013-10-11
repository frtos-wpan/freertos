/*
 * newlib.c - Helper functions for newlib
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "FreeRTOS.h"

#include STM32_CONF_H

#include "console.h"


int _close(int fd)
{
	return 0;
}


int _fstat(int fd, struct stat *st)
{
	if (fd < 0 || fd > 2) {
		errno = EBADF;
		return -1;
	}

	/* according to stm32ldiscovery/newlib.c */
	st->st_mode = S_IFCHR;
	return 0;
}


int _isatty(int fd)
{
	if (fd >= 0 && fd <= 3)
		return 1;
	errno = EBADF;
	return 0;
}


int _lseek(int fd, off_t offset, int whence)
{
	return 0;
}


ssize_t _read(int fd, void *buf, size_t count)
{
	errno = EBADF;
	return -1;
}


caddr_t _sbrk(int incr)
{
	return (caddr_t) pvPortMalloc(incr);
}


ssize_t _write(int fd, const void *buf, size_t count)
{
	ssize_t ret = count;

	if (fd < 0 || fd > 2) {
		errno = EBADF;
		return -1;
	}

	while (count--) {
		char c = *(const char *) buf++;

		if (c == '\n')	
			console_send("\r", 1);
		console_send(&c, 1);
	}
	return ret;
}
