#include "idt.h"

#include <stddef.h>

typedef struct __attribute__((packed)) {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} idt_entry_t;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} idtr_t;

static idt_entry_t g_idt[256];

static uint16_t g_cs_selector = 0;

extern void isr_stub_0(void);
extern void isr_stub_1(void);
extern void isr_stub_2(void);
extern void isr_stub_3(void);
extern void isr_stub_4(void);
extern void isr_stub_5(void);
extern void isr_stub_6(void);
extern void isr_stub_7(void);
extern void isr_stub_8(void);
extern void isr_stub_9(void);
extern void isr_stub_10(void);
extern void isr_stub_11(void);
extern void isr_stub_12(void);
extern void isr_stub_13(void);
extern void isr_stub_14(void);
extern void isr_stub_15(void);
extern void isr_stub_16(void);
extern void isr_stub_17(void);
extern void isr_stub_18(void);
extern void isr_stub_19(void);
extern void isr_stub_20(void);
extern void isr_stub_21(void);
extern void isr_stub_22(void);
extern void isr_stub_23(void);
extern void isr_stub_24(void);
extern void isr_stub_25(void);
extern void isr_stub_26(void);
extern void isr_stub_27(void);
extern void isr_stub_28(void);
extern void isr_stub_29(void);
extern void isr_stub_30(void);
extern void isr_stub_31(void);
extern void isr_stub_default(void);

static void set_gate(int vec, void (*isr)(void)) {
    uint64_t addr = (uint64_t)(uintptr_t)isr;
    g_idt[vec].offset_low = (uint16_t)(addr & 0xFFFF);
    g_idt[vec].selector = g_cs_selector;
    g_idt[vec].ist = 0;
    g_idt[vec].type_attr = 0x8E; // present, ring0, interrupt gate
    g_idt[vec].offset_mid = (uint16_t)((addr >> 16) & 0xFFFF);
    g_idt[vec].offset_high = (uint32_t)((addr >> 32) & 0xFFFFFFFF);
    g_idt[vec].zero = 0;
}

void idt_init(void) {
    __asm__ __volatile__("mov %%cs, %0" : "=r"(g_cs_selector));

    for (int i = 0; i < 256; i++) {
        set_gate(i, isr_stub_default);
    }

    set_gate(0, isr_stub_0);
    set_gate(1, isr_stub_1);
    set_gate(2, isr_stub_2);
    set_gate(3, isr_stub_3);
    set_gate(4, isr_stub_4);
    set_gate(5, isr_stub_5);
    set_gate(6, isr_stub_6);
    set_gate(7, isr_stub_7);
    set_gate(8, isr_stub_8);
    set_gate(9, isr_stub_9);
    set_gate(10, isr_stub_10);
    set_gate(11, isr_stub_11);
    set_gate(12, isr_stub_12);
    set_gate(13, isr_stub_13);
    set_gate(14, isr_stub_14);
    set_gate(15, isr_stub_15);
    set_gate(16, isr_stub_16);
    set_gate(17, isr_stub_17);
    set_gate(18, isr_stub_18);
    set_gate(19, isr_stub_19);
    set_gate(20, isr_stub_20);
    set_gate(21, isr_stub_21);
    set_gate(22, isr_stub_22);
    set_gate(23, isr_stub_23);
    set_gate(24, isr_stub_24);
    set_gate(25, isr_stub_25);
    set_gate(26, isr_stub_26);
    set_gate(27, isr_stub_27);
    set_gate(28, isr_stub_28);
    set_gate(29, isr_stub_29);
    set_gate(30, isr_stub_30);
    set_gate(31, isr_stub_31);

    idtr_t idtr;
    idtr.limit = (uint16_t)(sizeof(g_idt) - 1);
    idtr.base = (uint64_t)(uintptr_t)&g_idt[0];

    __asm__ __volatile__("lidt %0" : : "m"(idtr));
}
