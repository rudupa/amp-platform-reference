/**
 * @file amp_mailbox.h
 * @brief Inter-Core Mailbox Communication
 * 
 * Provides mailbox-based message passing between cores.
 * Mailboxes support fixed-size message slots with blocking/non-blocking modes.
 */

#ifndef AMP_MAILBOX_H
#define AMP_MAILBOX_H

#include <stdint.h>
#include <stdbool.h>
#include "amp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Mailbox handle
 */
typedef struct amp_mailbox_s *amp_mailbox_t;

/**
 * Mailbox configuration
 */
typedef struct {
    uint32_t msg_size;      /**< Size of each message in bytes */
    uint32_t msg_slots;     /**< Number of message slots */
} amp_mailbox_config_t;

/**
 * Create a mailbox
 * 
 * @param config Mailbox configuration
 * @return Mailbox handle or NULL on failure
 */
amp_mailbox_t amp_mailbox_create(const amp_mailbox_config_t *config);

/**
 * Destroy a mailbox
 * 
 * @param mbox Mailbox handle
 */
void amp_mailbox_destroy(amp_mailbox_t mbox);

/**
 * Send a message (blocking)
 * 
 * @param mbox Mailbox handle
 * @param msg Message data
 * @param timeout_ms Timeout in milliseconds (0 = no timeout)
 * @return 0 on success, negative on error
 */
int amp_mailbox_send(amp_mailbox_t mbox, const void *msg, uint32_t timeout_ms);

/**
 * Receive a message (blocking)
 * 
 * @param mbox Mailbox handle
 * @param msg Buffer to receive message
 * @param timeout_ms Timeout in milliseconds (0 = no timeout)
 * @return 0 on success, negative on error
 */
int amp_mailbox_recv(amp_mailbox_t mbox, void *msg, uint32_t timeout_ms);

/**
 * Try to send a message (non-blocking)
 * 
 * @param mbox Mailbox handle
 * @param msg Message data
 * @return 0 on success, -1 if full
 */
int amp_mailbox_try_send(amp_mailbox_t mbox, const void *msg);

/**
 * Try to receive a message (non-blocking)
 * 
 * @param mbox Mailbox handle
 * @param msg Buffer to receive message
 * @return 0 on success, -1 if empty
 */
int amp_mailbox_try_recv(amp_mailbox_t mbox, void *msg);

#ifdef __cplusplus
}
#endif

#endif /* AMP_MAILBOX_H */
