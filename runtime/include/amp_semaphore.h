/**
 * @file amp_semaphore.h
 * @brief Inter-Core Semaphore Synchronization
 * 
 * Provides counting semaphores for synchronization between cores.
 */

#ifndef AMP_SEMAPHORE_H
#define AMP_SEMAPHORE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Semaphore handle
 */
typedef struct amp_semaphore_s *amp_semaphore_t;

/**
 * Create a semaphore
 * 
 * @param initial_count Initial count value
 * @param max_count Maximum count value
 * @return Semaphore handle or NULL on failure
 */
amp_semaphore_t amp_semaphore_create(uint32_t initial_count, uint32_t max_count);

/**
 * Destroy a semaphore
 * 
 * @param sem Semaphore handle
 */
void amp_semaphore_destroy(amp_semaphore_t sem);

/**
 * Wait on semaphore (blocking)
 * 
 * Decrements the semaphore count. If count is 0, blocks until available.
 * 
 * @param sem Semaphore handle
 * @param timeout_ms Timeout in milliseconds (0 = no timeout)
 * @return 0 on success, negative on error/timeout
 */
int amp_semaphore_wait(amp_semaphore_t sem, uint32_t timeout_ms);

/**
 * Try to wait on semaphore (non-blocking)
 * 
 * @param sem Semaphore handle
 * @return 0 on success, -1 if unavailable
 */
int amp_semaphore_try_wait(amp_semaphore_t sem);

/**
 * Post to semaphore
 * 
 * Increments the semaphore count up to max_count.
 * 
 * @param sem Semaphore handle
 * @return 0 on success, negative on error
 */
int amp_semaphore_post(amp_semaphore_t sem);

/**
 * Get current semaphore count
 * 
 * @param sem Semaphore handle
 * @return Current count value
 */
uint32_t amp_semaphore_get_count(amp_semaphore_t sem);

#ifdef __cplusplus
}
#endif

#endif /* AMP_SEMAPHORE_H */
