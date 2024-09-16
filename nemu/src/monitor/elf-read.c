#include "elf-read.h"

void read_func_symbols(char *filename, func_sym *funcs, int *len){

	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdrs;
	char strtab[500];
	char shstrtab[500];
	Elf32_Sym *symtabs;
	uint32_t symtabs_len;
	int func_cnt = 0;
	size_t rb;
	
	FILE *fp;
	fp = fopen("./add-riscv32-nemu.elf", "r");
	if(!fp){
		printf("elf file read failure\n");
		return;
	}

	rb = fread((Elf32_Ehdr*)&ehdr, sizeof(Elf32_Ehdr), 1, fp);
	assert(rb == sizeof(Elf32_Ehdr));
	// read section header by e_shoff
	shdrs = (Elf32_Shdr *) malloc(ehdr.e_shnum * sizeof(Elf32_Shdr));
	fseek(fp, ehdr.e_shoff, SEEK_SET);
	rb = fread(shdrs, sizeof(Elf32_Shdr), ehdr.e_shnum, fp);
	assert(rb == ehdr.e_shnum * sizeof(Elf32_Shdr));
	
	int i;
	size_t str_lens[2];
	size_t sym_len = 0;
	uint32_t sym_off = 0;
	uint32_t str_offs[2];
	int j = 0;

	for(i=0; i<ehdr.e_shnum; i++){
		
		if(shdrs[i].sh_type == SHT_SYMTAB){
			sym_len += shdrs[i].sh_size;
			sym_off = shdrs[i].sh_offset;

		}else if(shdrs[i].sh_type == SHT_STRTAB){
			str_lens[j] = shdrs[i].sh_size;
			str_offs[j] = shdrs[i].sh_offset;
			j++;
		}

	}

	// read .strtab string
	fseek(fp, str_offs[0], SEEK_SET);
	rb = fread(strtab, str_lens[0], 1, fp);
	assert(rb == str_lens[0]);

	// read .shstrtab string
	fseek(fp, str_offs[1], SEEK_SET);
	rb = fread(shstrtab, str_lens[1], 1, fp);
	assert(rb == str_lens[1]);

	// read symbol table
	symtabs_len = sym_len / sizeof(Elf32_Sym);

	symtabs = (Elf32_Sym *) malloc(symtabs_len*sizeof(Elf32_Sym));
	fseek(fp, sym_off, SEEK_SET);
	rb = fread(symtabs, sizeof(Elf32_Sym), symtabs_len, fp);
	assert(rb == symtabs_len * sizeof(Elf32_Sym));
	
	printf("symtab FUNC entries:\n");
	for(i=0; i<symtabs_len; i++){
		if(ELF32_ST_TYPE(symtabs[i].st_info) == STT_FUNC){
			funcs[func_cnt].sym_addr = symtabs[i].st_value;
			strcpy(funcs[func_cnt].sym_name, &strtab[symtabs[i].st_name]);
			func_cnt++;
		}
	}

	*len = func_cnt;
}

/*

int main(){
	
	int len;
	func_sym funcs[100];
	
	char *filename = "./add-riscv32-nemu.elf";
	read_func_symbols(filename, funcs, &len);

	int i;
	printf("func syms:\n");
	for(i=0; i<len; i++){
		printf("[%2d] 0x%x, %s\n", i, funcs[i].sym_addr, funcs[i].sym_name);
	}

}
*/
