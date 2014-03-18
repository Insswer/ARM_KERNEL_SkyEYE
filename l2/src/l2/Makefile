#定义变量
OBJ = main.bin
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CFLAG = -nostdlib -fno-builtin
LFLAG = -Tlink.lds -nostdlib
OBJCOPY = arm-none-linux-gnueabi-objcopy
OBJDUMP = arm-none-linux-gnueabi-objdump

all : $(OBJ)

#目标 : 依赖
#[tab]命令
main.bin:main.elf
	$(OBJCOPY) -O binary $< $@
	$(OBJDUMP) -d main.elf > main.dis

main.elf:main.o start.o
	$(LD) $(LFLAG) $^ -o $@

%.o : %.c
	$(CC) $(CFLAG) -c -o $@ $<

%.o : %.s
	$(CC) $(CFLAG) -c -o $@ $<



clean:
	rm -f *.o *.bin *.elf *.dis
