##Copyright (C) 2009-2010  Patrick J. McCarty.
##Licensed under X11 License. See LICENSE.txt for details.

# Specify the port name for your In-System Programmer (ISP) device or cable here.
#  For a FTDI USB-to-UART converter on Windows this is something like COM2 (check in the Windows Device Manager under Ports for the number)
#    In XP: Right-click My Computer, click "Properties" then navigate to the "Hardware" tab followed by clicking "Device Manager".
#    Scroll down to "Ports" and click the "+" to the left of the name. Finally read the "COM" number next to the name "USB Serial Port (COMx)".
#  For a FTDI USB-to-UART converter on Linux this is something like /dev/ttyUSB0 (for a listing of USB serial ports, run: ls /dev/ttyUSB* ).
#  For a cheap SparkFun AVR-PG1B serial programmer on Windows this is typically COM1.
#    You can check using steps similar to above, but looking at the entry named "Communications Port (COMx)".
#  For a cheap SparkFun AVR-PG1B serial programmer on Linux this is something like /dev/ttyS0
#  For a fancy Atmel AVRISP mkII In-System Programmer simply enter usb (lowercase is important).
#    On Windows, you may need to install the LibUSB drivers included with WinAVR at: <WinAVR folder>\utils\libusb\bin\avrisp2.inf
#  For a cheapo PonyProg or Futurlec style parallel port programmer enter lpt1 (lowercase is important).
#PORT = COM7 #blue
PORT = COM4 #red
#/dev/tty.usbserial-A7005hpH

# Specify the type of In-System Programmer (ISP) device or cable you are using.
#  For the onboard FTDI USB-to-UART converter (used with a butterfly bootloader) use butterfly -b 57600
#  For a cheap SparkFun AVR-PG1B serial programmer use ponyser
#  For a fancy Atmel AVRISP mkII In-System Programmer use avrispmkII
#  For a cheapo PonyProg or Futurlec style parallel port programmer use pony-stk200
#  Avrdude supports many more types of programmers. For a complete list of valid options, open a commandline and run: avrdude -c ?
#  You can see the hardware pinouts used for the various programmers and even define your own programmer in avrdude's config file at:
#    <WinAVR folder>\bin\avrdude.conf
ISP = butterfly -b 57600 
#ISP = buspirate -b 57600

# Enter the target microcontroller model.
#  For Xiphos 1.0, this should always be atmega1281 unless you substituted a different microcontroller chip model.
#  For a complete list of models supported by avrdude, run: avrdude -p ?
MCU = atmega1281


# Determine which library files to compile and #defines to create based on variables set in the project Makefile.
FILES += $(LIB)/utility.c

ifeq ($(USE_LCD), 1)
	FILES += $(LIB)/LCD.c
	DEFINES += -D USE_LCD=1
endif

ifeq ($(USE_ADC), 1)
	FILES += $(LIB)/ADC.c
	DEFINES += -D USE_ADC=1
endif

USE_MOTORS = 0
ifeq ($(USE_MOTOR0), 1)
	USE_MOTORS = 1
	DEFINES += -D USE_MOTOR0=1
endif
ifeq ($(USE_MOTOR1), 1)
	USE_MOTORS = 1
	DEFINES += -D USE_MOTOR1=1
endif
ifeq ($(USE_MOTORS), 1)
	FILES += $(LIB)/motors.c
endif

ifneq ($(NUM_SERVOS), 0)
	FILES += $(LIB)/servos.c
	DEFINES += -D NUM_SERVOS=$(NUM_SERVOS)
endif

ifeq ($(USE_I2C), 1)
	FILES += $(LIB)/I2C.c
	DEFINES += -D USE_I2C=1
endif


# Makefile Targets

# Since the "all" target is the first target in the file, it will run when you simply type make (or make all).
#  We have configured this target so that it only compiles the program into .elf and .hex files, and displays their final sizes.

#for a map, add: -Wl,-Map,$(PROJECTNAME).map
#for a lst, add: -Wa,-adhlms=$(PROJECTNAME).lst
#tried unsuccessfully to remove unused code with: -Wl,-static -ffunction-sections -fdata-sections
#the -g is required to get C code interspersed in the disassembly listing
all:
	avr-gcc -g -mmcu=$(MCU) $(DEFINES) -I . -I $(LIB) -Os -Werror -mcall-prologues -o $(PROJECTNAME).elf $(FILES) -lm
	avr-objcopy -O ihex $(PROJECTNAME).elf $(PROJECTNAME).hex
	avr-size $(PROJECTNAME).elf

# This target first executes the "all" target to compile your code, and then programs the hex file into the ATmega using avrdude.
program: all
	avrdude -p $(MCU) -P $(PORT) -c $(ISP) -u -U flash:w:$(PROJECTNAME).hex

# This target can be called to delete any existing compiled files (binaries), so you know that your next compile is fresh.
# The dash in front of rm is not passed to the shell, and just tells make to keep running if an error code is returned by rm.
clean:
	-rm -f $(PROJECTNAME).elf $(PROJECTNAME).hex $(PROJECTNAME).lss

# This target generates a .lss extended listing file from your compiled .elf file, and prints info on the sections.
# The file shows you the assembly code with your original C code interspersed between it to help you make sense of the assembly.
# This allows you to inspect the actual AVR instructions that your code will be running, which is useful to compare
# two similar implementations and pick the more optimized version, or to verify correctness of an operation.
asm: all
	avr-objdump -h -S $(PROJECTNAME).elf > $(PROJECTNAME).lss
