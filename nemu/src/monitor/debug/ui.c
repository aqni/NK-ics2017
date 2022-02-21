#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1); /* -1 -> UINTN_MAX */
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_p(char *args);

static int cmd_x(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "'si [N=1]', Let the program pause execution after stepping through N instructions. When N is not given, the default is 1.", cmd_si},
  { "info", "'info <r|w>', Prints specified information (registers or watchpoints).", cmd_info },
  { "p", "'p <EXPR>', Print the value of expression <EXPR>.", cmd_p },
  { "x", "'x <N> <EXPR>', Dump N 4-bytes start from memory address <EXPR>.", cmd_x },
  { "w", "'w <EXPR>', Watch the expression <EXPR>.", cmd_w },
  { "d", "'d <N>', Delete the watchpoint with sequence number N.", cmd_d }, 
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(args, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s \t- %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args){
  /* extract the first argument */
  char *arg=strtok(args," ");
  int n=1;
  if (arg != NULL) {
    n=atoi(arg);
    if(n<=0){
      printf("Unknown command args,'%s',(format is 'si [n=1]',n must more than 0)\n", arg);
      return 0;
    }
  }
  cpu_exec(n);
  return 0; 
}

static int cmd_info(char *args){
  /* extract the first argument */
  char *arg=strtok(args," ");
  if(strlen(arg)==1){
    switch(*arg){
      case 'r':
        /* global var 'cpu' */
        for(int i=0;i<sizeof(cpu.gpr)/sizeof(*cpu.gpr);i++){
            printf("%s\t\t0x%08X\n", regsl[i], cpu.gpr[i]._32);
        }
        printf("eip\t\t0x%08X\n",cpu.eip);
        return 0;
      case 'w':
        /* global var 'head'*/
        panic("unimplemented!\n");
        return 0;
      default: break;
    }
  }
  printf("Unknown command args, '%s'\n", args);
  return 0;
}

static int cmd_p(char *args){
  panic("unimplemented!\n"); 
  return 0;
}

static int cmd_x(char *args){
  /* extract the first argument */
  char *n_arg=strtok(args," ");
  char *expr_arg=strtok(NULL," ");
  int N=atoi(n_arg);
  uint32_t expr=atoi(expr_arg);
  if(N==0||expr==0){
    printf("Unknown command args, '%s'\n", args);
    return 0;
  }
  uint32_t addr =expr;
  for(int i=0;i<4*N;i+=4,addr+=4){
    printf("%#X <+%d>:\t\t0x%08X\n", addr, i, vaddr_read(addr,4));
  }
  return 0;
}

static int cmd_w(char *args){
  panic("unimplemented!\n");
  return 0; 
}

static int cmd_d(char *args){
  panic("unimplemented!\n"); 
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
