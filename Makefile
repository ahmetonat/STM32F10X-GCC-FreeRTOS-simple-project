
# FreeRTOS directory:
FreeRTOS=/home/onat/elektronik/ARM/FreeRTOSV8.2.1/FreeRTOS

# compilation flags for gcc

CFLAGS  = -O1 -g
CFLAGS  += -I$(FreeRTOS)/include -DGCC_ARMCM3
CFLAGS += -I$(FreeRTOS)/Source/include 
CFLAGS += -I$(FreeRTOS)/Source
CFLAGS += -I$(FreeRTOS)/Source/portable/GCC/ARM_CM3
CFLAGS += -I$(FreeRTOS)/Source/portable/MemMang
CFLAGS += -I$(FreeRTOS)

#ASFLAGS = -g 

vpath %.c $(FreeRTOS)/STM32F10x_StdPeriph_Driver
vpath %.c $(FreeRTOS)/Source/include
vpath %.h $(FreeRTOS)/Source/include
vpath %.c $(FreeRTOS)/Source
vpath %.c $(FreeRTOS)/Source/portable/MemMang
vpath %.h $(FreeRTOS)/Source/portable/GCC/ARM_CM3
vpath %.c $(FreeRTOS)/Source/portable/GCC/ARM_CM3

# object files
#OBJS=  freertos_test01.o 
#OBJS=  freertos_test02.o 
#OBJS=  freertos_test03.o 
#OBJS=  freertos_test04.o
OBJS=  freertos_test04a.o
OBJS += list.o 
OBJS += tasks.o 
OBJS += heap_1.o 
OBJS += port.o 
OBJS += misc.o 
OBJS += startup_stm32f10x.o system_stm32f10x.o 
OBJS += stm32f10x_rcc.o stm32f10x_gpio.o

# name of executable
ELF=$(notdir $(CURDIR)).elf
MAP_FILE=$(notdir $(CURDIR)).map  #AO!
BIN_FILE=$(notdir $(CURDIR)).bin  #AO!

# Tools
CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
OBJCOPY=arm-none-eabi-objcopy  #AO!

# Library path
LIBROOT=/home/onat/elektronik/ARM/Compiler/STM32F10x_StdPeriph_Lib_V3.5.0

# Code Paths
DEVICE=$(LIBROOT)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CORE=$(LIBROOT)/Libraries/CMSIS/CM3/CoreSupport
PERIPH=$(LIBROOT)/Libraries/STM32F10x_StdPeriph_Driver

# Search path for standard files

# Search path for perpheral library
vpath %.c $(CORE)
vpath %.c $(PERIPH)/src
vpath %.c $(DEVICE)

#  Processor specific
# PTYPE defines the Proc. clock in the:
#STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/Device
#Support/ST/STM32F10x/system_stm32f10x.c file which is eventually included
# For STM32F100:
#PTYPE = STM32F10X_MD_VL 
#LDSCRIPT = stm32f100.ld
# For STM32F103:
PTYPE = STM32F10X_MD
LDSCRIPT = stm32f103.ld 

# Compilation Flags
FULLASSERT = -DUSE_FULL_ASSERT 

#LDFLAGS+= -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 
LDFLAGS+= -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 -Wl,-Map=$(MAP_FILE)
CFLAGS+= -mcpu=cortex-m3 -mthumb 
CFLAGS+= -I$(DEVICE) -I$(CORE) -I$(PERIPH)/inc -I.
CFLAGS+= -D$(PTYPE) -DUSE_STDPERIPH_DRIVER $(FULLASSERT)

OBJCOPYFLAGS = -O binary

# Build executable 

$(BIN_FILE) : $(ELF)	#AO!
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@ 

$(ELF) : $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS) $(LDFLAGS_POST)


# compile and generate dependency info

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -MM $(CFLAGS) $< > $*.d

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(ELF) $(MAP_FILE) #AO!

debug: $(ELF)
	arm-none-eabi-gdb $(ELF)


# pull in dependencies 
-include $(OBJS:.o=.d)

