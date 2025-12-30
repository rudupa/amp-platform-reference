/**
 * @file amp_barriers.h
 * @brief Memory Barrier Utilities
 * 
 * Platform-independent memory barrier macros
 */

#ifndef AMP_BARRIERS_H
#define AMP_BARRIERS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Data Memory Barrier
 * Ensures memory accesses are ordered
 */
#if defined(__ARM_ARCH) || defined(__arm__)
    #define AMP_DMB() __asm__ volatile("dmb" ::: "memory")
#else
    #define AMP_DMB() __sync_synchronize()
#endif

/**
 * Data Synchronization Barrier
 * Ensures all memory accesses complete
 */
#if defined(__ARM_ARCH) || defined(__arm__)
    #define AMP_DSB() __asm__ volatile("dsb" ::: "memory")
#else
    #define AMP_DSB() __sync_synchronize()
#endif

/**
 * Instruction Synchronization Barrier
 * Flushes instruction pipeline
 */
#if defined(__ARM_ARCH) || defined(__arm__)
    #define AMP_ISB() __asm__ volatile("isb" ::: "memory")
#else
    #define AMP_ISB() __sync_synchronize()
#endif

#ifdef __cplusplus
}
#endif

#endif /* AMP_BARRIERS_H */
