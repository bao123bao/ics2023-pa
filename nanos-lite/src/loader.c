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
	
	uintptr_t ret_addr = (uintptr_t)NULL;

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
	bool first_load = true;
	for(i=0; i<phnum; i++){
		printf("[%d] type=%d, offset=%d, vaddr=%d\n", 
			i, phdr[i].p_type, phdr[i].p_offset, phdr[i].p_vaddr);
		if (phdr[i].p_type == PT_LOAD) {
			
			// copy from ramrisk to memory
			ramdisk_read((void *)phdr[i].p_vaddr, 0 + phdr[i].p_offset, phdr[i].p_filesz);
			
			// set zeros
			memset((void *)phdr[i].p_vaddr + phdr[i].p_filesz, 0, phdr[i].p_memsz - phdr[i].p_filesz);

			if(first_load){
				ret_addr = (uintptr_t) phdr[i].p_vaddr;
				first_load = false;
				printf("jmp ret addr = %d\n", ret_addr);
			}
			printf("load the above\n");

		}
	}
	if(ret_addr)
	  return ret_addr;
	else 
		return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %d", entry);
  ((void(*)())entry) ();
}

