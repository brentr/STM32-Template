TEMPLATEROOT = ../..

# compilation flags for gdb

CFLAGS  = -O0 -g -DUSE_STM32L152D_EVAL -I../common
ASFLAGS = -g 

# object files

vpath %.c ../common

OBJS=  $(STARTUP) stm32l1xx_it.o stm32l152d_eval.o main.o
OBJS+= stm32l1xx_rcc.o stm32l1xx_gpio.o misc.o stm32l1xx_i2c.o 

# include common make file

include $(TEMPLATEROOT)/Makefile.common


