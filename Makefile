CC=/usr/local/gcc-68k/bin/m68k-elf-gcc
CFLAGS=-Wall -Wno-switch-bool -Wno-unused-value -Wno-unused-but-set-variable -m68000 -nostdlib -nodefaultlibs -Os -ffunction-sections -fdata-sections

#MADLIBC_OBJS=printf.o memset.o itoa.o strtoul.o memcpy.o strncmp.o dump.o \
#			modules.o strerror.o puts.o putchar.o getchar.o strcmp.o strncpy.o memchr.o random.o

BDOS_OBJS=fcntl.o kopen.o klseek.o kread.o kclose.o exit.o vfs.o disk.o devices.o ext2.o bdos.o kperror.o


all: bootldr shim bootldr.img 

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

#shim:	$(BDOS_OBJS) $(MADLIBC_OBJS) shim.o fletcher16.o elf.o
shim:	$(BDOS_OBJS) shim.o fletcher16.o elf.o

	/usr/local/gcc-68k/bin/m68k-elf-ld -o shim -T kspace.lds --gc-sections --defsym=_start=main -Ttext=0x500 $(BDOS_OBJS) shim.o fletcher16.o elf.o \
		/usr/local/madlibc/lib/libmadlibc.a /usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O srec shim shim.srec
	#ls -l shim
	#size -A -d shim
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O binary shim shim.out

#bootldr: $(BDOS_OBJS) $(MADLIBC_OBJS) bootldr.o disk.o assert.o
bootldr: $(BDOS_OBJS) bootldr.o disk.o assert.o

	/usr/local/gcc-68k/bin/m68k-elf-ld -o bootldr --gc-sections --defsym=_start=main -Ttext=0x0400 bootldr.o disk.o assert.o exit.o \
    /usr/local/madlibc/lib/libmadlibc.a /usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O srec bootldr bootldr.srec
	#ls -l bootldr
	#size -A -d bootldr
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O binary bootldr bootldr.out

bootldr.img: bootldr
	@@dd if=/dev/zero of=bootldr.img bs=128 count=256
	@@dd conv=notrunc if=bootldr.out of=bootldr.img
	@@dd if=shim.out of=bootldr.img bs=128 seek=256 oflag=append conv=notrunc
	ls -l *.out

malltest:	$(MADLIBC_OBJS) crt0.o malltest.o assert.o exit.o sbrk.o malloc.o perror.o
	/usr/local/gcc-68k/bin/m68k-elf-ld -T uspace.lds -o malltest --gc-sections --defsym=_start=_start -Ttext=0x100100 -e _start  crt0.o $(MADLIBC_OBJS) malltest.o 	\
		assert.o exit.o sbrk.o malloc.o perror.o \
		/usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O srec malltest malltest.srec
	#ls -l malltest
	#size -A -d malltest
	/usr/local/gcc-68k/bin/m68k-elf-objcopy --redefine-sym entry=_start -O binary malltest malltest.out

md5sum:    $(MADLIBC_OBJS) crt0.o md5sum.o assert.o exit.o sbrk.o malloc.o fcntl_uspace.o fopen.o fread.o fclose.o ustdio.o perror.o
	/usr/local/gcc-68k/bin/m68k-elf-ld -T uspace.lds -o md5sum --gc-sections --defsym=_start=_start -Ttext=0x100100 -e _start  crt0.o $(MADLIBC_OBJS) md5sum.o    \
		assert.o exit.o sbrk.o malloc.o fcntl_uspace.o fopen.o fread.o fclose.o	ustdio.o perror.o \
		/usr/local/gcc-68k/lib/gcc/m68k-elf/8.2.0/m68000/libgcc.a
	/usr/local/gcc-68k/bin/m68k-elf-objcopy -O srec md5sum md5sum.srec
	#ls -l md5sum
	#size -A -d md5sum
	/usr/local/gcc-68k/bin/m68k-elf-objcopy --redefine-sym entry=_start -O binary md5sum md5sum.out


clean:
	rm -f shim *.out *.srec *.o bootldr shim malltest md5sum *.img hello?.txt

veryclean: clean
	rm -f testfile.txt
	

install:
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

