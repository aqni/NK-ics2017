#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* DONE: Add more members if necessary */
  char expr_str[128];
  int old_value;
} WP;

WP* new_wp(const char* expr_str);
void free_wp(int no);
int print_wps(bool check_change);

#endif
