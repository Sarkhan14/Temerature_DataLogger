################################################################################
# Makefile for MyAVR Programm                                                  #
# Lab eight                                                                    #
################################################################################


# Options ######################################################################
PORT=/dev/ttyUSB0
PROGRAMMER=avr911
TARGET_CPU=atmega88pa
TARGET_PROCESSOR=m88p
CFLAGS=-Wall -Os -c -mmcu=$(TARGET_CPU)
LDFLAGS=-mmcu=$(TARGET_CPU)


# Program ######################################################################
all: program.hex
	echo program build complete

program: program.hex
	avrdude -p $(TARGET_PROCESSOR) -c $(PROGRAMMER) -P $(PORT) -U flash:w:program.hex:i

# Open GTK Terminal ############################################################
terminal:
	gtkterm -s 9600 -p $(PORT)


# Hex File #####################################################################
program.hex: program.elf
	avr-objcopy -O ihex program.elf program.hex

program.elf: program.o init.o lcd.o i2c_master.o
	avr-gcc $(LDFLAGS) program.o init.o lcd.o i2c_master.o -o program.elf



# Object Files #################################################################
init.o: init.c
	avr-gcc $(CFLAGS) init.c -o init.o

lcd.o: lcd.c
	avr-gcc $(CFLAGS) lcd.c -o lcd.o

i2c_master.o: i2c_master.c
	avr-gcc $(CFLAGS) i2c_master.c -o i2c_master.o

program.o: program.c
	avr-gcc $(CFLAGS) program.c -o program.o



# Clean ########################################################################
clean:
	rm -f *.o *.elf *.hex *~
