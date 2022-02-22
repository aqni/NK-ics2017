#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(const char* expr_str){
  if(free_ == NULL || strlen(expr_str)>=sizeof(head->expr_str)){
    return NULL;
  }

  bool success=true;
  printf("expr:%s\n",head->expr_str);
  int value=expr(head->expr_str,&success);
  if(!success) return NULL;

  WP* temp=head;
  head=free_;
  head->next=temp;
  free_=free_->next;

  head->old_value=value;
  strcpy(head->expr_str,expr_str);
  return head;
}

void free_wp(int no){
  WP *curr=head, *last=NULL;
  while(curr!=NULL && curr->NO!=no ) {
    last=curr;
    curr=curr->next;
  }
  if(curr==NULL) {
    return;
  }else if(curr == head){
    head=curr->next;
  }else{
    last->next=curr->next;
  }

  curr->next=free_;
  free_=curr;
}

int print_wps(bool check_change){
  int print_num=0;
  for(WP* curr=head;curr!=NULL;curr=curr->next){
    if(!check_change){
      printf("%d\t%s\t\t\t= %d\n",curr->NO,curr->expr_str,curr->old_value);
      print_num++;
    }else{
      bool success=true;
      int new_value=expr(curr->expr_str, &success);
      assert(success);
      if(new_value!=curr->old_value){
        printf("%d\t%s\t\t\t= %d -> %d\n",curr->NO,curr->expr_str,
          curr->old_value,new_value);
        curr->old_value=new_value;
        print_num++;
      }
    }
  }
  return print_num;
}


