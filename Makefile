CC=/usr/local/gcc-68k/bin/m68k-elf-gcc
CFLAGS=-Wall -Wno-switch-bool -Wno-unused-value -m68000 -nostdlib -nodefaultlibs -Os -ffunction-sections -fdata-sections 

OBJS=main.o printf.o memset.o itoa.o strtoul.o memcpy.o strncmp.o dump.o disk.o devices.o ext2.o \
		modules.o open.o strerror.o perror.o fcntl.o close.o read.o

all: main 8mb

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

main:	$(OBJS)
	/usr/local/gcc-68k/bin/m68k-elf-ld -o main --gc-sections --defsym=_start=main -Ttext=0x0400 $(OBJS) 	\
		/usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a 

	#/usr/local/gcc-68k/bin/m68k-elf-nm --print-size --size-sort --radix=d main
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O srec main main.srec
	/usr/local/gcc-68k/bin/m68k-elf-strip main
	ls -l main
	size -A -d main
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O binary main main.out
	#/usr/local/gcc-68k/bin/m68k-elf-objdump -d main.o

clean:
	rm -f main main.out main.srec *.o 

install:
	cp main.out ~/git-local/68kp/Musashi/diskc.cpm.fs
	cp 8mb.img ~/git-local/68kp/Musashi/8mb.img


8mb:
	@dd if=/dev/zero of=8mb.img count=65536 bs=128
	@mke2fs ./8mb.img
	@sudo mount 8mb.img mnt
	@sudo chown -R dan:dan mnt
	@echo "Hello world 1" > mnt/hello1.txt 2>&1
	@echo "Hello world 2" > mnt/hello2.txt 2>&1
	@dd if=/dev/urandom of=mnt/12blocks.bin bs=1024 count=12 1>/dev/null 2>&1
	@dd if=/dev/urandom of=mnt/13blocks.bin bs=1024 count=13 1>/dev/null 2>&1
	@mkdir -p mnt/foo/bar/baz
	@dd if=/dev/urandom of=mnt/foo/12blocks.bin bs=1024 count=12 1>/dev/null 2>&1
	@dd if=/dev/urandom of=mnt/foo/13blocks.bin bs=1024 count=13 1>/dev/null 2>&1
	@for FN in `seq 1 5`; do 	\
		dd if=/dev/urandom of=mnt/test-$${FN}.bin bs=1024 count=$${FN} 1>/dev/null 2>&1 ; \
		done
	@ls --inode -ln mnt
	@sync
	@sudo umount mnt
