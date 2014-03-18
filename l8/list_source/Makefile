#定义变量
#DO NOT use arm-none-linux-gnueabi
OBJ = main.bin
GCC = arm-elf-
CC = $(GCC)gcc
LD = $(GCC)ld
CFLAG = -nostdlib -fno-builtin
LFLAG = -Tlink.lds -nostdlib
PLATFLAG = -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc
OBJCOPY = $(GCC)objcopy
OBJDUMP = $(GCC)objdump
AFLAG =

all : $(OBJ)

#目标 : 依赖
#[tab]命令
main.bin:main.elf
	$(OBJCOPY) -O binary $< $@
	$(OBJDUMP) -D main.elf > main.dis

main.elf:main.o start.o printf.o string.o entry.o mmu.o exception.o swi.o int_control.o \
 timer.o
	$(LD) $(LFLAG) $^ $(PLATFLAG) -Map main.map -o $@

%.o : %.c
	$(CC) $(CFLAG) -c -o $@ $<

%.o : %.s
	$(CC) $(AFLAG) -c -o $@ $<



clean:
	rm -f *.o *.bin *.elf *.dis
