#include "panic.h"

#include "serial.h"

__attribute__((noreturn)) static void halt_forever(void) {
    __asm__ __volatile__("cli");
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}

static void serial_write_dec_u64(uint64_t v) {
    char buf[32];
    int i = 0;

    if (v == 0) {
        serial_write("0");
        return;
    }

    while (v != 0 && i < (int)(sizeof(buf) - 1)) {
        uint64_t q = v / 10;
        uint64_t r = v - q * 10;
        buf[i++] = (char)('0' + (char)r);
        v = q;
    }
    buf[i] = '\0';

    // reverse
    for (int a = 0, b = i - 1; a < b; a++, b--) {
        char t = buf[a];
        buf[a] = buf[b];
        buf[b] = t;
    }
    serial_write(buf);
}

__attribute__((noreturn)) void panic_at(const char* msg, const char* file, int line, const char* func) {
    serial_writeln("\r\n[ChaOS] PANIC");
    if (msg) {
        serial_write("msg: ");
        serial_writeln(msg);
    }
    if (file) {
        serial_write("at: ");
        serial_write(file);
        serial_write(":");
        serial_write_dec_u64((uint64_t)(uint32_t)line);
        if (func) {
            serial_write(" (");
            serial_write(func);
            serial_write(")");
        }
        serial_writeln("");
    }

    halt_forever();
}
