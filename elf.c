#include "stdio.h"
#include <sys/types.h>
#include "elf.h"
#include <string.h>
#include "fcntl.h"
#include "assert.h"
#include "byteorder.h"
#include <unistd.h>
#include "klseek.h"
#include "kread.h"
#include "kclose.h"
#include "kopen.h"

//#define DEBUG_ELF

int loadelf(char *s)
{
//    kernel_printf("loadelf(%s)\r\n");
    int elf_ok = 0;
    assert(s);
    //char buffer[4096];
    //int rd = 0;
    //int i = 0;
    int elf_fd = 0;
    elf_fd = kopen(s, O_RDONLY);

    if (elf_fd == -1) {
        perror("open");
        return 0;
    }

    elf_ok = elf_load_binary(elf_fd);
    kclose(elf_fd);
    return elf_ok;
}

int elf_load_binary(int elf_fd)
{

    char buffer[4096];
    char section_string_table[2048];
    Elf32_Ehdr *Header = NULL;
    Elf32_Shdr *SectionHeader = NULL;

    unsigned long offset = 0;
    unsigned long entsize = 0;
    int i = 0;
    char sh_strname[17];
    char flags[5];
    int fo = 0;
    int   sh_offset = 0;
    unsigned char *base = NULL;
    int (*start)();
    unsigned char *addr = NULL;
    int rd = 0;
    int num_entries = 0;
    uint32_t e_shstrndx = 0;
    char *section_name = NULL;
    char *load_ptr = NULL;

    /* FIXME: we should seek to the start of the fd here */

    kernel_memset(&buffer, 0, 4096);
    rd = kread(elf_fd, &buffer, 4096);
#ifdef DEBUG_ELF
    kernel_printf("[elf.c(elf_fd=%d): read %d bytes]\r\n", elf_fd, rd);
#endif

    addr = (unsigned char *) &buffer;
    base = addr;

#ifdef DEBUG_ELF
    kernel_printf("elf_load_binary(0x%x)\r\n", addr);
#endif

    Header = (Elf32_Ehdr*) addr;

    if ((addr[0] == 0x7f) && (addr[1] == 'E') &&
            (addr[2] == 'L') && (addr[3] == 'F')) {
        //kernel_printf("ELF executable found at 0x%lx\r\n", addr);
    } else {
        //kernel_printf("ELF executable _NOT_ found at 0x%lx\r\n", addr);
        return (0);
    }


    if (Header->e_ident[EI_CLASS] != ELFCLASS32) {
        kernel_printf("Can't exec ELF class %u\r\n",
               Header->e_ident[EI_CLASS]);
        return(0);
    }

    /* need to be able to support this byte order */

    switch(Header->e_ident[EI_DATA]) {
    case ELFDATA2LSB:
        if (is_big_endian()) {
            kernel_printf("elf.c: wrong byte order (LSB)\r\n");
            return 0;
        }
        break;
    case ELFDATA2MSB:
        if (! is_big_endian()) {
            kernel_printf("elf.c: wrong byte order (MSB)\r\n");
            return 0;
        }
        break;
    default:
        kernel_printf("elf.c: unsupported elf->e_ident value (%u)\r\n", Header->e_ident[EI_DATA]);
        return 0;
    }

    assert(Header->e_ident[EI_DATA] != ELFDATANONE);

    /* see elf.h for bytes order selection */

    if (Header->e_ident[EI_VERSION] != EV_CURRENT) {
        kernel_printf("Can't exec ELF version %u\r\n",
               Header->e_ident[EI_VERSION]);
        return 0 ;
    }

    /* see elf.h for architecture selection */

    if (Header->e_machine != EM_68K) {
        kernel_printf("Invalid ELF architecture (%u)\r\n",
               Header->e_machine);
        return 0;
    }

    /* account for byte order */

#ifdef DEBUG_ELF
    kernel_printf("ELF Flags: %u\r\n", nm_uint32(Header->e_flags));

    kernel_printf("ELF section header table @ 0x%x:\r\n", Header->e_shoff);
    kernel_printf("e_shoff     = 0x%x (%u)\r\n", Header->e_shoff, Header->e_shoff);
    kernel_printf("e_shnum     = 0x%x\r\n", Header->e_shnum);
    kernel_printf("e_shentsize = 0x%x\r\n", Header->e_shentsize);
    kernel_printf("e_shstrndx  = 0x%x (%u)\r\n", Header->e_shstrndx, Header->e_shstrndx);
    kernel_printf("e_entry     = 0x%x\r\n", Header->e_entry);
#endif

    start = (int (*)()) Header->e_entry;
    offset = Header->e_shoff;

    num_entries = Header->e_shnum;
    /* focus the section header table */

    entsize = Header->e_shentsize;
//    kernel_memcpy(&SecHeader, addr+offset + (Header->e_shstrndx*entsize),
//           entsize);

    /* store offset to section header string table */

//    sh_offset = SecHeader.sh_offset;
//    addr+=offset;

    assert ((Header->e_shnum*Header->e_shentsize) < 4096);

#ifdef DEBUG_ELF
    kernel_printf("[ reading %u bytes of section header table from 0x%lx ]\r\n",
           (Header->e_shnum*Header->e_shentsize),
           Header->e_shoff);
#endif
    if (klseek(elf_fd, Header->e_shoff, SEEK_SET) == -1) {
        perror("klseek");
        return 0;
    }

    sh_offset = Header->e_shoff;
    e_shstrndx = Header->e_shstrndx;

    /* file descriptor should be pointed at the section header table by now */

    rd = kread(elf_fd, &buffer, Header->e_shnum*Header->e_shentsize);

    SectionHeader = (Elf32_Shdr *) &buffer;
    SectionHeader += e_shstrndx;

#ifdef DEBUG_ELF
    kernel_printf("(section name string table is section %u, offset = 0x%lx, size = 0x%lx)\r\n", e_shstrndx, SectionHeader->sh_offset,
           SectionHeader->sh_size);
#endif

    if (klseek(elf_fd, SectionHeader->sh_offset, SEEK_SET) == -1) {
        perror("klseek");
        return 0;
    }

    assert(SectionHeader->sh_size < 4096);
    kernel_memset(&section_string_table, 0, 256);

    rd = kread(elf_fd, &section_string_table, SectionHeader->sh_size);
//		ptr_dump(&section_string_table);
//		assert(NULL);

    SectionHeader = (Elf32_Shdr *) &buffer;

#ifdef DEBUG_ELF
    kernel_printf("[ID] %17s %13s   ADDRESS  OFFSET SIZE ESZ F     L  I  A\r\n",
           "SECTION_NAME", "SECTION_TYPE");
    kernel_puts("\r\n");
#endif

    for (i = 0; i < num_entries; i++) {
        kernel_memset(&flags, 0, 5);
        kernel_memset(&flags, '.',  4);
        kernel_memset(&sh_strname, 32, 17);

        fo = 0;


        if (SectionHeader->sh_flags & SHF_WRITE) {
            flags[fo] = 'W';
            fo++;
        }
        if (SectionHeader->sh_flags & SHF_ALLOC) {
            flags[fo] = 'A';
            fo++;
        }
        if (SectionHeader->sh_flags & SHF_EXECINSTR) {
            flags[fo] = 'X';
            fo++;
        }
        if (SectionHeader->sh_flags & SHF_MASKPROC) {
            flags[fo] = 'M';
            fo++;
        }

        //sh_string = base + sh_offset + SecHeader.sh_name;
        //memcpy(&sh_strname, sh_string, strlen(sh_string));

//				lseek(elf_fd, sh_offset + SectionHeader->sh_name, SEEK_SET);
//				read(elf_fd, &sh_strname,

        section_name = (char *) &section_string_table;
        section_name += SectionHeader->sh_name;


        if ((SectionHeader->sh_type == SHT_PROGBITS || SectionHeader->sh_type == SHT_NOBITS) && SectionHeader->sh_addr) {
#ifdef DEBUG_ELF
            kernel_printf("[%2u] 0x%08lx %17s sh_type=%14s sh_addr=%13u sh_offset=%6x sh_size=%6u sh_entsize=%u flags=%4s %4x %4x %4x\r\n",
                   i,
                   SectionHeader->sh_name,
                   //"sh_strname",
                   section_name,
                   //&section_string_table + SectionHeader->sh_name,
                   sh_types[SectionHeader->sh_type],
                   SectionHeader->sh_addr,
                   SectionHeader->sh_offset,
                   SectionHeader->sh_size,
                   SectionHeader->sh_entsize,
                   flags,
                   SectionHeader->sh_link,
                   SectionHeader->sh_info,
                   SectionHeader->sh_addralign);
#endif
            if (SectionHeader->sh_addr && SectionHeader->sh_type == SHT_PROGBITS) {
//                kernel_printf("Writing SHT_PROGBITS...\r\n");
                load_ptr = (char *) SectionHeader->sh_addr;
                klseek(elf_fd, SectionHeader->sh_offset, SEEK_SET);
                //kernel_printf("? = read(%d, 0x%lx, %u)\r\n", elf_fd, load_ptr, SectionHeader->sh_size);
                rd = kread(elf_fd, load_ptr, SectionHeader->sh_size);
                //kernel_printf("  + %d = read(%d, 0x%lx, %u)\r\n", rd, elf_fd, load_ptr, SectionHeader->sh_size);
                assert(rd == SectionHeader->sh_size);
            }


            if (SectionHeader->sh_addr && SectionHeader->sh_type == SHT_NOBITS) {
//                kernel_printf("Writing SHT_NOBITS...\r\n");
                load_ptr = (char *) SectionHeader->sh_addr;
                kernel_memset(load_ptr, 0, SectionHeader->sh_size);
            }

        }


        SectionHeader += 1;
    }

#ifdef DEBUG_ELF
    kernel_printf("[ELF LOADER RETURNING 1]\r\n");
    kernel_puts("\r\n");
#endif
    return(1);
}
