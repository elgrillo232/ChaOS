#pragma once

#include "uefi.h"

typedef struct {
    EFI_MEMORY_DESCRIPTOR* memory_map;
    UINTN memory_map_size;
    UINTN memory_map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;
} BOOT_INFO;

EFI_STATUS uefi_exit_boot_services(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table, BOOT_INFO* out);
