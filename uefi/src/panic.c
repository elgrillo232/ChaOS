#include "panic.h"

#include "klog.h"

__attribute__((noreturn)) static void halt_forever(void) {
    __asm__ __volatile__("cli");
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}

__attribute__((noreturn)) void panic_at(const char* msg, const char* file, int line, const char* func) {
    klog_writeln("\r\n[ChaOS] PANIC");
    if (msg) {
        klog_write("msg: ");
        klog_writeln(msg);
    }
    if (file) {
        klog_write("at: ");
        klog_write(file);
        klog_write(":");
        klog_dec_u64((uint64_t)(uint32_t)line);
        if (func) {
            klog_write(" (");
            klog_write(func);
            klog_write(")");
        }
        klog_writeln("");
    }

    halt_forever();
}
