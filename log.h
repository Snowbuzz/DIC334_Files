#ifndef _LOG_H_D_
#define _LOG_H_D_

#include "linux/kernel.h"

#define LOG_EMERG(format, args...) printk(KERN_EMERG DRIVER_NAME  ": " format "\n", ##args)
#define LOG_ALERT(format, args...) printk(KERN_ALERT DRIVER_NAME ": " format "\n", ##args)
#define LOG_CRIT(format, args...) printk(KERN_CRIT DRIVER_NAME ": " format "\n", ##args)
#define LOG_ERR(format, args...) printk(KERN_ERR DRIVER_NAME ": " format "\n", ##args)
#define LOG_WARN(format, args...) printk(KERN_WARNING DRIVER_NAME ": " format "\n", ##args)
#define LOG_NOTICE(format, args...) printk(KERN_NOTICE DRIVER_NAME ": " format "\n", ##args)

#define LOG_EMERG_DEV(d, format, args...) dev_printk(KERN_EMERG, &((d)->dev), format,  ##args);
#define LOG_ALERT_DEV(d, format, args...) dev_printk(KERN_ALERT, &((d)->dev), format,  ##args);
#define LOG_CRIT_DEV(d, format, args...) dev_printk(KERN_CRIT, &((d)->dev), format,  ##args);
#define LOG_ERR_DEV(d, format, args...) dev_printk(KERN_ERR, &((d)->dev), format,  ##args);
#define LOG_WARN_DEV(d, format, args...) dev_printk(KERN_WARNING, &((d)->dev), format,  ##args);
#define LOG_NOTICE_DEV(d, format, args...) dev_printk(KERN_NOTICE, &((d)->dev), format,  ##args);

#ifdef DEBUG_ON
#define LOG_INFO(format, args...) printk(KERN_INFO DRIVER_NAME " INFO: " format "\n", ##args)
#define LOG_DEBUG(format, args...) printk(KERN_DEBUG DRIVER_NAME " DEBUG: " format "\n", ##args)
#define LOG_DEBUG_DEV(d, format, args...) dev_printk(KERN_DEBUG, &((d)->dev), format,  ##args);
#define LOG_INFO_DEV(d, format, args...) dev_printk(KERN_INFO, &((d)->dev), format,  ##args);

#define LOG_FUNCTION LOG_DEBUG("entering function: %s", __FUNCTION__)

#else
#define LOG_INFO(format, args...) 
#define LOG_DEBUG(format, args...) 
#define LOG_DEBUG_DEV(dev, format, args...) 
#define LOG_INFO_DEV(dev, format, args...) 
#define LOG_FUNCTION 

#endif
 
#endif




