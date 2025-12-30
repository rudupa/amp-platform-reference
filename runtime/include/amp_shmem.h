/**
 * @file amp_shmem.h
 * @brief Shared Memory Management
 * 
 * Provides allocation and management of shared memory regions
 * accessible by multiple cores.
 */

#ifndef AMP_SHMEM_H
#define AMP_SHMEM_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Shared memory region descriptor
 */
typedef struct {
    void *base;         /**< Base address */
    size_t size;        /**< Size in bytes */
    uint32_t flags;     /**< Access flags */
} amp_shmem_region_t;

/**
 * Initialize shared memory subsystem
 * 
 * @param base Base address of shared memory pool
 * @param size Total size of shared memory pool
 * @return 0 on success, negative on error
 */
int amp_shmem_init(void *base, size_t size);

/**
 * Allocate a shared memory region
 * 
 * @param size Size to allocate in bytes
 * @return Pointer to allocated region or NULL on failure
 */
void *amp_shmem_alloc(size_t size);

/**
 * Free a shared memory region
 * 
 * @param ptr Pointer to region to free
 */
void amp_shmem_free(void *ptr);

/**
 * Get information about a shared memory region
 * 
 * @param ptr Pointer within the region
 * @param region Output region descriptor
 * @return 0 on success, negative on error
 */
int amp_shmem_get_region(const void *ptr, amp_shmem_region_t *region);

#ifdef __cplusplus
}
#endif

#endif /* AMP_SHMEM_H */
