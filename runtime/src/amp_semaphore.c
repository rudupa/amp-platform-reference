/**
 * @file amp_semaphore.c
 * @brief Inter-Core Semaphore Implementation
 */

#include "amp_semaphore.h"
#include "amp_shmem.h"
#include "amp_barriers.h"

/* Semaphore structure in shared memory */
struct amp_semaphore_s {
    volatile uint32_t count;
    uint32_t max_count;
};

/**
 * Atomic compare-and-swap helper
 */
static inline bool atomic_cas(volatile uint32_t *ptr, uint32_t old_val, uint32_t new_val)
{
#if defined(__ARM_ARCH) || defined(__arm__)
    uint32_t result;
    uint32_t tmp;
    
    __asm__ volatile(
        "1: ldrex %0, [%2]\n"
        "   cmp %0, %3\n"
        "   bne 2f\n"
        "   strex %1, %4, [%2]\n"
        "   cmp %1, #0\n"
        "   bne 1b\n"
        "2: dmb\n"
        : "=&r"(result), "=&r"(tmp)
        : "r"(ptr), "r"(old_val), "r"(new_val)
        : "cc", "memory"
    );
    
    /* CAS succeeded if we read old_val and STREX succeeded (tmp == 0) */
    return (result == old_val);
#else
    /* Use GCC built-in atomic compare-and-swap for non-ARM platforms */
    return __sync_bool_compare_and_swap(ptr, old_val, new_val);
#endif
}

/**
 * Create a semaphore
 */
amp_semaphore_t amp_semaphore_create(uint32_t initial_count, uint32_t max_count)
{
    if (initial_count > max_count || max_count == 0) {
        return NULL;
    }

    struct amp_semaphore_s *sem = amp_shmem_alloc(sizeof(struct amp_semaphore_s));
    if (!sem) {
        return NULL;
    }

    sem->count = initial_count;
    sem->max_count = max_count;

    return sem;
}

/**
 * Destroy a semaphore
 */
void amp_semaphore_destroy(amp_semaphore_t sem)
{
    /* Simple allocator doesn't support individual frees */
    (void)sem;
}

/**
 * Try to wait on semaphore (non-blocking)
 */
int amp_semaphore_try_wait(amp_semaphore_t sem)
{
    if (!sem) {
        return -1;
    }

    while (1) {
        uint32_t current = sem->count;
        if (current == 0) {
            return -1;
        }

        if (atomic_cas(&sem->count, current, current - 1)) {
            return 0;
        }
    }
}

/**
 * Wait on semaphore (blocking)
 */
int amp_semaphore_wait(amp_semaphore_t sem, uint32_t timeout_ms)
{
    /* Simple busy-wait timeout (Phase 1 limitation)
     * Production implementations should use hardware timers
     */
    uint32_t count = timeout_ms * 1000;
    
    while (amp_semaphore_try_wait(sem) != 0) {
        if (timeout_ms > 0 && --count == 0) {
            return -1;
        }
    }
    
    return 0;
}

/**
 * Post to semaphore
 */
int amp_semaphore_post(amp_semaphore_t sem)
{
    if (!sem) {
        return -1;
    }

    while (1) {
        uint32_t current = sem->count;
        if (current >= sem->max_count) {
            return -1;
        }

        if (atomic_cas(&sem->count, current, current + 1)) {
            return 0;
        }
    }
}

/**
 * Get current semaphore count
 */
uint32_t amp_semaphore_get_count(amp_semaphore_t sem)
{
    if (!sem) {
        return 0;
    }
    
    return sem->count;
}
