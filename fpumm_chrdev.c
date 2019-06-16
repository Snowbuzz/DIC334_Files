#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/jiffies.h>
#include <asm/uaccess.h>

#include "fpumm_chrdev.h"

#define DRIVER_NAME "dicmm"

#include "log.h"

#define DIC334MM_COUNT 32

static dev_t dev_id;

int 
fpumm_chrdev_init(void)
{
  int err;
  int i = 0;
  
  LOG_FUNCTION;
  
  err = alloc_chrdev_region(&dev_id, 0, DIC334MM_COUNT, "dic");
  if(err)
  {
    LOG_CRIT("failed init chardev");
    return err;
  }

  return 0;

}

void 
fpumm_chrdev_release(void)
{
  LOG_FUNCTION;
  
//  class_destroy(fpumm_class);
  
  unregister_chrdev_region(dev_id, DIC334MM_COUNT);
}

void 
fpumm_chrdev_notify(struct fpumm_t *fpu)
{
  LOG_FUNCTION;
  
 // wake_up_interruptible_all(&fpu->fpu_inst->irq_wait_queue);
}

