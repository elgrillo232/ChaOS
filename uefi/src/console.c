#include "console.h"

static EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* g_con_out = 0;

void console_init(EFI_SYSTEM_TABLE* system_table) {
    g_con_out = (system_table && system_table->ConOut) ? system_table->ConOut : 0;
}

void console_write_utf16(const CHAR16* s) {
    if (!g_con_out || !g_con_out->OutputString || !s) {
        return;
    }
    g_con_out->OutputString(g_con_out, s);
}

static void ascii_to_utf16(const char* in, CHAR16* out, UINTN out_chars) {
    if (!in || !out || out_chars == 0) {
        return;
    }

    UINTN i = 0;
    for (; i + 1 < out_chars && in[i] != '\0'; i++) {
        out[i] = (unsigned char)in[i];
    }
    out[i] = 0;
}

void console_write_ascii(const char* s) {
    CHAR16 buf[256];
    ascii_to_utf16(s, buf, (UINTN)(sizeof(buf) / sizeof(buf[0])));
    console_write_utf16(buf);
}

void console_writeln_ascii(const char* s) {
    console_write_ascii(s);
    console_write_utf16(L"\r\n");
}
