#include "dicmm.h"

//#define pr_fmt(fmt) "%s:%d:: " fmt, __func__, __LINE__

/*  
 *  hello-1.c - Простейший модуль ядра.
 */
#include <linux/module.h>       /* Необходим для любого модуля ядра */
#include <linux/kernel.h>       /* Здесь находится определение KERN_ALERT */

// #include <linux/module.h>       /* Необходим для любого модуля ядра */
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include "fpumm_chrdev.h"

#define DRIVER_NAME "dic334"
#define DEBUG_ON

#include "log.h"

#define DIC_DEV_NAME "dic334"

#define VM_IO           0x00004000      /* Memory mapped I/O or similar */
#define VM_RESERVED     0x00080000      /* Count as reserved_vm like IO */

#define SumChannel  16
#define FR0                     0x10     // Фронт событий по входам IN[ 7:0]  0>1
#define FR1                     0x80     // Фронт событий по входам IN[15:8]  1>0


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fastwel");

static int __init 	init(void);
static void __exit 	cleanup(void);

static int  		fpumm_init(struct fpumm_t *fpu, struct pci_dev *dev);

static void 		fpumm_release(struct fpumm_t *fpu);

static irqreturn_t	fpu_irq(int irq, void *data);

static int 		fpu_map_bar(struct fpumm_t *fpu, int bar_num, bool wcflag);
static void 		fpu_unmap_bars(struct fpumm_t *fpu);

/////////////////////////////////////////////////////////////////////////////////////////
//
// PCIe calls
//

static struct {
	/* (mmio) control registers i.e. the "register memory region" */
	void __iomem *regs_base_addr;
	resource_size_t regs_start;
	resource_size_t regs_len;
	/* data mmio region */
	void __iomem *data_base_addr;
	resource_size_t data_mmio_start;
	resource_size_t data_mmio_len;
	/* irq handling */
	unsigned int irq;
} ivshmem_dev;


static struct pci_device_id dic334_pci_id[]={
  {PCI_DEVICE(0x1D8C, 0xD334)},
//  {PCI_DEVICE(0x10EE, 0x0505)},
  {0}
};

#define IVSHMEM_IRQ_ID "dic334_irq_id"

/*  relevant control register offsets */
enum {
	DebClk = 0x1,	/* Interrupt Mask */
	IntrStatus = 0x04,	/* Interrupt Status */
};

/*
static irqreturn_t ivshmem_interrupt(int irq, void *dev_id)
{
//	u32 status;

	LOG_FUNCTION;

	if (unlikely(strcmp((char *)dev_id, IVSHMEM_IRQ_ID)))
		return IRQ_NONE;

	outb(ivshmem_dev.regs_base_addr+13,0);
*/
/*	status = readl(ivshmem_dev.regs_base_addr + IntrStatus);
	if (!status || (status == 0xFFFFFFFF))
		return IRQ_NONE;

	pr_info_ratelimited("interrupt (status = 0x%04x)\n", status);
*/

// finalize_irq:
 	//notify user-irq waiters
//  	fpu->irq_cnt++;

//	return IRQ_HANDLED;
//}

static int 	dic334_probe(struct pci_dev *dev, const struct pci_device_id *pid);
static void 	dic334_remove(struct pci_dev *dev);

static struct pci_driver dic334_drv = 
{
  .name = DIC_DEV_NAME,
  .id_table = dic334_pci_id,
  .probe = dic334_probe,
  .remove = dic334_remove  
};

//
// END PCIe calls
//
/////////////////////////////////////////////////////////////////////////////////////////

/* ============================================================
 *                    THE FILE OPS
 * ============================================================ */
static int ivshmem_major;
#define IVSHMEM_MINOR 0

