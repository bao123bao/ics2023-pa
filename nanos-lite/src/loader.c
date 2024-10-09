#include <proc.h>
#include <elf.h>
//#include <assert.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

//extern uint8_t ramdisk_start;
size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {

	Elf_Ehdr ehdr;
	ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
	
	uint16_t phnum = ehdr.e_phnum;
	uint16_t phentsize = ehdr.e_phentsize;

//	assert(*(uint32_t *)ehdr.e_ident == 0x7f454c46);
//	printf("assert passed\n");

	Elf_Phdr phdr[phnum];
	ramdisk_read(&phdr, 0 + ehdr.e_phoff, phnum*sizeof(Elf_Phdr));

	printf("phnum=%d, phentsize=%d\n", phnum, phentsize);
	int i;
	for(i=0; i<phnum; i++){
		printf("[%d] type=%d, offset=%d, vaddr=%d\n", 
			phdr[i].p_type, phdr[i].p_offset, phdr[i].p_vaddr);

	}


  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %d", entry);
 // ((void(*)())entry) ();
}

