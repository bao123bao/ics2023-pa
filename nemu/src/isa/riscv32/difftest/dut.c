/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  // pc is current instruction's address
	// ref_r->pc is dnpc value updated by ref
	int i;
	for(i=0; i<32; i++){
		if(ref_r->gpr[i] != cpu.gpr[i]){
			printf("Error at pc=0x%x, gpr[%d] check failed\n", i, pc);
			return false;
		}
	}
	if(ref_r->pc != cpu.pc){
		printf("Error at pc=0x%x, dnpc check failed\n", pc);
		printf("ref->pc=0x%x, dut->dnpc=0x%x\n", ref_r->pc, cpu.pc);
		return false;
	}
//	printf("ref pc=0x%x, s->pc=0x%x, cpu.pc=0x%x\n", ref_r->pc, pc, cpu.pc);
	//assert(ref_r->pc == cpu.pc);

//	if(ref_r->pc != pc)
//		return false;
		
	return true;
}

void isa_difftest_attach() {
}