static int ivshmem_mmap(struct file *filp, struct vm_area_struct *vma)
{

	unsigned long len;
//	unsigned long off;
	unsigned long start;
	
//	lock_kernel();

        unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;

	start = ivshmem_dev.data_mmio_start;

	len=PAGE_ALIGN((start & ~PAGE_MASK) + ivshmem_dev.data_mmio_len);
	start &= PAGE_MASK;

	printk(KERN_INFO "%lu - %lu + %lu\n",vma->vm_end ,vma->vm_start, offset);
        printk(KERN_INFO "%lu > %lu\n",(vma->vm_end - vma->vm_start + offset), len);

/*        if((offset + (vma->vm_end - vma->vm_start)) > ivshmem_dev.data_mmio_len)
	{
 	    unlock_kernel();
            return -EINVAL;
	}
*/


	if ((vma->vm_end - vma->vm_start + offset) > len) {
//		unlock_kernel();
		return -EINVAL;
	}


        offset += (unsigned long)ivshmem_dev.data_mmio_start;

	vma->vm_pgoff = offset>> PAGE_SHIFT;

	vma->vm_flags |= VM_SHARED|VM_RESERVED;

      //  vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

        if(io_remap_pfn_range(vma, vma->vm_start,
                              offset >> PAGE_SHIFT,
                              vma->vm_end - vma->vm_start,
                              vma->vm_page_prot))
	{
		printk("mmap in kernl drv failed\n");
//		unlock_kernel();
                return -EAGAIN;
	}
	
//	unlock_kernel();

        return 0;
/*
	unsigned long len;
//	unsigned long off;
	unsigned long start;

	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;

	struct fpumm_t *fpu;
  
  	LOG_FUNCTION;
  
  	fpu = pci_get_drvdata(dev);

	start = fpu->bars[1].addr;

	len=PAGE_ALIGN((start & ~PAGE_MASK) + fpu->bars[1].size);
	start &= PAGE_MASK;

	printk(KERN_INFO "%lu - %lu + %lu\n",vma->vm_end ,vma->vm_start, offset);
        printk(KERN_INFO "%lu > %lu\n",(vma->vm_end - vma->vm_start + offset), len);

	if ((offset + (vma->vm_end - vma->vm_start)) >
	    fpu->bars[1].size)
		return -EINVAL;

	offset += (unsigned long)fpu->bars[1].addr;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	
	vma->vm_flags |= VM_SHARED|VM_RESERVED;

	if (io_remap_pfn_range(vma, vma->vm_start,
			       offset >> PAGE_SHIFT,
			       vma->vm_end - vma->vm_start, vma->vm_page_prot))
		return -EAGAIN;

	return 0;

*/
}

static int ivshmem_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "Opening kvm_ivshmem device\n");

	if (MINOR(inode->i_rdev) != IVSHMEM_MINOR) {
		pr_info("minor: %d\n", IVSHMEM_MINOR);
		return -ENODEV;
	}
	return 0;
}

static int ivshmem_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static const struct file_operations ivshmem_ops = {
	.owner = THIS_MODULE,
	.open = ivshmem_open,
	.mmap = ivshmem_mmap,
	.release = ivshmem_release,
};


/* ============================================================
 *                  MODULE INIT/EXIT
 * ============================================================ */
#define DIC_DEVS_NUM 1	/* number of devices */
static struct cdev cdev;	/* char device abstraction */
static struct class *fpumm_class;	/* linux device model */

module_exit(cleanup);
module_init(init);


