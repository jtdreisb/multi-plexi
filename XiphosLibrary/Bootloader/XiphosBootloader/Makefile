# Makefile for the AVRProg-compatible Bootloader based on the 
# WinAVR Sample makefile written by Eric B. Weddington, Jörg Wunsch, et al.
# Released to the Public Domain
# Please read the make user manual!
#
# Additional material for this makefile was submitted by:
#  Tim Henigan
#  Peter Fleury
#  Reiner Patommel
#  Sander Pool
#  Frederik Rouleau
#  Markus Pfaff
#
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make program = Download the hex file to the device, using avrdude.  Please
#                customize the avrdude settings below first!
#
# make filename.s = Just compile filename.c into the assembler code only
#
# To rebuild project do "make clean" then "make all".
#

# user defined values

# MCU name
MCU = atmega1281

################## BOOTLOADER ######################
#  mt: Boot loader support. So far not done with a separate section
#  to get the interrupt vector into the bootloader area (for BOOTINTVEC=yes).
#  Bootloader address in datasheet and stk500 is given as
#  "word", gcc toolchain needs "byte"-address
#  (see LDFLAGS further down)

#/* Select Boot Size in Words (select one, comment out the others) */
## NO! BOOTSIZE=128
## NO! BOOTSIZE=256
## BOOTSIZE=512
## BOOTSIZE=1024
## BOOTSIZE=2048
BOOTSIZE=4096

# /* Select if bootloader should include the interrupt-vectors 
# when selecting 'no' here, the bootloader must not use
# any interrupts and the modified linker-scripts are used. */
##BOOTINTVEC=yes
BOOTINTVEC=no

##
ifeq ($(MCU), atmega32)
ifeq ($(BOOTSIZE), 256)
	MT_BOOTLOADER_ADDRESS = 0x7E00
endif
ifeq ($(BOOTSIZE), 512)
	MT_BOOTLOADER_ADDRESS = 0x7C00
endif
ifeq ($(BOOTSIZE), 1024)
	MT_BOOTLOADER_ADDRESS = 0x7800
endif
ifeq ($(BOOTSIZE), 2048)
	MT_BOOTLOADER_ADDRESS = 0x7000
endif
endif

##
ifeq ($(MCU), atmega64)
ifeq ($(BOOTSIZE), 512)
	MT_BOOTLOADER_ADDRESS = 0xFC00
endif
ifeq ($(BOOTSIZE), 1024)
	MT_BOOTLOADER_ADDRESS = 0xF800
endif
ifeq ($(BOOTSIZE), 2048)
	MT_BOOTLOADER_ADDRESS = 0xF000
endif
ifeq ($(BOOTSIZE), 4096)
	MT_BOOTLOADER_ADDRESS = 0xE000
endif
endif

##
ifeq ($(MCU), atmega644)
ifeq ($(BOOTSIZE), 512)
	MT_BOOTLOADER_ADDRESS = 0xFC00
endif
ifeq ($(BOOTSIZE), 1024)
	MT_BOOTLOADER_ADDRESS = 0xF800
endif
ifeq ($(BOOTSIZE), 2048)
	MT_BOOTLOADER_ADDRESS = 0xF000
endif
ifeq ($(BOOTSIZE), 4096)
	MT_BOOTLOADER_ADDRESS = 0xE000
endif
endif

##
ifeq ($(MCU), atmega644p)
ifeq ($(BOOTSIZE), 512)
	MT_BOOTLOADER_ADDRESS = 0xFC00
endif
ifeq ($(BOOTSIZE), 1024)
	MT_BOOTLOADER_ADDRESS = 0xF800
endif
ifeq ($(BOOTSIZE), 2048)
	MT_BOOTLOADER_ADDRESS = 0xF000
endif
ifeq ($(BOOTSIZE), 4096)
	MT_BOOTLOADER_ADDRESS = 0xE000
endif
endif

##
ifeq ($(MCU), atmega128)
ifeq ($(BOOTSIZE), 512)
	MT_BOOTLOADER_ADDRESS = 0x1FC00
