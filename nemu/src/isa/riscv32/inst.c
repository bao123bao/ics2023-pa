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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../monitor/elf-read.h"

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write
#define STACK_SIZE 500

enum {
  TYPE_I, TYPE_U, TYPE_S, TYPE_J, TYPE_R, TYPE_B, 
  TYPE_N, // none
};

/*
enum {
	EVENT_NULL = 0,
 	EVENT_YIELD, EVENT_SYSCALL, EVENT_PAGEFAULT, EVENT_ERROR,
 	EVENT_IRQ_TIMER, EVENT_IRQ_IODEV,
};
*/
//static bool yield_flag = false;


#ifdef CONFIG_FTRACE
// return addr of function call
static vaddr_t ret_addr;
static int stack_top = -1;
static int idx_stack_top = -1;
static vaddr_t ret_addr_stack[STACK_SIZE];
static int idx_stack[STACK_SIZE];
static int indent_level = 0;

#endif

// flag for jal and jalr
static bool ja_flag = false;

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immJ() do { *imm = (SEXT(BITS(i, 31, 31), 1)<<20) | (BITS(i, 19, 12)<<12) | (BITS(i, 20, 20)<<11) | BITS(i, 30, 21) << 1; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
#define immB() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 12) | BITS(i, 7, 7) << 11 | BITS(i, 30, 25) << 5 | BITS(i, 11, 8) << 1;  } while(0)

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
//	printf("deocde_operand: rs1=%d, rs2=%d, rd=%d\n", rs1, rs2, *rd);
  switch (type) {
    case TYPE_I: 
			src1R();
	//		printf("decode_operand switch: rs1=%d, *src1=%d\n",rs1, *src1);
			immI();
			//printf("imm=0x%x\n", *imm);
			break;
    case TYPE_U:                   
			immU(); 
			//printf("imm=0x%x\n", *imm);
			break;
		case TYPE_J:
			immJ();
			//printf("imm=0x%x\n", *imm);
			break;
    case TYPE_S: 
			src1R(); 
			src2R(); 
			immS(); 
			break;
		case TYPE_B:
			src1R();
			src2R();
			immB();
			break;
		case TYPE_R:
			src1R();
			src2R();
			break;
  }
}

static int decode_exec(Decode *s) {
	int rd = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);
	INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm);
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));
	INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd) = Mr(src1 + imm, 4));
	INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, R(rd) = SEXT(Mr(src1 + imm, 1), 8));
	INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(rd) = SEXT(Mr(src1 + imm, 2), 16));
	INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(rd) = Mr(src1 + imm, 2));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
	INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
	INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm);
	INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm);
	INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(rd) = src1 | imm);
	INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);
	INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd) = src1 + src2);
	INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(rd) = ((int32_t)src1 < (int32_t)src2) ? 1 : 0);
	INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(rd) = ((uint32_t)src1 < (uint32_t)src2 ? 1 : 0));
	INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd) = src1 ^ src2);
	INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd) = src1 | src2);
	INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd) = src1 & src2);
	INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd) = src1 << src2);
	INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(rd) = (int32_t)src1 >> src2);
	INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(rd) = (uint32_t)src1 >> src2);
	INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(rd) = ((uint32_t)src1) >> imm );
	INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(rd) = src1 << imm);
	INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = (int32_t)src1 >> imm);
	INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(rd) = ((uint32_t)src1 < (uint32_t)imm) ? 1 : 0);
	INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd) = src1 - src2);
	INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(rd) = s->snpc; s->dnpc = s->pc + imm;       ja_flag = true);
	INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, R(rd) = s->snpc; s->dnpc = (src1 + imm) & ~1; ja_flag = true);
	INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));
	INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, if(src1 == src2) s->dnpc = s->pc + imm);
	INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, if(src1 != src2) s->dnpc = s->pc + imm);
	INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, if((int32_t)src1 < (int32_t)src2) s->dnpc = s->pc + imm);
	INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, if((uint32_t)src1 < (uint32_t)src2) s->dnpc = s->pc + imm);
	INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, if((int32_t)src1 >= (int32_t)src2) s->dnpc = s->pc + imm);
	INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, if((uint32_t)src1 >= (uint32_t)src2) s->dnpc = s->pc + imm);
	INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(rd) = BITS(src1 * src2, 31, 0));
	INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, R(rd) = BITS( ((int64_t)SEXT(src1, 32)) * ((int64_t) SEXT(src2, 32)), 63, 32));
	INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu  , R, R(rd) = BITS( ((uint64_t)src1) * ((uint64_t)src2), 63, 32));
	INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, R(rd) = (int32_t)src1 / (int32_t)src2);
	INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, R(rd) = (uint32_t)src1 / (uint32_t)src2);
	INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, R(rd) = (int32_t)src1 % (int32_t)src2);
	INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, R(rd) = (uint32_t)src1 % (uint32_t)src2);

	INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrrw  , I, 
	//	printf("csrrw called\n");
		word_t *csrp;
		switch(imm){
			case 0x300: csrp = &cpu.mstatus; break;
			case 0x305: csrp = &cpu.mtvec;   break;
			case 0x341: csrp = &cpu.mepc;    break;
			case 0x342: csrp = &cpu.mcause;  break;
			default: assert(0);
		}
		if(rd!=0){
			R(rd) = *csrp;
		}
		*csrp = src1;
	);

	INSTPAT("??????? ????? ????? 010 ????? 11100 11", csrrs  , I,
		word_t *csrp;
		switch(imm){
			case 0x300: csrp = &cpu.mstatus; break;
			case 0x305: csrp = &cpu.mtvec;   break;
			case 0x341: csrp = &cpu.mepc;    break;
			case 0x342: csrp = &cpu.mcause;  break;
			default: assert(0);
		}
//		printf("csr0x%x = 0x%x\n",imm,*csrp);
		R(rd) = *csrp;
		//printf("rd=%d, csr0x%x = 0x%x\n",)
		*csrp = *csrp | (uint32_t)src1;
	);

	INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall  , N, 
