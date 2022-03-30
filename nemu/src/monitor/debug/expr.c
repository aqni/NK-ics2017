#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

uint32_t eval(int lidx, int ridx, bool *success);

enum {

  /* DONE: Add more token types */

  TK_NOTYPE = 0x100, TK_EQ, TK_NE, TK_LE, TK_GE, TK_AND, TK_OR, TK_LS, TK_RS,
  TK_DEREF, TK_MINUS, TK_NUM, TK_REG
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* DONE: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces

  {"([1-9][0-9]*)|(0[0-7]+)|(0x[0-9A-Fa-f]+)|0", TK_NUM},
  {"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip)", TK_REG},
  {"\\(", '('},
  {"\\)", ')'},

  {"\\+", '+'},         // plus
  {"-", '-'},
  {"\\*", '*'},
  {"/", '/'},
  {"%", '%'},

  {"==", TK_EQ},        // equal
  {"!=", TK_NE},
  {"<=", TK_LE},
  {">=", TK_GE},
  {">", '>'},
  {"<", '<'},

  {"&&", TK_AND},
  {"\\|\\|", TK_OR},
  {"!", '!'},

  {"&", '&'},
  {"\\|", '|'},
  {"\\^", '^'},
  {"~", '~'},
  {"<<", TK_LS},
  {">>", TK_RS},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(const char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        const char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* DONE: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        if(nr_token >= sizeof(tokens)/sizeof(*tokens)){
          printf("There are too many tokens (up to %d)!\n",nr_token);
          return false;
        }

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case TK_REG:
          case TK_NUM:
            if(substr_len >= sizeof(tokens->str)-1){
              printf("Token string is too large (up to %d bytes))!",substr_len);
              return false;
            }
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len]='\0';
          default:
            tokens[nr_token].type=rules[i].token_type;
            nr_token++;
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int lidx,int ridx,bool *success){
  /* Check brackets */
  int lb=0;
  for(int i=lidx;i<=ridx;i++){
    if(tokens[i].type=='('){
      lb++;
    }else if(tokens[i].type==')'){
      if(lb==0) return false;
      lb--;
    }
  }
  if(lb!=0){
    printf("unmatched parentheses!\n");
    return *success=false;
  }
  return (tokens[lidx].type=='(') && (tokens[ridx].type==')');
}

int priority_of(int type){
  switch(type){
    default: return -1;
    case TK_MINUS:
    case TK_DEREF:
    case '!':
    case '~':return 2;
    case '*': 
    case '/':
    case '%': return 3;
    case '+':
    case '-': return 4;
    case TK_LS:
    case TK_RS: return 5;
    case TK_LE:
    case TK_GE:
    case '>':
    case '<': return 6;
    case TK_EQ:
    case TK_NE:return 7;
    case '&': return 8;
    case '^': return 9;
    case '|': return 10;
    case TK_AND: return 11;
    case TK_OR: return 12;
  }
}

int operands_num_of(int type){
  switch (type){
    default: return -1;
    case TK_MINUS:
    case TK_DEREF:
    case '!':
    case '~':return 1;
    case '*': 
    case '/':
    case '%':
    case '+':
    case '-':
    case TK_LS:
    case TK_RS:
    case TK_LE:
    case TK_GE:
    case '>':
    case '<':
    case TK_EQ:
    case TK_NE:
    case '&':
    case '^':
    case '|':
    case TK_AND:
    case TK_OR: return 2;
  }
}

static int get_op_pos(int lidx,int ridx,bool* success)
{
  int op=lidx, priority=-1;
  bool inbracket=false;
  for(int i=lidx;i<=ridx;i++){
    switch (tokens[i].type){
      case '(': inbracket=true; break;
      case ')': inbracket=false; break;
      default:break;
    }
    if(inbracket) continue;
    
    int curr_prio=priority_of(tokens[i].type);
    if(curr_prio>=priority){
      op=i;
      priority=curr_prio;
    }
  }
  assert(!inbracket);
  if(priority<0 ||(operands_num_of(tokens[op].type)<2 && op!=lidx)){
    printf("Bad expression!\n"); 
    *success=false;
  }
  return op;
}

