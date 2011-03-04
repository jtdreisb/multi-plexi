# Enter the name of your main .c file here, without the .c extension. Avoid spaces or else surround the name with double quotes.
PROJECTNAME = MyProjectName


# Enter the name of the folder that contains the XiphosLibrary files.
# . means same folder, .. means parent folder, otherwise give a subfolder name like XiphosLibrary or a relative path like ../XiphosLibrary
LIB = ./XiphosLibrary


# Set these variables to specify which XiphosLibrary features your program requires.
# This affects which library files get compiled, as well as which functions are enabled.
USE_LCD    = 1
USE_ADC    = 0
USE_MOTOR0 = 1
USE_MOTOR1 = 1
NUM_SERVOS = 7
USE_I2C    = 0


# Specify any additional .c source files containing your program code.
FILES = \
		hbridge.c \
		movement.c \
		main.c \


# The rest of the makefile is pulled in from MasterMakefile.mk in the XiphosLibrary folder.
# This includes the PORT and ISP variables defining the programmer to be used, the MCU variable defining the target microcontroller model,
# and the following Makefile targets:
#   all      - compiles your code (creates .elf and .hex files).
#   program  - runs the all target to compile and then downloads hex file to board using avrdude.
#   clean    - deletes output files (.elf, .hex, and .lss).
#   asm      - generates a .lss extended listing file of assembly/C from your compiled .elf file, and prints info on the sections.
include $(LIB)/MasterMakefile.mk
