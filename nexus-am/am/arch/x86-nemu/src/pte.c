#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
  PDE *pdir=p->ptr;
  PDE *pde=&pdir[PDX(va)];

  if(!(*pde & PTE_P)){
    PTE *pg=(PTE *)palloc_f();
    for (int i = 0; i < NR_PTE; i++) pg[i] = 0;
    *pde=PTE_ADDR(pg) | PTE_P;
  }

  PTE *ptab= (PTE*)PTE_ADDR(*pde);
  PTE *pte=&ptab[PTX(va)];
  *pte=PTE_ADDR(pa) | PTE_P;
}

void _unmap(_Protect *p, void *va) {
}

// ustack的底部初始化一个以entry为返回地址的陷阱帧.
// 我们还需要在陷阱帧之前设置好_start()函数的栈帧,
// 这是为了_start()开始执行的时候, 可以访问到正确的栈帧.
// 我们只需要把这一栈帧中的参数设置为0或NULL即可, 至于返回地址,
// 我们永远不会从_start()返回, 因此可以不设置它.

// |               |
// +---------------+ <---- ustack.end
// |  stack frame  |
// |   of _start() |
// +---------------+
// |               |
// |   trap frame  |
// |               |
// +---------------+ <--+
// |               |    |
// |               |    |
// |               |    |
// |               |    |
// +---------------+    |
// |       tf      | ---+
// +---------------+ <---- ustack.start
// |               |

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {
  _RegSet *tf = *(_RegSet **)(ustack.start);

  // stack frame of _start() 设置三个空参数，忽略返回值
  uint32_t *stack = (uint32_t *)(ustack.end - 4 - 12);
  for (int i = 0; i < 3; i++) stack[i]=0;

  // 设置tf
  tf = (void *)(stack - sizeof(_RegSet));
  tf->eflags = 0x2;
  tf->cs = 8;
  tf->eip = (uintptr_t)entry;  // 为了保证differential testing的正确运行,

  return tf;
}
