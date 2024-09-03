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

#define NR_WP 5 

//static bool debug_flag = true;

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void print_wps(){
	WP *wp;
	printf("WP HEAD: \n");
	wp = head;
	if(!wp){
		printf("this wp list is NULL\n");
		return;
	}

	while(wp){
		printf("WP#%d ", wp->NO);
		wp = wp->next;
	}
	printf("END\n");
	
	wp = free_;
	printf("WP FREE_: \n");
	if(!wp){
		printf("this wp list is NULL\n");
		return;
	}

	while(wp){
		printf("WP#%d ", wp->NO);
		wp = wp->next;
	}
	printf("END\n");
}


// find wp's previous in head list
WP* find_prev(WP* head, WP* wp){
	if(!head || !wp){
		printf("input ptr is NULL, cannot find prev\n");
		return NULL;
	}

	WP *temp = head;
	while(temp->next && temp->next!=wp){
		temp = temp->next;
	}
	if(temp->next==NULL){
		printf("cannot find wp's prev\n");
		return NULL;
	}
		
	return temp;
	
}


WP* new_wp() {
	if (!free_){
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

void wp_test(){
	init_wp_pool();
	print_wps();

	WP *wp1 = new_wp();
	print_wps();

	free_wp(wp1);
	print_wps();

}















