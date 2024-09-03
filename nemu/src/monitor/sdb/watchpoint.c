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

#include "sdb.h"
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#define NR_WP 5 

static bool debug_flag = false;
static bool first_call = true;

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
	char str[50];
	uint32_t val;
	bool val_valid;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
		wp_pool[i].str[0] = '\0';
		wp_pool[i].val_valid = false;
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void print_wps(){
	// print list head
	WP *wp;
	printf("WP HEAD: ");
	wp = head;
	if(!wp){
		printf("NULL\n");
	}else{
		while(wp){
			printf("WP#%d ", wp->NO);
			wp = wp->next;
		}
		printf("END\n");
	}


	// print list free_
	wp = free_;
	printf("WP FREE_: ");
	if(!wp){
		printf("NULL\n");
	}else{
		while(wp){
			printf("WP#%d ", wp->NO);
			wp = wp->next;
		}
		printf("END\n");
	}
	putchar('\n');
}


// find wp's previous in head list
WP* find_prev(WP* head, WP* wp){
	if(!head || !wp){
		if(debug_flag)
			printf("input ptr is NULL, cannot find prev\n");
		return NULL;
	}

	WP *temp = head;
	while(temp->next && temp->next!=wp){
		temp = temp->next;
	}
	if(temp->next==NULL){
		if(debug_flag)
			printf("cannot find wp's prev\n");
		return NULL;
	}
		
	return temp;
	
}


WP* new_wp() {
	if (!free_){
		if(debug_flag)
			printf("No free watchpoint available, cannot new\n");
		return NULL;
	}

	if (!head){
		// if call new_wp for the first time (head==NULL)
		head = free_;
		free_ = free_->next;
		head->next = NULL;
		return head;
	}else{
		// head is not NULL
		WP *temp = free_;
		free_ = free_->next;
		temp->next = head;
		head = temp;
		return head;
	}
}

void free_wp(WP *wp) {
	if (!wp){
		if(debug_flag)
			printf("no allocated watchpoint, cannot free\n");
		return;
	}

	if (wp==head){
		// wp is head
		if (free_) {
			// free is not null
			WP *temp = free_;
			head = head->next;
			free_ = wp;
			free_->next = temp;
		}else{
		 	// free is null
			head = head->next;
			free_ = wp;
			free_->next = NULL;
		}	
	}else{
		// wp is not head
		if (free_){
			// free is not null
			WP *temp = find_prev(head, wp);
			temp->next = wp->next;
			wp->next = free_;
			free_ = wp;
		}else{
			// free is null
			WP *temp = find_prev(head, wp);
			temp->next = wp->next;
			wp->next = NULL;
			free_ = wp;
		}
		
	}
}

void create_watchpoint(char* e){
	if(first_call){
		if(debug_flag)
			printf("first call, init wp\n");
		init_wp_pool();
		first_call = false;
	}
	bool success = false;
	int val = expr(e, &success);
	if (success) {
		WP *wp = new_wp();
		strcpy(wp->str, e);
		wp->val = val;
		wp->val_valid = true;
		printf("Watchpoint %d: %s (value: %u = 0x%x)\n", wp->NO, wp->str, val, val);
		if(debug_flag)
			print_wps();
	}else{
		printf("Invalid expression\n");
		if(debug_flag)
			print_wps();
	}
}

void info_watchpoints(){
	WP *wp = head;
	if(first_call || !head){
		printf("No watchpoint created yet\n");
		return;
	}
	printf("%-6s %-15s\n", "Num", "What");
	while(wp){
		printf("%-6d %-15s\n", wp->NO, wp->str);
		wp = wp->next;
	}
}

void delete_watchpoint(int id){
	WP *wp = head;
	while ( wp && (wp->NO!=id) ){
		wp = wp->next;
	}
	if(wp){
		free_wp(wp);
		printf("Watchpoint %d deleted\n", id);
	}else{
		printf("Watchpoint %d not found\n", id);
	}
}

bool scan_watchpoints(){
	WP *wp = head;
	bool val_changed = false;

	while (wp){
		assert(wp->val_valid);
		bool success;
		uint32_t old_val = wp->val;
		uint32_t new_val = expr(wp->str, &success);
		assert(success);
		if (new_val != old_val) {
			printf("Watchpoint %d: %s\n", wp->NO, wp->str);
			printf("	Old value = %u (0x%x)\n", old_val, old_val);
			printf("	New value = %u (0x%x)\n", new_val, new_val);
			wp->val = new_val;
			val_changed = true;
		}
		wp = wp->next;
	}
	return val_changed;
}

void wp_test(){
	if(first_call){
		printf("first call, init wp");
		init_wp_pool();
		first_call = false;
	}
	print_wps();

	WP *wp1 = new_wp();
	WP *wp2 = new_wp();
	print_wps();

	free_wp(wp1);
	free_wp(wp2);
	print_wps();

}
