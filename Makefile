KDIR	     := /lib/modules/$(shell uname -r)/build
PWD	     := $(shell pwd)
ifneq ($(KERNELRELEASE),)

# NVIDIA_DRV_PATH=/usr/src/nvidia-340-340.29

obj-m   := fpu-mm.o

KBUILD_EXTRA_SYMBOLS := $(NVIDIA_DRV_PATH)/Module.symvers

# ccflags-y := -I$(src)/../include
# ccflags-y += -I$(NVIDIA_DRV_PATH)
# ccflags-y += -D_NVIDIA_RDMA_SUPPORT

 fpu-mm-y := hello-1.o
 fpu-mm-y += fpumm_chrdev.o 
# fpu-mm-y += fpumm-sgl.o

else

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	$(MAKE) -C ./test

clean:
	find . -name '*.cmd' -o -name '*.ko' -o -name '*.c~' -o -name '*.cpp~' -o -name '*.h~' -o -name '*.o' -o -name '*.mod.c' -o -name '*.symvers' -o -name '*.order' | xargs -n1 unlink
	@$(MAKE) -C ./test clean
endif


