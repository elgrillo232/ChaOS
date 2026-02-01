#pragma once

#include <stdarg.h>
#include <stdint.h>

// Minimal early logging to COM1 serial.
// Safe after ExitBootServices.

void klog_write(const char* s);
void klog_writeln(const char* s);
void klog_hex64(uint64_t v);
void klog_dec_u64(uint64_t v);

// Very small formatter.
// Supported: %% %s %c %u %d %x %p
void klogf(const char* fmt, ...);
void vklogf(const char* fmt, va_list args);