static int __init 
init(void)
{
   int err =  -ENOMEM;

   dev_t mjr = MKDEV(ivshmem_major, 0);

   LOG_FUNCTION; 


   printk("<1>Hello world 1.\n");

   
   /* obtain major */
 //  dev_t mjr = MKDEV(ivshmem_major, 0);
   if ((err = alloc_chrdev_region(&mjr, 0, DIC_DEVS_NUM,
				 DIC_DEV_NAME)) < 0) 
   {
		pr_err("alloc_chrdev_region error\n");
		return err;
   }

   ivshmem_major = MAJOR(mjr);
   
   /* connect fops with the cdev */
	cdev_init(&cdev, &ivshmem_ops);
	cdev.owner = THIS_MODULE;

	/* connect major/minor to the cdev */
	{
		dev_t devt;
		devt = MKDEV(ivshmem_major, IVSHMEM_MINOR);

		if ((err = cdev_add(&cdev, devt, 1))) {
			pr_err("cdev_add error\n");
			goto unregister_dev;
		}
	}
  fpumm_class = class_create(THIS_MODULE, DIC_DEV_NAME);
  if(!fpumm_class)
  {
    LOG_CRIT("failed to create fpumm device class");
    goto del_cdev;
  }

   /* create udev '/dev' node */
        {
                dev_t devt = MKDEV(ivshmem_major, IVSHMEM_MINOR);   
                if(!(device_create(fpumm_class, NULL, devt, NULL, DIC_DEV_NAME"%d", IVSHMEM_MINOR))){
                        pr_err("device_create error\n");
                        goto destroy_class;
                }
        }   

 
  /* 
   * Если вернуть ненулевое значение, то это будет воспринято как признак ошибки,
   * возникшей в процессе работы init_module; в результате модуль не будет загружен. 
   */

//   err = fpumm_chrdev_init();

   err = pci_register_driver(&dic334_drv);
   if(err)
   {
     LOG_CRIT("failed to register pcie driver");
     goto unregister_dev;  //exit;
   }
   
   printk("<1>Hello world 1n.\n");

     
   return 0;

//exit1:
//  fpumm_chrdev_release();  
  
//exit:
 // return err;

exit:
        device_destroy(fpumm_class, MKDEV(ivshmem_major, IVSHMEM_MINOR));
destroy_class:
        class_destroy(fpumm_class);

del_cdev:
	cdev_del(&cdev);
unregister_dev:
	unregister_chrdev_region(MKDEV(ivshmem_major, IVSHMEM_MINOR),
				 DIC_DEVS_NUM);

  return err;

}


 
//void cleanup_module(void)
static void __exit 
cleanup(void)
{
  //      printk(KERN_ALERT "Goodbye world 1.\n");

	LOG_FUNCTION;
  
  	pci_unregister_driver(&dic334_drv);
	
	device_destroy(fpumm_class, MKDEV(ivshmem_major, IVSHMEM_MINOR));
        class_destroy(fpumm_class);

	cdev_del(&cdev);
	unregister_chrdev_region(MKDEV(ivshmem_major, IVSHMEM_MINOR),
				 DIC_DEVS_NUM); 
}


static int 	
dic334_probe(struct pci_dev *dev, const struct pci_device_id *pid)
{
  struct fpumm_t *fpu;
  int err = 0;

  struct uio_info *info;
  struct ivshmem_info *ivshmem_info;

  LOG_NOTICE("supported device: %04X-%04X [%02x:%02x.%x]", dev->vendor, dev->device, dev->bus->number, PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn));

  fpu = kzalloc(sizeof(struct fpumm_t), GFP_KERNEL);
  if(!fpu)
  {
    LOG_CRIT_DEV(dev, "failed to allocate memory for fpumm driver");
    return -ENOMEM;
  }

   err = fpumm_init(fpu, dev);
   if(err)
    goto exit;  
  
   pci_set_drvdata(dev, fpu);

 /*  if (!(ivshmem_info = kzalloc(sizeof(struct ivshmem_info), GFP_KERNEL))) {
		kfree(info);
		dev_err(&dev->dev, "kzalloc error!\n");
		return -ENOMEM;
   }

   */

   /* ivshmem bar0: uio mapping 0 */
//   info->mem[0].addr = fpu->bars[0].vaddr;
//   info->mem[0].memtype = UIO_NONE;
		

  printk("<1>Hello world 2 %04X-%04X.\n", dev->vendor, dev->device);

  return 0;

exit:
  kfree(fpu);
  return err;

}

static void 	
dic334_remove(struct pci_dev *dev)
{
  struct fpumm_t *fpu;
  
  LOG_FUNCTION;
  
  fpu = pci_get_drvdata(dev);
  
//  fpumm_chrdev_remove(fpu);
  
  //pci_free_consistent(fpu->dev, 0x4000, fpu->t, fpu->tdma);
  
  fpumm_release(fpu);
  
  kfree(fpu);
}

