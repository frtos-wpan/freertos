#
# E407/Makefile - Build FreeRTOS for Olimex STM32-E407
#
# Developed by Werner Almesberger for Actility S.A., and
# licensed under LGPLv2 by Actility S.A.
#

NAME = demo
CHIP_FAMILY = STM32F4xx
CRYSTAL_Hz = 12000000
PLL_ARGS = -u 12

#
# -DUSE_STDPERIPH_DRIVER: make stm32f4xx_misc.c include stm32f4xx_conf.h
#

CFLAGS = -Wall -Os -g \
	-DUSE_STDPERIPH_DRIVER \
	-I. \
	-I../../Source/include \
	-I../Common/include \
	-I../Shared/newlib

CFLAGS += -DFREERTOS_BUILD_DATE="\"`date`\""

DRIVERS = exti gpio misc rcc spi syscfg usart

SRC = $(NAME).c ../Shared/newlib/newlib.c console.c \
	$(DRIVERS_C) \
	../Common/Minimal/flash.c \
	../../Source/list.c \
	../../Source/queue.c \
	../../Source/tasks.c \
	../../Source/timers.c \
	../../Source/portable/MemMang/heap_1.c

include ../Shared/STM32/Makefile.stm32

$(DRIVERS_C):	$(DEMOBUILDER)

#
# Adapt compiler name and increase configMINIMAL_STACK_SIZE from 130 to
# 1300 bytes.
#

FreeRTOSConfig.h: ../CORTEX_M4F_STM32F407ZG-SK/FreeRTOSConfig.h \
		  ../Shared/newlib/mkconfig
		$(BUILD) ../Shared/newlib/mkconfig -s 1300 $< >$@ || \
		    { rm -f $@; exit 1; }

clean::
		rm -f FreeRTOSConfig.h

#####  Contiki  ###############################################################

#
# Hardware layout options:
#
# CFLAGS	HAL		SPI		Description
# -------------	---------------	---------------	-----------------------------------------------
# (none)	rf230-hal	spi-gpio 	Olimex STM32-E407, atben in uSD slot
# -DODEV	rf230-hal	spi-gpio 	Olimex STM32-E407, atben on ODEV, bit-banging
# -DODEV	rf230-hal	spi-stm32	Olimex STM32-E407, atben on ODEV, SPI device
#

CFLAGS += -DODEV
HAL = rf230-hal
SPI = spi-stm32

EUI64 = 0xa0,0,0,0,0,0,0,3
RF_CHANNEL = 11
PAN_ADDR = 0xabcd
SHORT_ADDR = 0x8003

include ../Shared/Contiki/Makefile.contiki

#####  Final dependencies  ####################################################

#
# Since things get added to OBJS until the end of the Makefile, we need to put
# these dependencies last.
#

$(OBJS):	FreeRTOSConfig.h
