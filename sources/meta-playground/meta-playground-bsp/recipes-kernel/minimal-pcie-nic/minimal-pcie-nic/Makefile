obj-m += minimal_pcie_nic_drv.o

# Default target: build module using KERNEL_SRC and KERNEL_BUILD provided by Yocto
all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
