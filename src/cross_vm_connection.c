/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <camkes.h>
#include <vmlinux.h>
#include <sel4vm/guest_vm.h>

#include <sel4vmmplatsupport/drivers/cross_vm_connection.h>
#include <sel4vmmplatsupport/drivers/pci_helper.h>
#include <pci/helper.h>

#ifdef CONFIG_PLAT_QEMU_ARM_VIRT
#define CONNECTION_BASE_ADDRESS 0xDF000000
#else
#define CONNECTION_BASE_ADDRESS 0x3F000000
#endif

// these are defined in the dataport's glue code
extern dataport_caps_handle_t server_dp_handle;

//Leave field 3 as "-1" to opt to NOT register a consume event
//However, we can later replace the "-1" with the "done_notification_badge" or
//what have you. The final field, the "name"... doesn't matter.
//The dp_handle is required.
static struct camkes_crossvm_connection connections[] = {
    {&server_dp_handle, server_ready_emit, -1, "client_and_server"}
};

extern seL4_Word server_done_notification_badge(void);

static int consume_callback_0(vm_t *vm, void *cookie)
{
    consume_connection_event(vm, connections[0].consume_badge, true);
    printf("consume zero\n");
    return 0;
}

/* extern seL4_Word done_notification_badge(void); */
void init_cross_vm_connections(vm_t *vm, void *cookie)
{
    // setup the "done-wait" event/signal connections
    // These connection MUST be seL4_GlobalAsynch connections!
    connections[0].consume_badge = server_done_notification_badge();
    int err0 = register_async_event_handler(connections[0].consume_badge, consume_callback_0, NULL);
    ZF_LOGF_IF(err0, "Failed to register_async_event_handler 0 for init_cross_vm_connections.");

    // init the connections
    cross_vm_connections_init(vm, CONNECTION_BASE_ADDRESS, connections, ARRAY_SIZE(connections));
}

DEFINE_MODULE(cross_vm_connections, NULL, init_cross_vm_connections)
