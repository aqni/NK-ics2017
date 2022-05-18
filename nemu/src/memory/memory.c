#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int mmio_no = is_mmio(addr);
  if (mmio_no != -1) {
    return mmio_read(addr, len, mmio_no);
  } else {
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int mmio_no = is_mmio(addr);
  if (mmio_no != -1) {
    mmio_write(addr, len, data, mmio_no);
  } else {
    memcpy(guest_to_host(addr), &data, len);
  }
}

static inline bool cross_page(vaddr_t addr, int len){
  vaddr_t startpg=addr & ~PAGE_MASK;
  vaddr_t endpg=(addr + len - 1) & ~PAGE_MASK;
  return startpg != endpg;
}

paddr_t page_translate(vaddr_t addr,bool write) {
  /* if 没有开启分页机制 */
  if(!(cpu.cr0.paging)){
    return addr;
  }

  Log("cpu.cr0:%x",cpu.cr0.val);

  Log("access va:%x",addr);
  
  /* 访问页目录表 */
  PDE *pdir=(PDE*)(cpu.cr3.page_directory_base << 12);
  PDE pde;
  intptr_t ppde=(intptr_t)&pdir[addr >> 22];
  pde.val=paddr_read(ppde,sizeof(PDE));
  assert(pde.present);
  pde.accessed=true;
  
  /* 访问页表 */
  PTE *ptab=(PTE*)(pde.page_frame << 12);
  PTE pte;
  intptr_t ppte=(intptr_t)&ptab[(addr<<10) >> 12];
  pte.val=paddr_read(ppte, sizeof(PTE));
  assert(pte.present);
  pte.accessed = true;
  if(write) pte.dirty=true;

  /* 写回页目录、页表 */
  paddr_write(ppde,sizeof(PDE),pde.val);
  paddr_write(ppte,sizeof(PTE),pte.val);

  intptr_t paddr= (pte.page_frame << 12) | (addr & PAGE_MASK);
  return paddr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
    if (cross_page(addr,len)) {
        /* this is a special case, you can handle it later. */
        Assert(0,"vaddr_read cross_page");
    }
    else {
        paddr_t paddr = page_translate(addr,false);
        return paddr_read(paddr, len);
    }
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if (cross_page(addr, len)) {
    /* data cross the page boundary */
    Assert(0,"vaddr_write cross_page");
  } else {
    paddr_t paddr = page_translate(addr,true);
    paddr_write(paddr, len, data);
  }
}
