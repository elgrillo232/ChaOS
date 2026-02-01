$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Root = Split-Path -Parent $Root
Set-Location $Root

$BuildUefi = Join-Path $Root 'build\uefi'
$DiskRoot = Join-Path $Root 'build\uefi_disk'
$EfiOut = Join-Path $BuildUefi 'BOOTX64.EFI'

New-Item -ItemType Directory -Force -Path $BuildUefi | Out-Null

$CFlags = @(
  '-c','-O0',
  '-ffreestanding',
  '-fno-stack-protector',
  '-fno-pic',
  '-mno-red-zone',
  '-fshort-wchar',
  '-I','uefi\include'
)

function Compile($src, $obj) {
  Write-Host "`n> gcc $src"
  & gcc @CFlags -o $obj $src
  if ($LASTEXITCODE -ne 0) { throw "gcc failed" }
}

Compile 'uefi\src\efi_main.c' (Join-Path $BuildUefi 'efi_main.o')
Compile 'uefi\src\kernel.c' (Join-Path $BuildUefi 'kernel.o')
Compile 'uefi\src\console.c' (Join-Path $BuildUefi 'console.o')

Write-Host "`n> ld (BOOTX64.EFI)"
& ld -m i386pep -nostdlib -shared -Bsymbolic --subsystem 10 --entry efi_main --image-base 0 `
  -o $EfiOut `
  (Join-Path $BuildUefi 'efi_main.o') `
  (Join-Path $BuildUefi 'kernel.o') `
  (Join-Path $BuildUefi 'console.o')
if ($LASTEXITCODE -ne 0) { throw "ld failed" }

$BootPath = Join-Path $DiskRoot 'EFI\BOOT'
New-Item -ItemType Directory -Force -Path $BootPath | Out-Null
Copy-Item -Force $EfiOut (Join-Path $BootPath 'BOOTX64.EFI')

Write-Host "`nBuilt: $EfiOut"
Write-Host "Staged: $(Join-Path $BootPath 'BOOTX64.EFI')"
