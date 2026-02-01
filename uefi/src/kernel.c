#include "kernel.h"
#include "console.h"

void kernel_main(EFI_SYSTEM_TABLE* system_table) {
    console_init(system_table);

    console_writeln_ascii("ChaOS (UEFI)");
    console_writeln_ascii("Kernel + loader are the same binary (BOOTX64.EFI)");
    console_writeln_ascii("Next: implement ExitBootServices + a real kernel loader");

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
