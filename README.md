# ChaOS

A tiny hobby OS project focused on modern UEFI boot.

## Repo layout

- `uefi/` — the UEFI “kernel” (a `BOOTX64.EFI` UEFI application)
- `scripts/` — build/run helpers for UEFI
- `archive/bios/` — legacy BIOS boot sector + 32-bit kernel (archived)
- `build/` — build outputs (ignored by git)

## What it does (currently)

- Builds a `BOOTX64.EFI` and boots it under UEFI
- Prints a few lines via UEFI console output

## Requirements

- MSYS2 MinGW64 toolchain (`gcc`, `ld`)
- QEMU (for running) + EDK2 firmware (bundled in the Windows QEMU install)

## Build + run (UEFI)

### Build

- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-uefi.ps1`

This produces `build/uefi/BOOTX64.EFI` and stages it at `build/uefi_disk/EFI/BOOT/BOOTX64.EFI`.

### Run (QEMU)

- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run-uefi-qemu.ps1`

## Notes

- The UEFI build is a single binary: the “loader” and “kernel” are the same `BOOTX64.EFI`.
- The archived BIOS version remains in `archive/bios/` for reference.
