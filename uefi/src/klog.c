#include "klog.h"

#include "serial.h"
#include "kstring.h"

static void putc(char c) {
    char buf[2];
    buf[0] = c;
    buf[1] = '\0';
    serial_write(buf);
}

void klog_write(const char* s) {
    serial_write(s);
}

void klog_writeln(const char* s) {
    serial_writeln(s);
}

void klog_hex64(uint64_t v) {
    serial_write_hex64(v);
}

void klog_dec_u64(uint64_t v) {
    char buf[32];
    size_t i = 0;

    if (v == 0) {
        serial_write("0");
        return;
    }

    while (v != 0 && i < sizeof(buf) - 1) {
        uint64_t q = v / 10;
        uint64_t r = v - q * 10;
        buf[i++] = (char)('0' + (char)r);
        v = q;
    }
    buf[i] = '\0';

    // reverse
    for (size_t a = 0, b = i ? (i - 1) : 0; a < b; a++, b--) {
        char t = buf[a];
        buf[a] = buf[b];
        buf[b] = t;
    }
    serial_write(buf);
}

static void print_int(int64_t v) {
    if (v < 0) {
        putc('-');
        klog_dec_u64((uint64_t)(-v));
    } else {
        klog_dec_u64((uint64_t)v);
    }
}

void vklogf(const char* fmt, va_list args) {
    if (!fmt) {
        return;
    }

    for (size_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] != '%') {
            putc(fmt[i]);
            continue;
        }

        i++;
        char spec = fmt[i];
        if (spec == '\0') {
            break;
        }

        switch (spec) {
            case '%':
                putc('%');
                break;
            case 's': {
                const char* s = va_arg(args, const char*);
                serial_write(s ? s : "(null)");
                break;
            }
            case 'c': {
                int c = va_arg(args, int);
                putc((char)c);
                break;
            }
            case 'u': {
                uint64_t u = (uint64_t)va_arg(args, unsigned long long);
                klog_dec_u64(u);
                break;
            }
            case 'd': {
                long long d = va_arg(args, long long);
                print_int((int64_t)d);
                break;
            }
            case 'x': {
                uint64_t x = (uint64_t)va_arg(args, unsigned long long);
                klog_hex64(x);
                break;
            }
            case 'p': {
                void* p = va_arg(args, void*);
                klog_hex64((uint64_t)(uintptr_t)p);
                break;
            }
            default:
                serial_write("%?");
                putc(spec);
                break;
        }
    }
}

void klogf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vklogf(fmt, args);
    va_end(args);
}
