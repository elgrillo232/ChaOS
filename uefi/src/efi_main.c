#include "uefi.h"
#include "kernel.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
    kernel_main(image_handle, system_table);
    return EFI_SUCCESS;
}
