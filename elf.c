#include "kernel.h"
#include "types.h"
#include "elf.h"
#include "gdt.h"
#include "machdep.h"
#include "lock.h"
#include "proc.h"
#include "kinfo.h"
#define MAX_TASKS SYSTEM_PROCESS_MAX

#define NULL	0
//#define DEBUG_ELF

extern byte *gdt;
unsigned long _tss_sel;
extern volatile struct proc *curproc;
extern struct i386tss *dummytss;
extern _kernel_info k_info;



int elf_check_magic(unsigned long addr, unsigned long length)
{
    /* FIXME: move header checks from elf_load_binary to here */
    printf("elf_check_magic(0x%x, 0x%x)\n", addr, length);
    return (1);
}

int elf_load_exec(struct proc *p, unsigned char *addr, byte *dummy)
{
//		printf("elf_load_exec(%u, %u)\n", p, addr);
    elf_load_binary(addr, dummy);
    return 0;
}

int elf_load_binary(unsigned char *addr, byte *dummy)
{

    Elf32_Ehdr *Header = NULL;
    Elf32_Shdr SecHeader;
    unsigned long offset = 0;
    unsigned long entsize = 0;
    int i = 0;
    char sh_strname[17];
    char flags[5];
    int fo = 0;
    int   sh_offset = 0;
    char *sh_strtab = NULL;
    char *sh_string  = NULL;
    unsigned char *base = addr;
    unsigned int pid = 0;
    int (*start)();

//	printf("elf_load_binary(0x%x)\n", addr);

    Header = (Elf32_Ehdr*) addr;

    if ((addr[0] == 0x7f) && (addr[1] == 'E') &&
            (addr[2] == 'L') && (addr[3] == 'F')) {
        printf("ELF executable found at 0x%x\n", addr);
    } else {
        printf("ELF executable _NOT_ found at 0x%x\n", addr);
        return (-1);
    }


    if (Header->e_ident[EI_CLASS] != ELFCLASS32) {
        printf("Can't exec ELF class %u\n",
               Header->e_ident[EI_CLASS]);
        return(-1);
    }

    if (Header->e_ident[EI_DATA] != ELFDATA2LSB) {
        printf("Can't exec ELF byte order %u\n",
               Header->e_ident[EI_DATA]);
        return(-1);
    }

    if (Header->e_ident[EI_VERSION] != EV_CURRENT) {
        printf("Can't exec ELF byte order %u\n",
               Header->e_ident[EI_VERSION]);
        return -1;
    }

    if (Header->e_machine != EM_386) {
        printf("Invalid ELF architecture (%u)\n",
               Header->e_machine);
        return -1;
    }

    if (Header->e_flags != 0) {
        printf("Invalid (e_flags set for arch EM_386)\n");
        return -1;
    }


#ifdef DEBUG_ELF
    printf("ELF section header table @ 0x%x:\n", Header->e_shoff);
    printf("e_shoff     = 0x%x\n", Header->e_shoff);
    printf("e_shnum     = 0x%x\n", Header->e_shnum);
    printf("e_shentsize = 0x%x\n", Header->e_shentsize);
    printf("e_shstrndx  = 0x%x\n", Header->e_shstrndx);
    printf("e_entry     = 0x%x\n", Header->e_entry);
#endif /* DEBUG_ELF */

    start = Header->e_entry;

    offset = Header->e_shoff;

    /* focus the section header table */

    entsize = Header->e_shentsize;
    kmemcpy(&SecHeader, addr+offset + (Header->e_shstrndx*entsize),
            entsize);

    /* store offset to section header string table */

    sh_offset = SecHeader.sh_offset;
    addr+=offset;

    for (i = 0; i < Header->e_shnum; i++) {
        kmemcpy(&SecHeader, addr, entsize);
        kmemset(&flags, 0, 5);
        kmemset(&flags, 32,  4);
        kmemset(&sh_strname, 32, 17);

        fo = 0;

        if (SecHeader.sh_flags & SHF_WRITE) {
            flags[fo] = 'W';
            fo++;
        }
        if (SecHeader.sh_flags & SHF_ALLOC) {
            flags[fo] = 'A';
            fo++;
        }
        if (SecHeader.sh_flags & SHF_EXECINSTR) {
            flags[fo] = 'X';
            fo++;
        }
        if (SecHeader.sh_flags & SHF_MASKPROC) {
            flags[fo] = 'M';
            fo++;
        }

        sh_string = base + sh_offset + SecHeader.sh_name;
        kmemcpy(&sh_strname, sh_string, strlen(sh_string));
#ifdef DEBUG_ELF
        printf("  [%2u] %17s %14s %08x %06x %06x %02x %2s %2u %2x %2x\n",
               i,
               sh_strname,
               sh_types[SecHeader.sh_type],
               SecHeader.sh_addr,
               SecHeader.sh_offset,
               SecHeader.sh_size,
               SecHeader.sh_entsize,
               flags,
               SecHeader.sh_link,
               SecHeader.sh_info,
               SecHeader.sh_addralign);
#endif /* DEBUG_ELF */
        /* cheap and nasty setup for calling program from
           kernel. with paging and multitasking enabled, we
           wouldn't do this */

        if (SecHeader.sh_addr > 0 && SecHeader.sh_size > 0) {
            if (kmemcpy(SecHeader.sh_addr, base+SecHeader.sh_offset,
                        SecHeader.sh_size) == NULL) {
                printf("kmemcpy() failed in ELF setup\n");
                return -1;
            }
        }
        addr += entsize;
    }


    return(1);
}
