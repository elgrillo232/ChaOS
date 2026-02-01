#include "kernel.h"
#include "console.h"
#include "bootinfo.h"
#include "serial.h"

static void halt_forever(void) {
    __asm__ __volatile__("cli");
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}

void kernel_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
    console_init(system_table);

    console_writeln_ascii("ChaOS (UEFI)");
    console_writeln_ascii("Phase 1: collecting memory map + exiting boot services...");

    BOOT_INFO boot_info;
    EFI_STATUS st = uefi_exit_boot_services(image_handle, system_table, &boot_info);

    serial_init();
    if (st == EFI_SUCCESS) {
        serial_writeln("[ChaOS] ExitBootServices: OK");
    } else {
        serial_writeln("[ChaOS] ExitBootServices: FAILED");
        halt_forever();
    }

    serial_writeln("[ChaOS] Boot services are gone; kernel is now in control.");
    (void)boot_info;

    halt_forever();
}
