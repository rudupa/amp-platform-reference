/**
 * @file amp_mailbox.c
 * @brief Inter-Core Mailbox Implementation
 */

#include "amp_mailbox.h"
#include "amp_shmem.h"
#include "amp_barriers.h"
#include <string.h>

/* Mailbox structure in shared memory */
struct amp_mailbox_s {
    volatile uint32_t write_idx;
    volatile uint32_t read_idx;
    uint32_t msg_size;
    uint32_t msg_slots;
    uint32_t mask;  /* msg_slots - 1, for fast modulo */
    char data[];    /* Message data follows */
};

/**
 * Create a mailbox
 */
amp_mailbox_t amp_mailbox_create(const amp_mailbox_config_t *config)
{
    if (!config || config->msg_size == 0 || config->msg_slots == 0) {
        return NULL;
    }

    /* Ensure msg_slots is power of 2 for efficient indexing */
    uint32_t slots = config->msg_slots;
    if ((slots & (slots - 1)) != 0) {
        /* Round up to next power of 2 */
        slots--;
        slots |= slots >> 1;
        slots |= slots >> 2;
        slots |= slots >> 4;
        slots |= slots >> 8;
        slots |= slots >> 16;
        slots++;
    }

    size_t total_size = sizeof(struct amp_mailbox_s) + (config->msg_size * slots);
    struct amp_mailbox_s *mbox = amp_shmem_alloc(total_size);
    
    if (!mbox) {
        return NULL;
    }

    mbox->write_idx = 0;
    mbox->read_idx = 0;
    mbox->msg_size = config->msg_size;
    mbox->msg_slots = slots;
    mbox->mask = slots - 1;

    return mbox;
}

/**
 * Destroy a mailbox
 */
void amp_mailbox_destroy(amp_mailbox_t mbox)
{
    /* Simple allocator doesn't support individual frees */
    (void)mbox;
}

/**
 * Try to send a message (non-blocking)
 */
int amp_mailbox_try_send(amp_mailbox_t mbox, const void *msg)
{
    if (!mbox || !msg) {
        return -1;
    }

    uint32_t write_idx = mbox->write_idx;
    uint32_t read_idx = mbox->read_idx;

    /* Check if mailbox is full */
    if (write_idx - read_idx >= mbox->msg_slots) {
        return -1;
    }

    /* Copy message */
    uint32_t slot = write_idx & mbox->mask;
    void *dest = &mbox->data[slot * mbox->msg_size];
    memcpy(dest, msg, mbox->msg_size);

    /* Memory barrier before updating write index */
    AMP_DMB();
    mbox->write_idx = write_idx + 1;

    return 0;
}

/**
 * Try to receive a message (non-blocking)
 */
int amp_mailbox_try_recv(amp_mailbox_t mbox, void *msg)
{
    if (!mbox || !msg) {
        return -1;
    }

    uint32_t write_idx = mbox->write_idx;
    uint32_t read_idx = mbox->read_idx;

    /* Check if mailbox is empty */
    if (read_idx >= write_idx) {
        return -1;
    }

    /* Copy message */
    uint32_t slot = read_idx & mbox->mask;
    const void *src = &mbox->data[slot * mbox->msg_size];
    memcpy(msg, src, mbox->msg_size);

    /* Memory barrier before updating read index */
    AMP_DMB();
    mbox->read_idx = read_idx + 1;

    return 0;
}

/**
 * Send a message (blocking)
 */
int amp_mailbox_send(amp_mailbox_t mbox, const void *msg, uint32_t timeout_ms)
{
    /* Simple busy-wait timeout (Phase 1 limitation)
     * Production implementations should use hardware timers
     */
    uint32_t count = timeout_ms * 1000;
    
    while (amp_mailbox_try_send(mbox, msg) != 0) {
        if (timeout_ms > 0 && --count == 0) {
            return -1;
        }
    }
    
    return 0;
}

/**
 * Receive a message (blocking)
 */
int amp_mailbox_recv(amp_mailbox_t mbox, void *msg, uint32_t timeout_ms)
{
    /* Simple busy-wait timeout (Phase 1 limitation)
     * Production implementations should use hardware timers
     */
    uint32_t count = timeout_ms * 1000;
    
    while (amp_mailbox_try_recv(mbox, msg) != 0) {
        if (timeout_ms > 0 && --count == 0) {
            return -1;
        }
    }
    
    return 0;
}
