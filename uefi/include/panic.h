#pragma once

#include "uefi.h"

// Panic and assertion helpers.
// These only use serial output (safe after ExitBootServices).

__attribute__((noreturn)) void panic_at(const char* msg, const char* file, int line, const char* func);

#define PANIC(msg) panic_at((msg), __FILE__, __LINE__, __func__)

#define ASSERT(expr) do { \
    if (!(expr)) { \
        panic_at("ASSERT FAILED: " #expr, __FILE__, __LINE__, __func__); \
    } \
} while (0)
