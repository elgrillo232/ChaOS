#include "pmm.h"

#include "kstring.h"

#include <stdint.h>

#define BITMAP_BYTES (PMM_TRACKED_PAGES / 8ULL)

// 1 = used/reserved, 0 = free
static uint8_t g_bitmap[BITMAP_BYTES];
static UINTN g_free_pages = 0;

static inline UINTN page_index(EFI_PHYSICAL_ADDRESS addr) {
    return (UINTN)(addr / PMM_PAGE_SIZE);
}

static inline void bit_set(UINTN idx) {
    g_bitmap[idx >> 3] |= (uint8_t)(1u << (idx & 7));
}

static inline void bit_clear(UINTN idx) {
    g_bitmap[idx >> 3] &= (uint8_t)~(1u << (idx & 7));
}

static inline int bit_test(UINTN idx) {
    return (g_bitmap[idx >> 3] >> (idx & 7)) & 1u;
}

static int is_usable_type(UINT32 type) {
    // Be conservative for now: after ExitBootServices, BootServices memory becomes available
    // to the OS, but we may still be running on firmware-created page tables that live in
    // that region. We'll start with conventional memory only.
    return type == EfiConventionalMemory;
}

static int is_bootservices_type(UINT32 type) {
    return type == EfiBootServicesCode || type == EfiBootServicesData;
}

static void mark_range_free(UINTN start_page, UINTN page_count) {
    if (page_count == 0) {
        return;
    }
    UINTN end_page = start_page + page_count;
    if (end_page > (UINTN)PMM_TRACKED_PAGES) {
        end_page = (UINTN)PMM_TRACKED_PAGES;
    }

    // Keep the first 1MiB reserved for safety (legacy, IVT/BDA, etc.)
    const UINTN min_free_page = (UINTN)(0x100000ULL / PMM_PAGE_SIZE);
    if (end_page <= min_free_page) {
        return;
    }
    if (start_page < min_free_page) {
        start_page = min_free_page;
    }

    for (UINTN i = start_page; i < end_page; i++) {
        if (bit_test(i)) {
            bit_clear(i);
            g_free_pages++;
        }
    }
}

static void mark_range_used(UINTN start_page, UINTN page_count) {
    if (page_count == 0) {
        return;
    }
    UINTN end_page = start_page + page_count;
    if (end_page > (UINTN)PMM_TRACKED_PAGES) {
        end_page = (UINTN)PMM_TRACKED_PAGES;
    }

    for (UINTN i = start_page; i < end_page; i++) {
        if (!bit_test(i)) {
            bit_set(i);
            if (g_free_pages) {
                g_free_pages--;
            }
        }
    }
}

void pmm_init(const BOOT_INFO* boot_info) {
    // Start with everything reserved.
    memset(g_bitmap, 0xFF, (size_t)BITMAP_BYTES);
    g_free_pages = 0;

    if (!boot_info || !boot_info->memory_map || boot_info->descriptor_size == 0) {
        return;
    }

    const uint8_t* base = (const uint8_t*)boot_info->memory_map;
    for (UINTN off = 0; off + boot_info->descriptor_size <= boot_info->memory_map_size; off += boot_info->descriptor_size) {
        const EFI_MEMORY_DESCRIPTOR* d = (const EFI_MEMORY_DESCRIPTOR*)(base + off);
        if (!is_usable_type(d->Type)) {
            continue;
        }

        EFI_PHYSICAL_ADDRESS phys = d->PhysicalStart;
        UINT64 pages = d->NumberOfPages;
        if (pages == 0) {
            continue;
        }

        UINTN start = page_index(phys);
        mark_range_free(start, (UINTN)pages);
    }
}

void pmm_add_uefi_bootservices(const BOOT_INFO* boot_info) {
    if (!boot_info || !boot_info->memory_map || boot_info->descriptor_size == 0) {
        return;
    }

    const uint8_t* base = (const uint8_t*)boot_info->memory_map;
    for (UINTN off = 0; off + boot_info->descriptor_size <= boot_info->memory_map_size; off += boot_info->descriptor_size) {
        const EFI_MEMORY_DESCRIPTOR* d = (const EFI_MEMORY_DESCRIPTOR*)(base + off);
        if (!is_bootservices_type(d->Type)) {
            continue;
        }

        EFI_PHYSICAL_ADDRESS phys = d->PhysicalStart;
        UINT64 pages = d->NumberOfPages;
        if (pages == 0) {
            continue;
        }

        UINTN start = page_index(phys);
        mark_range_free(start, (UINTN)pages);
    }
}

EFI_PHYSICAL_ADDRESS pmm_alloc_pages(UINTN pages) {
    if (pages == 0) {
        return 0;
    }

    UINTN run_start = 0;
    UINTN run_len = 0;

    for (UINTN i = 0; i < (UINTN)PMM_TRACKED_PAGES; i++) {
        if (!bit_test(i)) {
            if (run_len == 0) {
                run_start = i;
            }
            run_len++;
            if (run_len == pages) {
                mark_range_used(run_start, pages);
                return (EFI_PHYSICAL_ADDRESS)run_start * PMM_PAGE_SIZE;
            }
        } else {
            run_len = 0;
        }
    }

    return 0;
}

void pmm_free_pages(EFI_PHYSICAL_ADDRESS addr, UINTN pages) {
    if (pages == 0 || addr == 0) {
        return;
    }

    UINTN start = page_index(addr);
    mark_range_free(start, pages);
}

UINTN pmm_free_page_count(void) {
    return g_free_pages;
}
