#include "paging.h"

#include "pmm.h"
#include "klog.h"
#include "kstring.h"

// x86_64 paging flags
#define PTE_P   (1ULL << 0)
#define PTE_RW  (1ULL << 1)
#define PTE_PS  (1ULL << 7)

static inline void write_cr3(uint64_t value) {
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(value) : "memory");
}

static uint64_t* alloc_pt_page(void) {
    EFI_PHYSICAL_ADDRESS phys = pmm_alloc_pages(1);
    if (!phys) {
        return 0;
    }
    uint64_t* v = (uint64_t*)(uintptr_t)phys;
    memset(v, 0, 4096);
    return v;
}

EFI_STATUS paging_init_and_switch(const BOOT_INFO* boot_info) {
    (void)boot_info;

    // We assume UEFI currently provides an identity map (function pointers == physical).
    // This kernel currently only supports running below 4GiB.
    uint64_t here = (uint64_t)(uintptr_t)&paging_init_and_switch;
    if (here >= 0x100000000ULL) {
        klog_writeln("[ChaOS] paging: kernel above 4GiB not supported yet");
        return EFI_UNSUPPORTED;
    }

    uint64_t* pml4 = alloc_pt_page();
    uint64_t* pdpt = alloc_pt_page();
    if (!pml4 || !pdpt) {
        return EFI_OUT_OF_RESOURCES;
    }

    // Map 0..4GiB identity using 2MiB pages.
    // Layout:
    //   PML4[0] -> PDPT
    //   PDPT[0..3] -> PD0..PD3
    pml4[0] = ((uint64_t)(uintptr_t)pdpt) | PTE_P | PTE_RW;

    for (UINTN gb = 0; gb < 4; gb++) {
        uint64_t* pd = alloc_pt_page();
        if (!pd) {
            return EFI_OUT_OF_RESOURCES;
        }

        pdpt[gb] = ((uint64_t)(uintptr_t)pd) | PTE_P | PTE_RW;

        // 512 entries * 2MiB = 1GiB
        uint64_t base = (uint64_t)gb * 0x40000000ULL;
        for (UINTN i = 0; i < 512; i++) {
            uint64_t phys = base + (uint64_t)i * 0x200000ULL;
            pd[i] = (phys & 0xFFFFFFFFFFE00000ULL) | PTE_P | PTE_RW | PTE_PS;
        }
    }

    // Switch to our new page tables.
    __asm__ __volatile__("cli" : : : "memory");
    write_cr3((uint64_t)(uintptr_t)pml4);
    __asm__ __volatile__("sti" : : : "memory");

    klog_writeln("[ChaOS] paging: switched to kernel page tables (identity 0-4GiB)");
    return EFI_SUCCESS;
}
