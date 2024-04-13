#include <stdint.h>
#include <stddef.h>

#include <camkes.h>

#define FFI_SUCCESS 0
#define FFI_FAILURE 1

typedef struct Connection {
    volatile void * data;
    int length;
    void (*wait)(void);
    void (*emit)(void);
} Connection_t;

// memcpy variants for volatile buffers
volatile void * memcpy_vdest(volatile void *dest, const void *src, size_t n) {
    for (int i = 0; i < n; i++)
        ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
    return dest;
}
void * memcpy_vsrc(void *dest, const volatile void *src, size_t n) {
    for (int i = 0; i < n; i++)
        ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
    return dest;
}

// This array needs to be edited to match the particular CAmkES component
#define CONNS_LEN 1
Connection_t conns[CONNS_LEN];

void InitDataports() {
    conns[0] = (Connection_t){
        .data = client_dp,
        .length = 4096,
        .wait = &client_ready_wait,
        .emit = &client_done_emit
    };
}

bool WriteLinuxDataport(const uint8_t* payload) {
    Connection_t conn = conns[0];
    if (conn.data == NULL) {
        return false;
    }
    // Should we fail instead of truncating if input is too long?
    memcpy_vdest(conn.data, (const void *)(payload), 4096);
    return true;
}

bool ReadLinuxDataport(uint8_t* contents) {
    Connection_t conn = conns[0];
    if (conn.data == NULL) {
        return false;
    }
    // Should we fail if output buffer is too long?
    memcpy_vsrc((void *)(contents), conn.data, 4096);
    return true;
}

bool WaitLinuxDataport() {
    Connection_t conn = conns[0];
    if (conn.wait == NULL) {
        return false;
    }
    conn.wait();
    return true;
}

bool EmitLinuxDataport() {
    Connection_t conn = conns[0];
    if (conn.emit == NULL) {
        return false;
    }
    conn.emit();
    return true;
}
