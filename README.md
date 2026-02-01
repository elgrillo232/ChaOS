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

## Notes

- VGA text mode is memory-mapped at `0xB8000` and uses 2 bytes per cell (character + attribute).
- The boot sector uses BIOS `int 13h` to read sectors starting from sector 2.
