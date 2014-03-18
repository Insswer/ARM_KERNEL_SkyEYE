
helloworld.bin:helloworld.elf
	arm-none-linux-gnueabi-objcopy -O binary $< $@
	arm-none-linux-gnueabi-objdump -d helloworld.elf > helloworld.dis

helloworld.o : helloworld.c
	arm-none-linux-gnueabi-gcc -nostdlib -c helloworld.c -o helloworld.o

start.o : start.s
	arm-none-linux-gnueabi-gcc -nostdlib -c start.s -o start.o

helloworld.elf:helloworld.o start.o
	arm-none-linux-gnueabi-ld -Thello.lds helloworld.o start.o -o $@


clean:
	rm -f *.o *.bin *.elf *.dis 
