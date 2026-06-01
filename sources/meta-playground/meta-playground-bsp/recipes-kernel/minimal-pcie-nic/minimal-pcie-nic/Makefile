obj-m += minimal_pcie_nic_drv.o

# Pick up extra flags injected by the Yocto recipe (e.g. -DTRACE_PCIE_NIC -DDEBUG)
ccflags-y += $(EXTRA_CFLAGS)

# Default target: build module using KERNEL_SRC and KERNEL_BUILD provided by Yocto
all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
