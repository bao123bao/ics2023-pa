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

const char *regs_r[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_r_display(CPU_state *ref_r) {
	int i;
	printf("===Nemu vs ref gpr values===\n");
	printf("id          %-20s %-20s\n", "nemu (dut)", "ref");
	for (i=0; i<32; i++){
		printf("[%2d]%-7s 0x%-20x 0x%-20x\n", i, regs_r[i], cpu.gpr[i], ref_r->gpr[i]);
	}
	printf("%-7s 0x%-20x 0x%-20x\n", "pc", cpu.pc, ref_r->pc);
	// print CSRs
	printf("%-7s 0x%-20x 0x%-20x\n", "mcause", cpu.mcause, ref_r->mcause);
	printf("%-7s 0x%-20x 0x%-20x\n", "mstatus", cpu.mstatus, ref_r->mstatus);
	printf("%-7s 0x%-20x 0x%-20x\n", "mepc", cpu.mepc, ref_r->mepc);
	printf("%-7s 0x%-20x 0x%-20x\n", "mtvec", cpu.mtvec, ref_r->mtvec);
}

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  // pc is current instruction's address
	// ref_r->pc is dnpc value updated by ref
	int i;
	for(i=0; i<32; i++){
		if(ref_r->gpr[i] != cpu.gpr[i]){
			printf("Error at pc=0x%x, gpr[%d] <%s>  check failed\n",pc, i, regs_r[i]);
			isa_reg_r_display(ref_r);
			return false;
		}
	}
	if(ref_r->pc != cpu.pc){
		printf("Error at pc=0x%x, pc check failed\n", pc);
		printf("ref->pc=0x%x, dut->pc=0x%x\n", ref_r->pc, cpu.pc);
		isa_reg_r_display(ref_r);
		return false;
	}
	/*	
	if(ref_r->mstatus != cpu.mstatus){
		printf("Error at pc=0x%x, CSR mstatus check failed\n", pc);
		printf("ref->mstatus=0x%x, dut->mstatus=0x%x\n", ref_r->mstatus, cpu.mstatus);
		isa_reg_r_display(ref_r);
		return false;
	}
	*/
	/*
	if(ref_r->mcause != cpu.mcause){
		printf("Error at pc=0x%x, CSR mcause check failed\n", pc);
		printf("ref->mcause=0x%x, dut->mcause=0x%x\n", ref_r->mcause, cpu.mcause);
		isa_reg_r_display(ref_r);
		return false;
	}
	*/
	/*
	if(ref_r->mepc != cpu.mepc){
		printf("Error at pc=0x%x, CSR mepc check failed\n", pc);
		printf("ref->mepc=0x%x, dut->mepc=0x%x\n", ref_r->mepc, cpu.mepc);
		isa_reg_r_display(ref_r);
		return false;
	}
	*/
	if(ref_r->mtvec != cpu.mtvec){
		printf("Error at pc=0x%x, CSR mtvec check failed\n", pc);
		printf("ref->mtvec=0x%x, dut->mtvec=0x%x\n", ref_r->mtvec, cpu.mtvec);
		isa_reg_r_display(ref_r);
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
