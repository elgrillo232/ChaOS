#pragma once

#include "bootinfo.h"

// Physical memory manager (page-frame allocator)
// - Tracks the first 4GiB of physical memory using a static bitmap.
// - Marks usable pages based on UEFI memory map.
// - Allocates contiguous pages (first-fit).

#define PMM_PAGE_SIZE 4096ULL
#define PMM_TRACKED_BYTES (4ULL * 1024ULL * 1024ULL * 1024ULL)
#define PMM_TRACKED_PAGES (PMM_TRACKED_BYTES / PMM_PAGE_SIZE)

void pmm_init(const BOOT_INFO* boot_info);

EFI_PHYSICAL_ADDRESS pmm_alloc_pages(UINTN pages);
void pmm_free_pages(EFI_PHYSICAL_ADDRESS addr, UINTN pages);

UINTN pmm_free_page_count(void);
