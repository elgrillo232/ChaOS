#include "uefi.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
    (void)image_handle;

    if (system_table && system_table->ConOut && system_table->ConOut->OutputString) {
        static const CHAR16 msg[] = L"ChaOS UEFI boot OK\r\n";
        system_table->ConOut->OutputString(system_table->ConOut, msg);
    }

    for (;;) {
        __asm__ __volatile__("hlt");
    }

    return EFI_SUCCESS;
}