#ifdef CONFIG_ETRACE
		printf("exception raised by ecall @ 0x%x\n", s->pc);
#endif
		// should read exception number from a7
		s->dnpc = isa_raise_intr(11, s->pc);
	);
	
	INSTPAT("0011000 00010 00000 000 00000 11100 11", mret   , N, 
		s->dnpc = cpu.mepc;
		uint32_t mask_MPP = (1U << 12) | (1U << 11);
		uint32_t mask_MPIE = (1U  << 7);
		// reset bit 12 to 11
		cpu.mstatus &= ~mask_MPP;
		// set bit 7
		cpu.mstatus |= mask_MPIE;
		//yield_flag = false;
	);

  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, 
#ifdef CONFIG_ETRACE
		printf("exception raised by ebreak @ 0x%x\n", s->pc);
#endif	
		NEMUTRAP(s->pc, R(10));
	); // R(10) is $a0

  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();
//	printf("s->dnpc=0x%x, s->pc=0x%x, imm=0x%x, rd=0x%x\n", s->dnpc, s->pc,imm, R(rd));
  R(0) = 0; // reset $zero to 0

	
#ifdef CONFIG_FTRACE
	int func_idx;
	int i;


	// check for function call or return
	if(ja_flag){
		//printf("ja_flag==true\n");
		// check for funcion call
		//printf("s->dnpc=0x%x, ret_addr=0x%x\n", s->dnpc, ret_addr);
		if((func_idx = check_func_sym(s->dnpc, funcs, func_sym_len)) > 0){
			// have a function call
			for(i=0; i<indent_level; i++){
				printf("    ");
			}
			printf("call <%s> @ 0x%x, ret_addr=0x%x\n", 
				funcs[func_idx].sym_name, s->dnpc, s->snpc);
			ret_addr = s->snpc;
			
			// push in return address into stack
//			printf("push 0x%x\n", s->snpc);
			ret_addr_stack[++stack_top] = s->snpc;
			idx_stack[++idx_stack_top] = func_idx;
			assert(stack_top < STACK_SIZE-1);

			indent_level++;
			//printf("expected return addr: 0x%x\n", ret_addr);
		}else if(s->dnpc == ret_addr_stack[stack_top]) {	
	//		printf("pop 0x%x\n", ret_addr_stack[stack_top]);
			stack_top--;
			// check for function return 
			indent_level--;
			for(i=0; i<indent_level; i++){
				printf("    ");
			}
			printf("<%s> return, to addr=0x%x\n", funcs[idx_stack[idx_stack_top--]].sym_name, s->dnpc);
		}
		ja_flag = false;
	}

#endif

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