endif
ifeq ($(BOOTSIZE), 1024)
	MT_BOOTLOADER_ADDRESS = 0x1F800
endif
ifeq ($(BOOTSIZE), 2048)
	MT_BOOTLOADER_ADDRESS = 0x1F000
endif
ifeq ($(BOOTSIZE), 4096)
	MT_BOOTLOADER_ADDRESS = 0x1E000
endif
endif

##
ifeq ($(MCU), atmega1281)
ifeq ($(BOOTSIZE), 512)
	MT_BOOTLOADER_ADDRESS = 0x1FC00
endif
ifeq ($(BOOTSIZE), 1024)
	MT_BOOTLOADER_ADDRESS = 0x1F800
endif
ifeq ($(BOOTSIZE), 2048)
	MT_BOOTLOADER_ADDRESS = 0x1F000
endif
ifeq ($(BOOTSIZE), 4096)
	MT_BOOTLOADER_ADDRESS = 0x1E000
endif
endif


# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Target file name (without extension).
TARGET = main

# List C source files here. (C dependencies are automatically generated.)
SRC = lcd.c $(TARGET).c

# Optimization level, can be [0, 1, 2, 3, s].
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here
CDEFS = -DBOOTSIZE=$(BOOTSIZE)

# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:.c=.lst)
CFLAGS += $(CSTANDARD)


# Assembler flags.
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information
ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs


# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref -lm


################## BOOTLOADER ######################
# MT_BOOTLOADER_ADDRESS (=Start of Boot Loader section
# in bytes - not words) as defined above.
LDFLAGS += -Wl,--section-start=.text=$(MT_BOOTLOADER_ADDRESS)

# check if linker-scripts without interrupt-vectors should
# be used and set linker-option, announce to C-code by define
ifeq ($(BOOTINTVEC), no)
LDFLAGS += -T./avr5.x
CFLAGS  += -DBOOTLOADERHASNOVECTORS
endif


# Programming support using avrdude. Settings and variables.

# Programmer. To see all valid options, type: avrdude -c ?
AVRDUDE_PROGRAMMER = avrispmkII

AVRDUDE_PORT = usb

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)


# ---------------------------------------------------------------------------

# Define programs and commands.
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f


# Define Messages
# English
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_FLASH = Creating load file for Flash:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:




# Define all object files.
OBJ = $(SRC:.c=.o)

# Define all listing files.
LST = $(SRC:.c=.lst)


# Compiler flags to generate dependency files.
GENDEPFLAGS = -MD -MP -MF $(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)





# Default target.
all: sizebefore build sizeafter

build: elf hex lss sym

elf: $(TARGET).elf
hex: $(TARGET).hex
lss: $(TARGET).lss
sym: $(TARGET).sym


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -x -A $(TARGET).elf
sizebefore:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); echo; fi

sizeafter:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); echo; fi


# Program the device.
program: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

# Set the fuses
# DON'T MODIFY THESE LINES UNLESS YOU *REALLY* KNOW WHAT YOU ARE DOING!
# YOU CAN PERMANENTLY LOSE THE ABILITY TO REPROGRAM YOUR CHIP!!!
fuses:
	$(AVRDUDE) -v -u $(AVRDUDE_FLAGS) -U efuse:w:0xFD:m -U hfuse:w:0x90:m -U lfuse:w:0xDF:m

# This just shows a ton of info, with the last few lines indicating the fuses.
readfuses:
	$(AVRDUDE) -v $(AVRDUDE_FLAGS)


# Create final output file (.hex) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@



# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@


# Compile: create assembler files from C source files.
%.s : %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
%.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@



# Target: clean project.
clean:
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).cof
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).a90
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lnk
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(OBJ)
	$(REMOVE) $(LST)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) $(SRC:.c=.o.d)


# Include the dependency files.
-include $(wildcard *.d)


# Listing of phony targets.
.PHONY : all sizebefore sizeafter \
build elf hex lss sym \
clean program
