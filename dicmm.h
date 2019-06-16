#pragma once

#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/wait.h>
#include <linux/spinlock.h>

///////////////////////////////////////////////////////////////////////

#define DIC_TIMEOUT 10000 //milliseconds

struct fpumm_bar_t
{
  unsigned long addr;
  unsigned long size;
  unsigned char *vaddr;
};

struct fpumm_t{
  struct pci_dev *dev;
  struct fpumm_bar_t bars[6];

//  bool	dma_active;
//  struct fpumm_dma_t *pdma;
    
  char name[64];  
  
  u32 __iomem *cdmacr;
  u32 __iomem *cdmasr;
  
  u32 __iomem *sa;
  u32 __iomem *da;
  u32 __iomem *btt;  
  
  u8 __iomem *ram;
 // dma_addr_t ram_dma;
  unsigned long ram_size;
  
  u8 __iomem *ucore;
 // dma_addr_t ucore_dma;
  unsigned long ucore_size;
  
  u32 __iomem *axibar2pciebar_0u;
  u32 __iomem *axibar2pciebar_0l;
  u32 __iomem *axibar2pciebar_1u;
  u32 __iomem *axibar2pciebar_1l;
  
  //dma_addr_t tdma;
  u8 *t;
  
  unsigned long irq_cnt;
  
  spinlock_t irq_lock;
  
  struct fpumm_instance_t *fpu_inst;
};


//
// DIC334 MEMORY-MAP DRIVER
//
///////////////////////////////////////////////////////////////////////
