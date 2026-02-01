#include "serial.h"

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static int serial_tx_ready(void) {
    return (inb(0x3F8 + 5) & 0x20) != 0;
}

void serial_init(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x01);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

static void serial_putc(char c) {
    while (!serial_tx_ready()) {
        __asm__ __volatile__("pause");
    }
    outb(0x3F8, (uint8_t)c);
}

void serial_write(const char* s) {
    if (!s) {
        return;
    }

    for (const char* p = s; *p; p++) {
        if (*p == '\n') {
            serial_putc('\r');
        }
        serial_putc(*p);
    }
}

void serial_writeln(const char* s) {
    serial_write(s);
    serial_write("\r\n");
}
