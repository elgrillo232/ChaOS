#include "kernel.h"
#include "console.h"
#include "bootinfo.h"
#include "serial.h"
#include "pmm.h"

#include <stdint.h>

static void halt_forever(void) {
    __asm__ __volatile__("cli");
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}

static void serial_write_hex64(uint64_t v) {
    static const char* kHex = "0123456789ABCDEF";
    char buf[2];
    buf[1] = '\0';
    serial_write("0x");
    for (int i = 60; i >= 0; i -= 4) {
        buf[0] = kHex[(v >> i) & 0xFULL];
        serial_write(buf);
    }
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
        halt_forever();
    }

    serial_writeln("[ChaOS] Boot services are gone; kernel is now in control.");
    // Do not call UEFI console services after ExitBootServices.

    pmm_init(&boot_info);
    serial_write("[ChaOS] PMM free pages: ");
    serial_write_hex64((uint64_t)pmm_free_page_count());
    serial_writeln("");

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
