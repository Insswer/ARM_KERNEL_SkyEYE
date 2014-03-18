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
	$(OBJDUMP) -d main.elf > main.dis

main.elf:main.o start.o printf.o string.o entry.o mmu.o
	$(LD) $(LFLAG) $^ $(PLATFLAG) -o $@

%.o : %.c
	$(CC) $(CFLAG) -c -o $@ $<

%.o : %.s
	$(CC) $(AFLAG) -c -o $@ $<



clean:
	rm -f *.o *.bin *.elf *.dis
