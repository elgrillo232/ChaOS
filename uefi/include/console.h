#pragma once

#include "uefi.h"

void console_init(EFI_SYSTEM_TABLE* system_table);
void console_write_utf16(const CHAR16* s);
void console_write_ascii(const char* s);
void console_writeln_ascii(const char* s);