static int cal_expr(int op,int lidx,int ridx,bool* success)
{
#define EVAL_VAL1 (eval(lidx, op - 1, success))
#define EVAL_VAL2 (eval(op + 1, ridx, success))
  switch (tokens[op].type) {
    default: return *success=false;
    case TK_MINUS: return - EVAL_VAL2;
    case TK_DEREF: return vaddr_read(EVAL_VAL2,4);
    case '!': return ! EVAL_VAL2;
    case '~': return ~ EVAL_VAL2;
    case '*': return EVAL_VAL1 * EVAL_VAL2; 
    case '/': return EVAL_VAL1 / EVAL_VAL2;
    case '%': return EVAL_VAL1 % EVAL_VAL2;
    case '+': return EVAL_VAL1 + EVAL_VAL2;
    case '-': return EVAL_VAL1 - EVAL_VAL2;
    case TK_LS: return EVAL_VAL1 << EVAL_VAL2;
    case TK_RS: return EVAL_VAL1 >> EVAL_VAL2;
    case TK_LE: return EVAL_VAL1 <= EVAL_VAL2;
    case TK_GE: return EVAL_VAL1 >= EVAL_VAL2;
    case '>': return EVAL_VAL1 > EVAL_VAL2;
    case '<': return EVAL_VAL1 < EVAL_VAL2;
    case TK_EQ: return EVAL_VAL1 == EVAL_VAL2;
    case TK_NE: return EVAL_VAL1 != EVAL_VAL2;
    case '&': return EVAL_VAL1 & EVAL_VAL2;
    case '^': return EVAL_VAL1 ^ EVAL_VAL2;
    case '|': return EVAL_VAL1 | EVAL_VAL2;
    case TK_AND: return EVAL_VAL1 && EVAL_VAL2;
    case TK_OR: return EVAL_VAL1 || EVAL_VAL2;
  }
#undef EVAL_VAL1
#undef EVAL_VAL2
}

static int token_value(int pos,bool* success){
  uint32_t val=0;
  switch(tokens[pos].type){
    default: return *success=false;
    case TK_NUM:
      if(EOF==sscanf(tokens[pos].str,"%i",&val)){
        printf("unknow num:%s\n",tokens[pos].str); 
        return *success=false;
      }
      return val;
    case TK_REG: 
      if(0==strcmp(tokens[pos].str+1,"eip")){
        return cpu.eip;
      }
      for(int i=0;i<8;i++){
        if(0==strcmp(tokens[pos].str+1,regsl[i])){
          return cpu.gpr[i]._32;
        }
      }
      Assert(0,"Unimplemented token: %d\n",tokens[pos].type);
  }
  return 0;
}


uint32_t eval(int lidx, int ridx, bool *success) {
  if (lidx > ridx) {
    /* Bad expression */
    printf("Bad expression!\n");
    return *success=false;
  }
  else if (lidx == ridx) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    return token_value(lidx,success);
  }
  else if (check_parentheses(lidx, ridx, success) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(lidx + 1, ridx - 1, success);
  }
  else {
    /* get the position of dominant operator in the token expression */
    int op=get_op_pos(lidx,ridx,success);
    Log("opindex:%d\n",op);
    /* return the value */
    return cal_expr(op,lidx,ridx,success);
  }
}

uint32_t expr(const char *e, bool *success) {
  if (!e||!make_token(e)) {
    *success = false;
    return 0;
  }

  /* DONE: Insert codes to evaluate the expression. */

  for(int i=0;i<nr_token;i++){
    switch(tokens[i].type){
      default: break;
      case TK_NOTYPE:assert(0);
      case '-':
        if(i==0||(tokens[i-1].type!=')'&& tokens[i-1].type!=TK_NUM)){
          tokens[i].type=TK_MINUS;
        }
        break;
      case '*':
        if(i==0||(tokens[i-1].type!=')'&& tokens[i-1].type!=TK_NUM)){
          tokens[i].type=TK_DEREF;
        }
        break;
    }
  }

  *success=true;
  return eval(0,nr_token-1,success);
}