static int 
fpumm_init(struct fpumm_t *fpu, struct pci_dev *dev)
{
  int err;    
  unsigned char __iomem  *axibridge_bar;
  
  LOG_FUNCTION; 
   
  fpu->dev = dev;
  
  sprintf(fpu->name, "DIC334-%d", dev->devfn);
  
  err = pci_enable_device(dev);
  if(err)
  {
    LOG_CRIT_DEV(dev, "failed to enable device, error = %d", err);
    goto exit;
  }

  pci_set_master(dev);   
 
 /* err = pci_set_dma_mask(dev, DMA_BIT_MASK(64));
  if(err)
  {
    LOG_CRIT_DEV(dev, "no suitable DMA available");    
    goto exit1;
  }
  */

  err = fpu_map_bar(fpu, 0, false);
  if(err || !fpu->bars[0].size)
  {
    if(!err) 
      err = -ENODEV;
    LOG_CRIT_DEV(fpu->dev, "failed to remap BAR0");
    goto exit2;
  }
  
  //map usercore regs
  err = fpu_map_bar(fpu, 1, false);
  if(err || !fpu->bars[1].size)
  {
    if(!err) 
      err = -ENODEV;
    LOG_CRIT_DEV(fpu->dev, "failed to remap ");
    goto exit2;
  }

  err = pci_enable_msi(fpu->dev);
  if(err)
  {
    LOG_CRIT_DEV(fpu->dev, "failed to initialize msi");
    goto exit2;
  }

//	err = request_irq(17, fpu_irq, 0, fpu->name, fpu);
	err = request_irq(fpu->dev->irq, fpu_irq, 0, fpu->name, fpu);
	
//  pr_info("NUWBER IRQ: %#x\n", fpu->dev->irq);
  
 // err = request_irq(fpu->dev->irq,    /* Номер IRQ  */
//	 	ivshmem_interrupt,	/* наш обработчик */
// 		IRQF_SHARED, 
//		fpu->name, 
//		IVSHMEM_IRQ_ID);
  if(err)
  {
    LOG_CRIT_DEV(fpu->dev, "failed to get irq, error = %d", err);
    goto exit3;
  }
 
  fpu->irq_cnt = 0;

  return 0;

exit3:
  pci_disable_msi(fpu->dev);
  
exit2:
  fpu_unmap_bars(fpu);

exit1:
  pci_disable_device(dev);
  
exit:
  return err;
}


static void 
fpumm_release(struct fpumm_t *fpu)
{
  LOG_FUNCTION;
  
//  free_irq(fpu->dev->irq, IVSHMEM_IRQ_ID);
	
  free_irq(fpu->dev->irq, fpu);  

  pci_disable_msi(fpu->dev);

  outb(0,ivshmem_dev.regs_base_addr+13);
  
  outb(0,ivshmem_dev.regs_base_addr+5);
  
  fpu_unmap_bars(fpu);
  
  pci_disable_device(fpu->dev);
}

