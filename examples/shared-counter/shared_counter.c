/**
 * @file shared_counter.c
 * @brief Shared Counter Example - Concurrent shared memory access
 * 
 * Demonstrates:
 * - Shared memory access from multiple cores
 * - Semaphore-based synchronization
 * - Atomic operations and memory barriers
 */

#include "amp_boot.h"
#include "amp_config.h"
#include "amp_semaphore.h"
#include "amp_shmem.h"
#include "amp_barriers.h"
#include <stdio.h>
#include <stdint.h>

/* Shared memory configuration */
#ifndef SHMEM_BASE
#define SHMEM_BASE 0x20040000
#endif
#ifndef SHMEM_SIZE
#define SHMEM_SIZE (16 * 1024)
#endif

#define INCREMENT_COUNT 100

/* Shared counter structure */
typedef struct {
    volatile uint32_t counter;
    volatile uint32_t core0_increments;
    volatile uint32_t core1_increments;
} shared_counter_t;

/* Global shared resources */
static shared_counter_t *g_shared_counter = NULL;
static amp_semaphore_t g_counter_sem = NULL;

/**
 * Increment the shared counter safely
 */
static void increment_counter(uint32_t core_id)
{
    /* Wait on semaphore */
    if (amp_semaphore_wait(g_counter_sem, 1000) != 0) {
        printf("Core %u: Semaphore wait failed\n", core_id);
        return;
    }

    /* Critical section - increment counter */
    uint32_t old_value = g_shared_counter->counter;
    g_shared_counter->counter = old_value + 1;

    /* Track per-core increments */
    if (core_id == 0) {
        g_shared_counter->core0_increments++;
    } else {
        g_shared_counter->core1_increments++;
    }

    /* Memory barrier */
    AMP_DMB();

    /* Release semaphore */
    amp_semaphore_post(g_counter_sem);
}

/**
 * Core 1 entry point
 */
void core1_main(void)
{
    amp_boot_signal_ready();
    printf("Core 1: Starting counter increments\n");

    /* Increment counter INCREMENT_COUNT times */
    for (int i = 0; i < INCREMENT_COUNT; i++) {
        increment_counter(1);
        
        /* Print progress every 20 increments */
        if ((i + 1) % 20 == 0) {
            printf("Core 1: Completed %d increments\n", i + 1);
        }
    }

    printf("Core 1: Finished all increments\n");
    printf("Core 1: Counter value = %u\n", g_shared_counter->counter);

    while (1) {
#if defined(__ARM_ARCH) || defined(__arm__)
        __asm__ volatile("wfi");
#else
        /* Busy wait on non-ARM platforms */
#endif
    }
}

/**
 * Main function - runs on Core 0
 */
int main(void)
{
    printf("=== Shared Counter Example ===\n");
    printf("Core 0: Initializing...\n");

    /* Initialize shared memory */
    if (amp_shmem_init((void *)SHMEM_BASE, SHMEM_SIZE) != 0) {
        printf("Core 0: Failed to initialize shared memory\n");
        return 1;
    }

    /* Initialize AMP */
    if (amp_boot_init() != AMP_BOOT_SUCCESS) {
        printf("Core 0: Failed to initialize AMP\n");
        return 1;
    }

    /* Allocate shared counter in shared memory */
    g_shared_counter = (shared_counter_t *)amp_shmem_alloc(sizeof(shared_counter_t));
    if (!g_shared_counter) {
        printf("Core 0: Failed to allocate shared counter\n");
        return 1;
    }

    /* Initialize counter */
    g_shared_counter->counter = 0;
    g_shared_counter->core0_increments = 0;
    g_shared_counter->core1_increments = 0;

    /* Create semaphore for synchronization (binary semaphore) */
    g_counter_sem = amp_semaphore_create(1, 1);
    if (!g_counter_sem) {
        printf("Core 0: Failed to create semaphore\n");
        return 1;
    }

    printf("Core 0: Starting Core 1...\n");

    /* Boot core 1 */
    #ifdef PLATFORM_RP2350
    extern void multicore_launch_core1(void (*entry)(void));
    multicore_launch_core1(core1_main);
    #else
    amp_boot_core(AMP_CORE1, (uint32_t)(uintptr_t)core1_main, 0);
    #endif

    /* Wait for core 1 */
    if (amp_boot_wait_core_ready(AMP_CORE1, 1000) != AMP_BOOT_SUCCESS) {
        printf("Core 0: Timeout waiting for Core 1\n");
        return 1;
    }

    printf("Core 0: Both cores running, starting increments...\n");

    /* Core 0 also increments the counter */
    for (int i = 0; i < INCREMENT_COUNT; i++) {
        increment_counter(0);
        
        /* Print progress every 20 increments */
        if ((i + 1) % 20 == 0) {
            printf("Core 0: Completed %d increments\n", i + 1);
        }
    }

    printf("Core 0: Finished all increments\n");

    /* Wait a bit for Core 1 to finish */
    for (volatile int i = 0; i < 1000000; i++);

    /* Display results */
    printf("\n=== Results ===\n");
    printf("Expected counter value: %d\n", INCREMENT_COUNT * 2);
    printf("Actual counter value:   %u\n", g_shared_counter->counter);
    printf("Core 0 increments:      %u\n", g_shared_counter->core0_increments);
    printf("Core 1 increments:      %u\n", g_shared_counter->core1_increments);
    
    if (g_shared_counter->counter == INCREMENT_COUNT * 2) {
        printf("SUCCESS: Counter value is correct!\n");
    } else {
        printf("ERROR: Counter value mismatch!\n");
    }

    printf("===============================\n");

    return 0;
}
