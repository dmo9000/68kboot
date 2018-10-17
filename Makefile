CC=/usr/local/gcc-68k/bin/m68k-elf-gcc
CFLAGS=-Wall -Wno-switch-bool -Wno-unused-value -m68000 -nostdlib -nodefaultlibs -Os -ffunction-sections -fdata-sections 

MADLIBC_OBJS=printf.o memset.o itoa.o strtoul.o memcpy.o strncmp.o dump.o \
			modules.o strerror.o perror.o puts.o putchar.o 

BDOS_OBJS=fcntl.o open.o read.o close.o exit.o vfs.o disk.o devices.o ext2.o bdos.o 
 

all: main newmain 8mb

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

main:	$(BDOS_OBJS) $(MADLIBC_OBJS) main.o
	/usr/local/gcc-68k/bin/m68k-elf-ld -o main --gc-sections --defsym=_start=main -Ttext=0x0400 $(MADLIBC_OBJS) $(BDOS_OBJS) main.o 	\
		/usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a 

	#/usr/local/gcc-68k/bin/m68k-elf-nm --print-size --size-sort --radix=d main
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O srec main main.srec
	/usr/local/gcc-68k/bin/m68k-elf-strip main
	ls -l main
	size -A -d main
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O binary main main.out
	#/usr/local/gcc-68k/bin/m68k-elf-objdump -d main.o

newmain:	$(MADLIBC_OBJS) crt0.o newmain.o
	/usr/local/gcc-68k/bin/m68k-elf-ld -T uspace.lds -o newmain --gc-sections --defsym=_start=_start -Ttext=0x100100 -e _start  crt0.o $(MADLIBC_OBJS) newmain.o 	\
		/usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a 

	#/usr/local/gcc-68k/bin/m68k-elf-nm --print-size --size-sort --radix=d main
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O srec newmain newmain.srec
	#/usr/local/gcc-68k/bin/m68k-elf-strip newmain
	ls -l newmain
	size -A -d newmain
	/usr/local/gcc-68k/bin/m68k-elf-objcopy --redefine-sym entry=_start -O binary newmain newmain.out
	#/usr/local/gcc-68k/bin/m68k-elf-objdump -d main.o


clean:
	rm -f main main.out main.srec newmain newmain.out newmain.srec *.o 8mb.img 

install:
	cp main.out ~/git-local/68kp/diskc.cpm.fs
	cp 8mb.img ~/git-local/68kp/8mb.img


8mb:
	@dd if=/dev/zero of=8mb.img count=65536 bs=128
	@mke2fs ./8mb.img
	@sudo mount 8mb.img mnt
	@sudo chown -R dan:dan mnt
	@printf "Hello world 1\r\n" > mnt/hello1.txt 2>&1
	@printf "Hello world 2\r\n" > mnt/hello2.txt 2>&1
	@dd if=/dev/urandom of=mnt/12blocks.bin bs=1024 count=12 1>/dev/null 2>&1
	@dd if=/dev/urandom of=mnt/13blocks.bin bs=1024 count=13 1>/dev/null 2>&1
	@mkdir -p mnt/foo/bar/baz
	@dd if=/dev/urandom of=mnt/foo/12blocks.bin bs=1024 count=12 1>/dev/null 2>&1
	@dd if=/dev/urandom of=mnt/foo/13blocks.bin bs=1024 count=13 1>/dev/null 2>&1
	@cp texttest.txt mnt/
	@cp newmain.out mnt/newmain.out
	@for FN1 in `seq 0 255` ; do \
		for FN2 in `seq 1 512`; do	\
			printf "%02x" $${FN1} >> mnt/testfile.txt ; \
			done	\
		done
	
	@ls --inode -ln mnt
	@sync
	@sudo umount mnt
