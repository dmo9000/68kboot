CC=/usr/local/gcc-68k/bin/m68k-elf-gcc
CFLAGS=-Wall -Wno-switch-bool -Wno-unused-value -Wno-unused-but-set-variable -m68000 -nostdlib -nodefaultlibs -nostdinc -Os -ffunction-sections -fdata-sections -I/usr/local/madlibc/include -Isys

BDOS_OBJS=fcntl.o kopen.o klseek.o kread.o kwrite.o kclose.o kstat.o exit.o vfs.o disk.o devices.o ext2.o bdos.o kperror.o kgetenv.o kchdir.o ktime.o kputenv.o kprintf.o kmemset.o	\
					endian.o kmemcpy.o kstrncmp.o kgetchar.o kstrtoul.o kstrncat.o kstrerror.o kstrchr.o modules.o dump.o ktermios.o kgetcwd.o kstrlen.o blkdev.o


all: bootldr shim bootldr.img 

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

shim:	$(BDOS_OBJS) shim.o fletcher16.o elf.o

	/usr/local/gcc-68k/bin/m68k-elf-ld -o shim -T kspace.lds --gc-sections --defsym=_start=main -Ttext=0x500 $(BDOS_OBJS) shim.o fletcher16.o elf.o	\
		/usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O srec shim shim.srec
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O binary shim shim.out

#		/usr/local/madlibc/lib/libmadlibc.a /usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a


bootldr: $(BDOS_OBJS) bootldr.o disk.o assert.o

	/usr/local/gcc-68k/bin/m68k-elf-ld -o bootldr --gc-sections --defsym=_start=main -Ttext=0x0400 bootldr.o disk.o assert.o exit.o \
    /usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O srec bootldr bootldr.srec
	#ls -l bootldr
	#size -A -d bootldr
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O binary bootldr bootldr.out

    #/usr/local/madlibc/lib/libmadlibc.a /usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a

bootldr.img: bootldr
	@@dd if=/dev/zero of=bootldr.img bs=128 count=256
	@@dd conv=notrunc if=bootldr.out of=bootldr.img
	@@dd if=shim.out of=bootldr.img bs=128 seek=256 oflag=append conv=notrunc
	ls -l *.out

clean:
	rm -f shim *.out *.srec *.o bootldr shim malltest md5sum *.img hello?.txt

veryclean: clean
	rm -f testfile.txt
	

install:
	sudo cp bdos.h /usr/local/madlibc/include/
	chmod 644 *.out
	cp bootldr.img ~/git-local/68kp/diskc.cpm.fs
	ls -l *.out
	@( SIZE_BOOT=`stat -c %s bootldr.out` ;	\
	if [ $${SIZE_BOOT} -gt 32768 ]; then	\
				echo "bootldr.out is too large (>32K)" ;	\
				exit	1	; \
			else			\
				echo "bootldr.out: size is okay (<= 32K)" ;				\
			fi )
	@( SIZE_SHIM=`stat -c %s shim.out` ;		\
	if [ $${SIZE_SHIM} -gt 65536 ]; then	\
				echo "shim.out is too large (>32K)" ;	\
				exit	1	; \
			else			\
				echo "shim.out:    size is okay (<= 64K)" ;				\
			fi )

