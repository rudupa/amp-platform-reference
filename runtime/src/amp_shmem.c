/**
 * @file amp_shmem.c
 * @brief Shared Memory Management Implementation
 */

#include "amp_shmem.h"
#include <string.h>

/* Simple memory allocator for shared memory */
typedef struct {
    void *base;
    size_t size;
    size_t allocated;
} amp_shmem_pool_t;

static amp_shmem_pool_t g_shmem_pool = {0};

/**
 * Initialize shared memory subsystem
 */
int amp_shmem_init(void *base, size_t size)
{
    if (!base || size == 0) {
        return -1;
    }

    g_shmem_pool.base = base;
    g_shmem_pool.size = size;
    g_shmem_pool.allocated = 0;

    /* Clear the shared memory region */
    memset(base, 0, size);

    return 0;
}

/**
 * Allocate a shared memory region
 * Simple bump allocator - suitable for static allocations
 */
void *amp_shmem_alloc(size_t size)
{
    if (size == 0 || !g_shmem_pool.base) {
        return NULL;
    }

    /* Align to 8-byte boundary */
    size = (size + 7) & ~((size_t)7);

    /* Check if we have enough space */
    if (g_shmem_pool.allocated + size > g_shmem_pool.size) {
        return NULL;
    }

    void *ptr = (char *)g_shmem_pool.base + g_shmem_pool.allocated;
    g_shmem_pool.allocated += size;

    return ptr;
}

/**
 * Free a shared memory region
 * Note: Simple allocator doesn't support individual frees
 */
void amp_shmem_free(void *ptr)
{
    /* Bump allocator doesn't support individual frees
     * This is intentional for simplicity in Phase 1
     */
    (void)ptr;
}

/**
 * Get information about a shared memory region
 */
int amp_shmem_get_region(const void *ptr, amp_shmem_region_t *region)
{
    if (!ptr || !region || !g_shmem_pool.base) {
        return -1;
    }

    /* Check if pointer is within shared memory pool */
    uintptr_t addr = (uintptr_t)ptr;
    uintptr_t base = (uintptr_t)g_shmem_pool.base;
    
    if (addr < base || addr >= base + g_shmem_pool.allocated) {
        return -1;
    }

    region->base = g_shmem_pool.base;
    region->size = g_shmem_pool.size;
    region->flags = 0;

    return 0;
}
