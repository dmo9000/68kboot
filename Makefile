CC=/usr/local/gcc-68k/bin/m68k-elf-gcc
CFLAGS=-Wall -Wno-switch-bool -Wno-unused-value -m68000 -nostdlib -nodefaultlibs -Os -ffunction-sections -fdata-sections 

OBJS=main.o printf.o memset.o itoa.o strtoul.o memcpy.o strncmp.o dump.o disk.o devices.o ext2.o \
		modules.o

# LOL don't use -m68000 because gcc breaks, and so apparently does Mushashi
#
all: main 8mb

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

main:	$(OBJS)
	/usr/local/gcc-68k/bin/m68k-elf-ld -o main --gc-sections --defsym=_start=main -Ttext=0x0400 main.o printf.o memset.o itoa.o strtoul.o memcpy.o \
		strncmp.o dump.o disk.o	devices.o ext2.o modules.o \
		/usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a 


	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O srec main main.srec
	/usr/local/gcc-68k/bin/m68k-elf-strip main
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O binary main main.out
	#/usr/local/gcc-68k/bin/m68k-elf-objdump -d main.o

clean:
	rm -f main main.out main.srec *.o 

install:
	cp main.out ~/git-local/68kp/Musashi/diskc.cpm.fs
	cp 8mb.img ~/git-local/68kp/Musashi/8mb.img


8mb:
	dd if=/dev/zero of=8mb.img count=65536 bs=128
	mke2fs ./8mb.img
	sudo mount 8mb.img mnt
	sudo chown -R dan:dan mnt
	echo "Hello world 1" > mnt/hello1.txt
	echo "Hello world 2" > mnt/hello2.txt
	mkdir -p mnt/foo/bar/baz
	ls --inode -ln mnt
	sync
	sudo umount mnt
