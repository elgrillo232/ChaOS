#pragma once

#include "bootinfo.h"

// Creates a new identity-mapped page table (2MiB pages) and switches CR3.
// Current implementation supports mapping the first 4GiB.
EFI_STATUS paging_init_and_switch(const BOOT_INFO* boot_info);
