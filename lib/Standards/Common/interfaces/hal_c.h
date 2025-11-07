#pragma once

/*
 * @file hal_c.h
 * @brief C-compatible Hardware Abstraction Layer (HAL) for standards layer
 *
 * Implements C function pointer style interfaces with integer return codes.
 * This header is safe for inclusion from C or C++ and is intended for the
 * standards layer to remain platform agnostic and real-time friendly.
 *
 * Error codes: 0 = success; negative values indicate specific failures.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* Generic opaque context pointer passed to all HAL functions */
typedef void* hal_ctx_t;

/* Network HAL */
typedef struct hal_network_vtbl_s {
    int (*send_packet)(hal_ctx_t ctx, const void* data, size_t length);
    int (*receive_packet)(hal_ctx_t ctx, void* buffer, size_t* inout_length, uint32_t timeout_ms);
    int (*send_vlan_packet)(hal_ctx_t ctx, const void* data, size_t length, uint16_t vlan_id, uint8_t priority);
    int (*get_mac_address)(hal_ctx_t ctx, uint8_t mac[6]);
    int (*get_link_status)(hal_ctx_t ctx); /* 1=up, 0=down, <0 error */
} hal_network_vtbl_t;

/* Time/Clock HAL */
typedef struct hal_clock_vtbl_s {
    int (*get_time_ns)(hal_ctx_t ctx, uint64_t* out_time_ns);
} hal_clock_vtbl_t;

/* Timer HAL (non-blocking, callback-based) */
typedef void (*hal_timer_cb_t)(hal_ctx_t ctx, void* user);

typedef struct hal_timer_vtbl_s {
    int (*set_timer_ns)(hal_ctx_t ctx, uint64_t delay_ns, hal_timer_cb_t cb, void* user);
    int (*cancel_timer)(hal_ctx_t ctx, hal_timer_cb_t cb, void* user);
} hal_timer_vtbl_t;

/* Memory HAL (optional, for RT-friendly pools) */
typedef struct hal_memory_vtbl_s {
    void* (*alloc)(hal_ctx_t ctx, size_t size);
    void  (*free)(hal_ctx_t ctx, void* ptr);
} hal_memory_vtbl_t;

/* Aggregated HAL */
typedef struct hal_c_s {
    hal_ctx_t            context; /* user-provided opaque context */
    const hal_network_vtbl_t* network;
    const hal_clock_vtbl_t*   clock;
    const hal_timer_vtbl_t*   timer;
    const hal_memory_vtbl_t*  memory; /* optional */
} hal_c_t;

#ifdef __cplusplus
} /* extern "C" */
#endif
