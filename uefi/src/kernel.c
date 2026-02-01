#include "kernel.h"
#include "console.h"
#include "bootinfo.h"
#include "serial.h"
#include "pmm.h"
#include "idt.h"
#include "paging.h"
#include "heap.h"
#include "panic.h"


static void halt_forever(void) {
    __asm__ __volatile__("cli");
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}

static inline uint64_t read_cr2(void) {
    uint64_t v;
    __asm__ __volatile__("mov %%cr2, %0" : "=r"(v));
    return v;
}

void isr_common_handler(isr_context_t* ctx) {
    serial_writeln("\r\n[ChaOS] EXCEPTION");
    serial_write("vector=");
    serial_write_hex64(ctx ? ctx->vector : 0);
    serial_write(" error=");
    serial_write_hex64(ctx ? ctx->error : 0);
    serial_writeln("");
    serial_write("rip=");
    serial_write_hex64(ctx ? ctx->rip : 0);
    serial_write(" rflags=");
    serial_write_hex64(ctx ? ctx->rflags : 0);
    serial_writeln("");
    serial_write("cr2=");
    serial_write_hex64(read_cr2());
    serial_writeln("");

    halt_forever();
}

void kernel_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
    console_init(system_table);

    // Serial stays usable after ExitBootServices; init early so users know where logs go.
    serial_init();

    console_writeln_ascii("ChaOS (UEFI)");
    console_writeln_ascii("Phase 1: collecting memory map + exiting boot services...");
    console_writeln_ascii("Note: after ExitBootServices, logs go to SERIAL (QEMU terminal), not this window.");

    BOOT_INFO boot_info;
    EFI_STATUS st = uefi_exit_boot_services(image_handle, system_table, &boot_info);
    if (st == EFI_SUCCESS) {
        serial_writeln("[ChaOS] ExitBootServices: OK");
    } else {
        serial_writeln("[ChaOS] ExitBootServices: FAILED");
        // If ExitBootServices failed, the UEFI console should still be usable.
        console_writeln_ascii("ExitBootServices FAILED (see serial output)");
        PANIC("ExitBootServices failed");
    }

    serial_writeln("[ChaOS] Boot services are gone; kernel is now in control.");
    // Do not call UEFI console services after ExitBootServices.

    idt_init();

    pmm_init(&boot_info);
    serial_write("[ChaOS] PMM free pages: ");
    serial_write_hex64((uint64_t)pmm_free_page_count());
    serial_writeln("");

    ASSERT(pmm_free_page_count() != 0);

    EFI_STATUS pst = paging_init_and_switch(&boot_info);
    if (pst != EFI_SUCCESS) {
        serial_writeln("[ChaOS] paging: FAILED");
        PANIC("paging_init_and_switch failed");
    }

    pmm_add_uefi_bootservices(&boot_info);
    serial_write("[ChaOS] PMM free pages (after BootServices free): ");
    serial_write_hex64((uint64_t)pmm_free_page_count());
    serial_writeln("");

    ASSERT(pmm_free_page_count() != 0);

    heap_init();
    serial_writeln("[ChaOS] heap: initialized");

    void* h1 = kmalloc(64);
    void* h2 = kmalloc(4096);
    ASSERT(h1 != 0);
    ASSERT(h2 != 0);
    serial_write("[ChaOS] kmalloc 64 @ ");
    serial_write_hex64((uint64_t)(uintptr_t)h1);
    serial_writeln("");
    serial_write("[ChaOS] kmalloc 4096 @ ");
    serial_write_hex64((uint64_t)(uintptr_t)h2);
    serial_writeln("");
    kfree(h1);
    kfree(h2);
    serial_writeln("[ChaOS] kfree: OK");

    EFI_PHYSICAL_ADDRESS a = pmm_alloc_pages(1);
    EFI_PHYSICAL_ADDRESS b = pmm_alloc_pages(8);
    serial_write("[ChaOS] alloc 1 page @ ");
    serial_write_hex64((uint64_t)a);
    serial_writeln("");
    serial_write("[ChaOS] alloc 8 pages @ ");
    serial_write_hex64((uint64_t)b);
    serial_writeln("");

    halt_forever();
}
