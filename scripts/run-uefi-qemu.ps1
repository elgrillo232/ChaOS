$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Root = Split-Path -Parent $Root

$QemuDir = 'C:\Program Files\qemu'
$Qemu = Join-Path $QemuDir 'qemu-system-x86_64.exe'
$Firmware = Join-Path $QemuDir 'share\edk2-x86_64-code.fd'
$DiskRoot = Join-Path $Root 'build\uefi_disk'

if (-not (Test-Path $Qemu)) { throw "QEMU not found: $Qemu" }
if (-not (Test-Path $Firmware)) { throw "EDK2 firmware not found: $Firmware" }

# Build if missing
if (-not (Test-Path (Join-Path $DiskRoot 'EFI\BOOT\BOOTX64.EFI'))) {
  & (Join-Path $Root 'scripts\build-uefi.ps1')
}

Write-Host "Booting UEFI from: $DiskRoot"
& $Qemu -machine q35 -m 256M `
  -drive "if=pflash,format=raw,readonly=on,file=$Firmware" `
  -drive "file=fat:rw:$DiskRoot,format=raw" `
  -serial stdio
