# ChaOS

A tiny hobby OS booting from a floppy image: 16-bit boot sector loads a 32-bit freestanding kernel and jumps into protected mode.

## Repo layout

- `boot/` — boot sector + helpers (disk load, GDT, protected-mode switch)
- `Kernel/` — 32-bit kernel entry + C kernel
- `build/` — build outputs (ignored by git)

## What it does (currently)

- Boots from a `.img` floppy disk image
- Loads the kernel at `0x1000`
- Switches to 32-bit protected mode
- Kernel writes an `X` to the top-left of the screen (VGA text memory `0xB8000`)

## Requirements

- NASM
- GCC (32-bit capable) + binutils
- QEMU (optional, for running)

## Build + run

This repo is set up to produce a 1.44MB floppy image at `build/chaos.img`.

If you have local scripts:

- Build: `build.bat` or `build.ps1`
- Run: `run.bat` or `run.ps1`

(These scripts are intentionally not committed; adjust to your local toolchain paths.)

## UEFI (x86_64) boot (experimental)

This repo also includes a minimal UEFI application at `uefi/bootx64.c`.

It builds a `BOOTX64.EFI` that should run on most x86_64 UEFI machines (typically with Secure Boot disabled), and in QEMU using EDK2/OVMF.

### Build `BOOTX64.EFI`

From the repo root:

- Compile: `gcc -c -O0 -ffreestanding -fno-stack-protector -fno-pic -mno-red-zone -fshort-wchar -I uefi -o build/uefi/bootx64.o uefi/bootx64.c`
- Link: `ld -m i386pep -nostdlib -shared -Bsymbolic --subsystem 10 --entry efi_main --image-base 0 -o build/uefi/BOOTX64.EFI build/uefi/bootx64.o`

### Run in QEMU (UEFI)

Create a folder that will be exposed as a FAT drive:

- `mkdir build/uefi_disk/EFI/BOOT`
- Copy `build/uefi/BOOTX64.EFI` to `build/uefi_disk/EFI/BOOT/BOOTX64.EFI`

Then run QEMU with the x86_64 EDK2 firmware (path may vary by install):

- `"C:\Program Files\qemu\qemu-system-x86_64.exe" -machine q35 -m 256M -drive if=pflash,format=raw,readonly=on,file="C:\Program Files\qemu\share\edk2-x86_64-code.fd" -drive file=fat:rw:build/uefi_disk,format=raw -serial stdio`

You should see: `ChaOS UEFI boot OK`.

## Notes

- VGA text mode is memory-mapped at `0xB8000` and uses 2 bytes per cell (character + attribute).
- The boot sector uses BIOS `int 13h` to read sectors starting from sector 2.
