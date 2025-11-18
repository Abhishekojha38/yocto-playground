# Doby PCI Device for QEMU
A minimal custom PCI device for experimenting with QEMU PCI internals, MMIO
access, and writing Linux drivers.

## Overview
`doby-pci-dev` is a simple, test-only PCI device added to QEMU. It helps in
learning and validating:
- PCI enumeration  
- BAR allocation  
- MMIO register access  
- QEMU device modeling  
- Linux PCI driver development  

The device exposes:
- **Vendor ID:** PCI_VENDOR_ID_QEMU  
- **Device ID:** 0x5678  
- **Revision:** 0x01  
- **MMIO BAR:** 4 KB  
- **Register:** `data_reg` (32-bit)  
All reads and writes print to the QEMU console.

## Architecture
```
QEMU Machine
   |
PCI Bus
   |
+------------------+
| doby-pci-dev     |
+------------------+
   |
MMIO BAR (0x1000)
   |
data_reg (32-bit)
```

## Register Map
| Offset | Name      | Size | Description                     |
|--------|-----------|------|---------------------------------|
| 0x00   | data_reg  | 4 B  | R/W register, prints to console |


## Running QEMU
```
runqemu playground-arm64 nographic slirp qemuparams="-device doby-pci-dev"
```

## Checking the Device in Linux
```
lspci -nn | grep 1234
```
Expected:
```
root@playground-arm64:~# lspci -nn | grep 1234
00:05.0 Unclassified device [00ff]: Device [1234:5678] (rev 01)
```
Detailed info:
```
root@playground-arm64:~# lspci -vv -s 00:05.0
00:05.0 Unclassified device [00ff]: Device 1234:5678 (rev 01)
        Subsystem: Red Hat, Inc. Device 1100
        Control: I/O- Mem- BusMaster- SpecCycle- MemWINV- VGASnoop- ParErr- Stepping- SERR- FastB2B- DisINTx-
        Status: Cap- 66MHz- UDF- FastB2B- ParErr- DEVSEL=fast >TAbort- <TAbort- <MAbort- >SERR- <PERR- INTx-
        Region 0: Memory at 10043000 (32-bit, non-prefetchable) [disabled] [size=4K]

root@playground-arm64:~#
```

## Credits
Developed by **Abhishek Ojha**  
Purpose: PCI learning, QEMU device modeling, Yocto integration, and driver testing.
