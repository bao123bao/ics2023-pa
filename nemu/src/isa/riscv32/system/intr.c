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
#include <stdio.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
	cpu.mcause = NO;
	cpu.mepc = (word_t) epc;

	uint32_t mask_MPP = (1U << 12) | (1U << 11);
  uint32_t mask_MPIE = (1U  << 7);
	// set bit 12 to 11
	cpu.mstatus |= mask_MPP;
	// reset bit 7
	cpu.mstatus &= ~mask_MPIE;
	//printf("riase NO = %d, cpu.mcause=%d,  epc=%d\n", NO, cpu.mcause, epc);
  return cpu.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
