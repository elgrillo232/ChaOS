#include "bootinfo.h"

static EFI_STATUS get_memory_map(
    EFI_BOOT_SERVICES* bs,
    EFI_MEMORY_DESCRIPTOR** out_map,
    UINTN* out_map_size,
    UINTN* out_map_key,
    UINTN* out_desc_size,
    UINT32* out_desc_ver
) {
    if (!bs || !bs->GetMemoryMap || !bs->AllocatePool || !bs->FreePool) {
        return EFI_INVALID_PARAMETER;
    }

    UINTN map_size = 0;
    UINTN map_key = 0;
    UINTN desc_size = 0;
    UINT32 desc_ver = 0;

    EFI_STATUS st = bs->GetMemoryMap(&map_size, 0, &map_key, &desc_size, &desc_ver);
    if (st != EFI_BUFFER_TOO_SMALL) {
        return st;
    }

    if (desc_size == 0) {
        return EFI_INVALID_PARAMETER;
    }

    map_size += 2 * desc_size;

    EFI_MEMORY_DESCRIPTOR* map = 0;
    st = bs->AllocatePool(EfiLoaderData, map_size, (void**)&map);
    if (EFI_ERROR(st) || !map) {
        return EFI_OUT_OF_RESOURCES;
    }

    st = bs->GetMemoryMap(&map_size, map, &map_key, &desc_size, &desc_ver);
    if (EFI_ERROR(st)) {
        bs->FreePool(map);
        return st;
    }

    *out_map = map;
    *out_map_size = map_size;
    *out_map_key = map_key;
    *out_desc_size = desc_size;
    *out_desc_ver = desc_ver;
    return EFI_SUCCESS;
}

EFI_STATUS uefi_exit_boot_services(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table, BOOT_INFO* out) {
    if (!system_table || !system_table->BootServices || !out) {
        return EFI_INVALID_PARAMETER;
    }

    EFI_BOOT_SERVICES* bs = system_table->BootServices;
    if (!bs->ExitBootServices) {
        return EFI_INVALID_PARAMETER;
    }

    for (int attempt = 0; attempt < 8; attempt++) {
        EFI_MEMORY_DESCRIPTOR* map = 0;
        UINTN map_size = 0;
        UINTN map_key = 0;
        UINTN desc_size = 0;
        UINT32 desc_ver = 0;

        EFI_STATUS st = get_memory_map(bs, &map, &map_size, &map_key, &desc_size, &desc_ver);
        if (EFI_ERROR(st)) {
            return st;
        }

        st = bs->ExitBootServices(image_handle, map_key);
        if (st == EFI_SUCCESS) {
            out->memory_map = map;
            out->memory_map_size = map_size;
            out->memory_map_key = map_key;
            out->descriptor_size = desc_size;
            out->descriptor_version = desc_ver;
            return EFI_SUCCESS;
        }

        if (st == EFI_INVALID_PARAMETER) {
            bs->FreePool(map);
            continue;
        }

        bs->FreePool(map);
        return st;
    }

    return EFI_INVALID_PARAMETER;
}
