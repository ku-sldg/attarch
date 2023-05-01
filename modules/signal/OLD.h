#ifndef mike_devices_h
#define mike_devices_h

#define MAX_CONNECTOR_DEVICES 32
#define DEVICE_NAME_MAX_LEN 50
typedef struct connector_dev_node {
    struct pci_dev *dev;
    struct uio_info *uio;
    char dev_name[DEVICE_NAME_MAX_LEN];
} connector_dev_node_t;
connector_dev_node_t* devices[MAX_CONNECTOR_DEVICES];

#endif

