/*
 * pll.c - Generate PLL settings for STM32Fxxx CPUs
 *
 * Developed by Werner Almesberger for Actility S.A., and
 * licensed under LGPLv2 by Actility S.A.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>


#define	MHz	* 1e6


static struct in {
	unsigned long src;	/* source clock, in Hz */
	unsigned m_min, m_max;	/* range of PLLM */
	unsigned n_min, n_max;	/* range of PLLN */
	unsigned p_min, p_max;	/* range of PLLP */
	unsigned q_min, q_max;	/* range of PLLQ */
	unsigned sys_min;	/* minimum system clock; 0 if no minimum */
	unsigned sys_max;	/* maximum system clock; 0 if no maximum */
} in;

static struct out {
	unsigned m, n, p, q;	/* PLL configuration parameters */
	double vco_in;		/* VCO input frequency, Hz */
	double vco_out;		/* VCO output frequency, Hz */
	double sys;		/* System clock, Hz */
	double usb;		/* USB/SDIO/RNG clock, Hz */
} out, best;

static bool found = 0;
static bool need_usb = 0;
static bool (*valid)(void);


/* ----- STM32F2/F4 common ------------------------------------------------- */


static bool valid_common(void)
{
	if (out.usb > 48 MHz)
		return 0;
	if (out.vco_in < 1 MHz || out.vco_in > 2 MHz)
		return 0;

	/* USB 2.0, 7.1.11 "Data Signaling Rate", Full-Speed +/- 0.25% */

	if (need_usb && fabs(1.0 - out.usb / (48 MHz)) > 0.0025)
		return 0;

	if (in.sys_min && out.sys < in.sys_min)
		return 0;
	if (in.sys_max && out.sys > in.sys_max)
		return 0;

	return 1;
}


static void stm32_setup(void)
{
	in.m_min = 2;	/* M in [2, 63] */
	in.m_max = 63;
	in.p_min = 0;	/* P in [0, 3] */
	in.p_max = 3;
	in.q_min = 2;	/* Q in [2, 15] */
	in.q_max = 15;
}


/* ----- STM32F205xx, STM32F207xx, STM32F215xx, STM32F217xx ---------------- */


static bool f2_valid(void)
{
	if (out.vco_out < 64 MHz || out.vco_out > 432 MHz)
		return 0;
	if (out.sys > 120 MHz)
		return 0;
	return valid_common();
}


static void f2_setup(void)
{
	stm32_setup();

	in.n_min = 64;	/* N in [64, 432] */
	in.n_max = 432;
	
	valid = f2_valid;
}


/* ----- STM32F40xxx, STM32F41xxx, STM32F42xxx, STM32F43xxx ---------------- */


static bool f4_valid(void)
{
	if (out.vco_out < 192 MHz || out.vco_out > 432 MHz)
		return 0;
	if (out.sys > 168 MHz)
		return 0;
	return valid_common();
}


static void f4_setup(void)
{
	stm32_setup();

	in.n_min = 192;	/* N in [192, 432] */
	in.n_max = 432;
	
	valid = f4_valid;
}


/* ----- Parameter search loop --------------------------------------------- */


static bool better(void)
{
	if (!found)
		return 1;

	/*
	 * The manual recommends to use VCO_in = 2 MHz, i.e., the highest
	 * permissible frequency. We interpret this as "higher is better".
	 */
	if (out.vco_in > best.vco_in)
		return 1;

	/*
	 * All else being equal, we go for the highest system clock frequency.
	 */
	if (out.sys > best.sys)
		return 1;

	return 0;
}



static void calc(void)
{
	out.vco_in = (double) in.src / out.m;
	out.vco_out = out.vco_in * out.n;
	out.sys = out.vco_out / 2 / (out.p + 1);
	out.usb = out.vco_out / out.q;
}


#define	FOR_RANGE(v)	for (out.v = in.v##_min; out.v <= in.v##_max; out.v++)


static void loop(void)
{
	FOR_RANGE(m)
		FOR_RANGE(n)
			FOR_RANGE(p)
				FOR_RANGE(q) {
					calc();
					if (!valid())
						continue;
					if (!better())
						continue;
					best = out;
					found = 1;
				}
}


/* ----- Command-line processing ------------------------------------------- */


static void usage(const char *name)
{
	fprintf(stderr,
	    "usage: %s [-u] family src_MHz [min_MHz [max_MHz]]\n\n"
	    "  -u       ensure that FS USB clock is 48 MHz +/- 0.25%%\n"
	    "  family   chip family (f2 or f4)\n"
	    "  src_MHz  clock source frequency (crystal or 16 for the HSI)\n"
	    "  min_MHz  optional minimum system clock frequency\n"
	    "  max_MHz  optional maximum system clock frequency\n"
	    , name);
	exit(1);
}


static void setup(const char *family)
{
	if (!strcmp(family, "f2"))
		f2_setup();
	else if (!strcmp(family, "f4"))
		f4_setup();
	else {
		fprintf(stderr, "unknown chip family \"%s\"\n", family);
		exit(1);
	}
}


int main(int argc, char **argv)
{
	int c;

	while ((c = getopt(argc, argv, "u")) != EOF)
		switch (c) {
		case 'u':
			need_usb = 1;
			break;
		default:
			usage(*argv);
		}

	switch (argc-optind) {
	case 4:
		in.sys_max = atof(argv[optind+3]) MHz;
		if (in.sys_max <= 0)
			usage(*argv);
		/* fall through */
	case 3:
		in.sys_min = atof(argv[optind+2]) MHz;
		if (in.sys_min < 0)
			usage(*argv);
		/* fall through */
	case 2:
		in.src = atof(argv[optind+1]) MHz;
		if (in.src <= 0)
			usage(*argv);
		setup(argv[optind]);
		break;
	default:
		usage(*argv);
	}

	loop();

	if (!found) {
		fprintf(stderr, "no solution found\n");
		exit(1);
	}

	/*
	 * Output format is  VAR=value;  suitable for use in Bourne shell
	 * "eval" and - to some extent - also in C.
	 *
	 * In order to be compatible with many languages, we don't output
	 * comments to indicate units or to provide further information about
	 * the values or their origin.
	 */

	printf("SRC=%.3f;\n", in.src / 1e6);	/* clock source, MHz */
	printf("PLLM=%u;\n", best.m);		/* PLLM field */
	printf("PLLN=%u;\n", best.n);		/* PLLN field */
	/*
	 * PLLP is a little weird, because the field in RCC_PLLCFGR is named
	 * "PLLP" but the value of the PLLP parameter is not the same as the
	 * value of the field. We therefore use "PLLP" for the parameter and
	 * "P_field" for the actual field value.
	 */
	printf("P_field=%u;\n", best.p);	/* PLLP field */
	printf("PLLP=%u;\n", 2 * (best.p + 1));	/* PLLP parameter */
	printf("PLLQ=%u;\n", best.q);		/* PLLQ field */
	printf("VCO_in=%.3f;\n", best.vco_in / 1e6);   /* VCO input, MHz */
	printf("VCO_out=%.3f;\n", best.vco_out / 1e6); /* VCO output, MHz */
	printf("SYS=%.3f;\n", best.sys / 1e6);	/* System clock, MHz */
	printf("USB=%.3f;\n", best.usb / 1e6);	/* USB/SDIO/RNG clock, MHz */

	return 0;
}
