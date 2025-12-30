/**
 * @file amp_ringbuf.c
 * @brief Lock-Free Ring Buffer Implementation
 */

#include "amp_ringbuf.h"
#include "amp_shmem.h"
#include "amp_barriers.h"
#include <string.h>

/* Ring buffer structure in shared memory */
struct amp_ringbuf_s {
    volatile uint32_t write_idx;
    volatile uint32_t read_idx;
    uint32_t size;
    uint32_t mask;  /* size - 1, for fast modulo */
    char data[];    /* Buffer data follows */
};

/**
 * Create a ring buffer
 */
amp_ringbuf_t amp_ringbuf_create(size_t size)
{
    /* Ensure size is power of 2 */
    if (size == 0 || (size & (size - 1)) != 0) {
        return NULL;
    }

    size_t total_size = sizeof(struct amp_ringbuf_s) + size;
    struct amp_ringbuf_s *rb = amp_shmem_alloc(total_size);
    
    if (!rb) {
        return NULL;
    }

    rb->write_idx = 0;
    rb->read_idx = 0;
    rb->size = (uint32_t)size;
    rb->mask = (uint32_t)(size - 1);

    return rb;
}

/**
 * Destroy a ring buffer
 */
void amp_ringbuf_destroy(amp_ringbuf_t rb)
{
    /* Simple allocator doesn't support individual frees */
    (void)rb;
}

/**
 * Get available bytes to read
 */
size_t amp_ringbuf_available(amp_ringbuf_t rb)
{
    if (!rb) {
        return 0;
    }

    uint32_t write_idx = rb->write_idx;
    uint32_t read_idx = rb->read_idx;
    
    /* Unsigned subtraction handles wraparound correctly */
    return (size_t)(write_idx - read_idx);
}

/**
 * Get free space in buffer
 */
size_t amp_ringbuf_free_space(amp_ringbuf_t rb)
{
    if (!rb) {
        return 0;
    }

    return rb->size - amp_ringbuf_available(rb);
}

/**
 * Write data to ring buffer
 */
size_t amp_ringbuf_write(amp_ringbuf_t rb, const void *data, size_t len)
{
    if (!rb || !data || len == 0) {
        return 0;
    }

    size_t free_space = amp_ringbuf_free_space(rb);
    if (len > free_space) {
        len = free_space;
    }

    const char *src = (const char *)data;
    uint32_t write_idx = rb->write_idx;
    
    for (size_t i = 0; i < len; i++) {
        rb->data[(write_idx + i) & rb->mask] = src[i];
    }

    /* Memory barrier before updating write index */
    AMP_DMB();
    rb->write_idx = write_idx + (uint32_t)len;

    return len;
}

/**
 * Read data from ring buffer
 */
size_t amp_ringbuf_read(amp_ringbuf_t rb, void *data, size_t len)
{
    if (!rb || !data || len == 0) {
        return 0;
    }

    size_t available = amp_ringbuf_available(rb);
    if (len > available) {
        len = available;
    }

    char *dst = (char *)data;
    uint32_t read_idx = rb->read_idx;
    
    for (size_t i = 0; i < len; i++) {
        dst[i] = rb->data[(read_idx + i) & rb->mask];
    }

    /* Memory barrier before updating read index */
    AMP_DMB();
    rb->read_idx = read_idx + (uint32_t)len;

    return len;
}

/**
 * Clear all data from buffer
 */
void amp_ringbuf_clear(amp_ringbuf_t rb)
{
    if (!rb) {
        return;
    }

    rb->read_idx = rb->write_idx;
    AMP_DMB();
}
