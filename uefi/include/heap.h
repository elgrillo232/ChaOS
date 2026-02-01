#pragma once

#include "uefi.h"

// Simple kernel heap (malloc/free) built on top of the physical page allocator.
// - Intended for early single-core bringup.
// - Uses identity-mapped pages from the PMM (virtual == physical for now).

void heap_init(void);

void* kmalloc(UINTN size);
void kfree(void* ptr);
