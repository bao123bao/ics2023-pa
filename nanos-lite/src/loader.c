#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
	
	int fd = fs_open(filename, 0, 0);
	
	Elf_Ehdr ehdr;
	
	size_t rd_len = fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
	assert(rd_len == sizeof(Elf_Ehdr));

	uint16_t phnum = ehdr.e_phnum;

	Elf_Phdr phdr[phnum];
	
	int seek_ret = fs_lseek(fd, ehdr.e_phoff, SEEK_SET);
	assert(seek_ret == ehdr.e_phoff);

	rd_len = fs_read(fd, &phdr, phnum*sizeof(Elf_Phdr));
	assert(rd_len == phnum*sizeof(Elf_Phdr));
//	ramdisk_read(&phdr, 0 + ehdr.e_phoff, phnum*sizeof(Elf_Phdr));

	int i;
	for(i=0; i<phnum; i++){
		if (phdr[i].p_type == PT_LOAD) {
			
			fs_lseek(fd, phdr[i].p_offset, SEEK_SET);
			fs_read(fd, (void *)phdr[i].p_vaddr, phdr[i].p_filesz);
			
			// set zeros
			memset((void *)phdr[i].p_vaddr + phdr[i].p_filesz, 0, phdr[i].p_memsz - phdr[i].p_filesz);
		}
	}
	fs_close(fd);

	return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  Log("naive_uload, filename=%s\n", filename);
	uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

