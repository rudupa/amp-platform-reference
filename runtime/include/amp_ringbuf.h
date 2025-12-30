/**
 * @file amp_ringbuf.h
 * @brief Lock-Free Ring Buffer for Inter-Core Communication
 * 
 * Provides a lock-free ring buffer for efficient streaming data
 * between cores. Supports single producer/single consumer.
 */

#ifndef AMP_RINGBUF_H
#define AMP_RINGBUF_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Ring buffer handle
 */
typedef struct amp_ringbuf_s *amp_ringbuf_t;

/**
 * Create a ring buffer
 * 
 * @param size Buffer size in bytes (must be power of 2)
 * @return Ring buffer handle or NULL on failure
 */
amp_ringbuf_t amp_ringbuf_create(size_t size);

/**
 * Destroy a ring buffer
 * 
 * @param rb Ring buffer handle
 */
void amp_ringbuf_destroy(amp_ringbuf_t rb);

/**
 * Write data to ring buffer
 * 
 * @param rb Ring buffer handle
 * @param data Data to write
 * @param len Length in bytes
 * @return Number of bytes written
 */
size_t amp_ringbuf_write(amp_ringbuf_t rb, const void *data, size_t len);

/**
 * Read data from ring buffer
 * 
 * @param rb Ring buffer handle
 * @param data Buffer to read into
 * @param len Maximum bytes to read
 * @return Number of bytes read
 */
size_t amp_ringbuf_read(amp_ringbuf_t rb, void *data, size_t len);

/**
 * Get available bytes to read
 * 
 * @param rb Ring buffer handle
 * @return Number of bytes available
 */
size_t amp_ringbuf_available(amp_ringbuf_t rb);

/**
 * Get free space in buffer
 * 
 * @param rb Ring buffer handle
 * @return Number of free bytes
 */
size_t amp_ringbuf_free_space(amp_ringbuf_t rb);

/**
 * Clear all data from buffer
 * 
 * @param rb Ring buffer handle
 */
void amp_ringbuf_clear(amp_ringbuf_t rb);

#ifdef __cplusplus
}
#endif

#endif /* AMP_RINGBUF_H */