static int 
fpu_map_bar(struct fpumm_t *fpu, int bar_num, bool wcflag)
{
  int err =-ENODEV;
  unsigned long flag;
  
  unsigned long bar_addr;
  unsigned long bar_size;
  void * bar_vaddr;

  unsigned int val;
  
  LOG_FUNCTION;  
  
  if(pci_resource_len(fpu->dev, bar_num)==0) 
  {
    LOG_DEBUG_DEV(fpu->dev, "BAR%d is missing", bar_num);
    
    fpu->bars[bar_num].addr = 0;
    fpu->bars[bar_num].size = 0;
    fpu->bars[bar_num].vaddr = 0;
    
    return 0;
  }
  
  bar_addr = pci_resource_start(fpu->dev, bar_num);
  bar_size = pci_resource_len(fpu->dev, bar_num);
  bar_vaddr = NULL;
  
  flag = pci_resource_flags(fpu->dev,bar_num);
  
  if(flag & IORESOURCE_MEM)
  {    
    err = pci_request_region(fpu->dev, bar_num, fpu->name);
    if(err)
    {
      LOG_CRIT_DEV(fpu->dev, "failed to request pcie device regions");
      goto exit;
    }    
    
    if(wcflag)
      bar_vaddr = ioremap_wc(bar_addr, bar_size); 
    else
      bar_vaddr = ioremap_nocache(bar_addr, bar_size); 
    if(!bar_vaddr)
    {
      release_mem_region(bar_addr, bar_size);
      LOG_CRIT_DEV(fpu->dev, "failed to map mem region for BAR%d: addr 0x%lX, size %ld", bar_num, bar_addr, bar_size);
      goto exit1;
    }
    
    LOG_DEBUG_DEV(fpu->dev, "Memory resoures mapped: phys addr = 0x%lX, virt addr = 0x%lX, size = %ld", bar_addr, (unsigned long)bar_vaddr, bar_size);    

	
	ivshmem_dev.data_mmio_start = bar_addr;
        ivshmem_dev.data_mmio_len   = bar_size;
        ivshmem_dev.data_base_addr = pci_iomap(fpu->dev, bar_num, 0);
  }
  else
  {
    LOG_DEBUG_DEV(fpu->dev, "BAR%d is IO resource", bar_num);
    LOG_DEBUG_DEV(fpu->dev, "IO resoures mapped: phys addr = 0x%lX, virt addr = 0x%lX, size = %ld", bar_addr, (unsigned long)bar_vaddr, bar_size);

   err = pci_request_region(fpu->dev, bar_num, fpu->name);
    if(err)
    {
      LOG_CRIT_DEV(fpu->dev, "failed to request pcie device regions");
      goto exit;
    }

   release_region(bar_addr, bar_size);

   outb(0x3,bar_addr+0xA);

   val = inb(bar_addr + 0xE);

  //  void __iomem *regs = ioremap(bar_addr, 64);

  //_region(bar_addr, bar_size);

  //  
    pr_info("0xdead0000: %#x, DebClk: %#x \n", val,DebClk);

     ivshmem_dev.regs_start = bar_addr;
     ivshmem_dev.regs_len = bar_size;

//   DebClk = 1;  // 400ns
  // Установить время анти-дребезга, фронты срабатывания из 1 в 0
  // по всем входам и разрешить прерывания по событию любого входа
  outb(FR0|FR1|(DebClk<<2)|DebClk,bar_addr + 5);



  outb(0xFF,bar_addr+3); // Сброс событий EV[15:0]
  outb(0xFF,bar_addr+4); // Сброс событий EV[15:0]

//  outb(bar_addr+0xA,0x3);
  outb(0x55,bar_addr);

  outb(0x31,bar_addr+13);    



//    iounmap(regs);
  }
  
  fpu->bars[bar_num].addr = bar_addr;
  fpu->bars[bar_num].size = bar_size;
  fpu->bars[bar_num].vaddr = bar_vaddr;

  
    
  
  return 0;
exit1:
  pci_release_region(fpu->dev, bar_num);
exit:
  //ipopcie_unmap_bars(dev);  
  fpu_unmap_bars(fpu);
  
  return err;
}

static void 
fpu_unmap_bars(struct fpumm_t *fpu)
{
  int i = 0;
  
  LOG_FUNCTION;
  
  for(i=0;i<6;++i)
  {
    if(fpu->bars[i].size > 0)
    {
      iounmap((void*)fpu->bars[i].vaddr);

      LOG_DEBUG_DEV(fpu->dev, "Memory resoures released: phys addr = 0x%lX, virt addr = 0x%lX, size = %ld", fpu->bars[i].addr, (unsigned long)fpu->bars[i].vaddr, fpu->bars[i].size);
      
      fpu->bars[i].addr = 0;
      fpu->bars[i].size = 0;
      fpu->bars[i].vaddr = 0;
    }    
  }
  
  pci_release_regions(fpu->dev);
}

/*
void 
fpumm_chrdev_notify(struct fpumm_t *fpu)
{
  LOG_FUNCTION;
  
  wake_up_interruptible_all(&fpu->fpu_inst->irq_wait_queue);
}

*/

static irqreturn_t	
fpu_irq(int irq, void *data)
{
  u32 cdmasr;
  u16 w_data;

  struct fpumm_t *fpu = data;
  irqreturn_t ret = IRQ_NONE;
  struct fpumm_dma_t *pdma;
    
  //TODO: remove irq processing to tasklet
  
  LOG_FUNCTION;

  spin_lock(&fpu->irq_lock);

  outb(0,fpu->bars[0].addr+13);

  w_data = 0xFF&inb(fpu->bars[0].addr+3);    // Чтение регистра событий EV[15:0]

  outb(w_data,fpu->bars[0].addr+3);        // Сброс событий EV[7:0]

finalize_irq:
  //notify user-irq waiters
  fpu->irq_cnt++;
  fpumm_chrdev_notify(fpu); 
    
  spin_unlock(&fpu->irq_lock);

  outb(0x31,fpu->bars[0].addr+13);
  
  return ret;
}
