#include "uefi.h"
#include "kernel.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
    (void)image_handle;

    kernel_main(system_table);
    return EFI_SUCCESS;
}
