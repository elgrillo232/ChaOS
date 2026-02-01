#pragma once

#include <stddef.h>

// Freestanding implementations of common C string/memory routines.
// We provide the standard names so the compiler/linker can resolve builtins.

void* memcpy(void* dst, const void* src, size_t n);
void* memmove(void* dst, const void* src, size_t n);
void* memset(void* dst, int c, size_t n);
int memcmp(const void* a, const void* b, size_t n);

size_t strlen(const char* s);
size_t strnlen(const char* s, size_t maxlen);
