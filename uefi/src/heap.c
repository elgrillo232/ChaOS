#include "heap.h"

#include "pmm.h"

#include <stdint.h>

typedef union header {
    struct {
        union header* next;
        UINTN units;
    } s;
    // Force alignment.
    uint64_t _align;
} header_t;

static header_t g_base;
static header_t* g_freep = 0;

static UINTN bytes_to_units(UINTN nbytes) {
    // Round up to header-sized units, plus one for the header itself.
    UINTN units = (nbytes + sizeof(header_t) - 1) / sizeof(header_t) + 1;
    if (units < 2) {
        units = 2;
    }
    return units;
}

static header_t* morecore(UINTN nunits) {
    // Allocate whole pages from PMM.
    UINTN bytes = nunits * (UINTN)sizeof(header_t);
    UINTN pages = (bytes + 4095u) / 4096u;
    if (pages == 0) {
        pages = 1;
    }

    EFI_PHYSICAL_ADDRESS phys = pmm_alloc_pages(pages);
    if (!phys) {
        return 0;
    }

    header_t* hp = (header_t*)(uintptr_t)phys;
    hp->s.units = (pages * 4096u) / (UINTN)sizeof(header_t);

    // Add the new memory to the free list by freeing the block after its header.
    kfree((void*)(hp + 1));
    return g_freep;
}

void heap_init(void) {
    g_base.s.next = &g_base;
    g_base.s.units = 0;
    g_freep = &g_base;

    // Prime with a small initial pool.
    (void)morecore(bytes_to_units(16 * 1024u));
}

void* kmalloc(UINTN nbytes) {
    if (nbytes == 0) {
        return 0;
    }
    if (!g_freep) {
        heap_init();
    }

    UINTN nunits = bytes_to_units(nbytes);
    header_t* prevp = g_freep;
    for (header_t* p = prevp->s.next;; prevp = p, p = p->s.next) {
        if (p->s.units >= nunits) {
            if (p->s.units == nunits) {
                prevp->s.next = p->s.next;
            } else {
                // Allocate tail end.
                p->s.units -= nunits;
                p += p->s.units;
                p->s.units = nunits;
            }
            g_freep = prevp;
            return (void*)(p + 1);
        }

        if (p == g_freep) {
            if (!morecore(nunits)) {
                return 0;
            }
            prevp = g_freep;
            p = prevp->s.next;
        }
    }
}

void kfree(void* ap) {
    if (!ap) {
        return;
    }
    if (!g_freep) {
        heap_init();
    }

    header_t* bp = ((header_t*)ap) - 1;
    header_t* p = g_freep;

    // Find the place to insert in address order.
    for (;; p = p->s.next) {
        if (bp > p && bp < p->s.next) {
            break;
        }
        if (p >= p->s.next && (bp > p || bp < p->s.next)) {
            break;
        }
    }

    // Join to upper neighbor
    if (bp + bp->s.units == p->s.next) {
        bp->s.units += p->s.next->s.units;
        bp->s.next = p->s.next->s.next;
    } else {
        bp->s.next = p->s.next;
    }

    // Join to lower neighbor
    if (p + p->s.units == bp) {
        p->s.units += bp->s.units;
        p->s.next = bp->s.next;
    } else {
        p->s.next = bp;
    }

    g_freep = p;
}
