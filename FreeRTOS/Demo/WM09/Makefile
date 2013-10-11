#
# WM09/Makefile - Build FreeRTOS for USI WM-N-BM-09 EVB
#
# Developed by Werner Almesberger for Actility S.A., and
# licensed under LGPLv2 by Actility S.A.
#

NAME = demo
CHIP_FAMILY = STM32F2xx
CRYSTAL_Hz = 26000000
PLL_ARGS = 26

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
# HAL		SPI		Description
# -------------	---------------	-----------------------------------------------
# rf230-hal	spi-stm32	USI WM-N-BM-09 EVB, atben on DEV9, SPI device
# rf230-hal	spi-gpio	USI WM-N-BM-09 EVB, atben on DEV9, bit-banging
#

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


#####  Flash via OpenOCD ######################################################

#
# Use BRCM's OpenOCD binary which has the BCM9WCD1EVAL1 layout and some other
# enhancements.
#
WICED = ../../../../wiced
OPENOCD = $(WICED)/Tools/OpenOCD/Linux64/openocd-all-brcm-libftdi
WICED_OPENOCD = $(WICED)/Tools/OpenOCD

.PHONY:		flash run

flash:		$(NAME).elf
		$(OPENOCD) \
		    -f $(WICED_OPENOCD)/BCM9WCD1EVAL1.cfg \
		    -f $(WICED_OPENOCD)/stm32f2x.cfg \
		    -f $(WICED_OPENOCD)/stm32f2x-flash-app.cfg \
		    -c "flash write_image erase $<" \
		    -c shutdown

run:
		$(OPENOCD) \
		    -f $(WICED_OPENOCD)/BCM9WCD1EVAL1.cfg \
		    -f $(WICED_OPENOCD)/stm32f2x.cfg \
		    -c init \
		    -c "reset run" \
		    -c shutdown
