#ifndef __LIBUSB_H__
#define __LIBUSB_H__

#include <poll.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIBUSB_HOTPLUG_MATCH_ANY    -1

#define libusb_device_descriptor    usb_device_desc_t

enum libusb_error {
    LIBUSB_SUCCESS,
    LIBUSB_ERROR_NO_MEM,
    LIBUSB_ERROR_NOT_SUPPORTED,
    LIBUSB_ERROR_NOT_FOUND,
    LIBUSB_ERROR_IO,
};

enum libusb_capability {
    LIBUSB_CAP_HAS_CAPABILITY,
    LIBUSB_CAP_HAS_HOTPLUG,
    LIBUSB_CAP_HAS_HID_ACCESS,
    LIBUSB_CAP_SUPPORTS_DETACH_KERNEL_DRIVER,
};

enum libusb_speed {
    LIBUSB_SPEED_UNKNOWN                = USB_SPEED_UNKNOWN,
    LIBUSB_SPEED_LOW                    = USB_SPEED_LOW,
    LIBUSB_SPEED_FULL                   = USB_SPEED_FULL,
    LIBUSB_SPEED_HIGH                   = USB_SPEED_HIGH,
    LIBUSB_SPEED_SUPER                  = USB_SPEED_SUPER,
};

enum libusb_endpoint_direction {
    LIBUSB_ENDPOINT_IN                  = USB_DIR_IN,
    LIBUSB_ENDPOINT_OUT                 = USB_DIR_OUT,
};

enum libusb_request_type {
    LIBUSB_REQUEST_TYPE_STANDARD        = USB_TYPE_STANDARD,
    LIBUSB_REQUEST_TYPE_CLASS           = USB_TYPE_CLASS,
    LIBUSB_REQUEST_TYPE_VENDOR          = USB_TYPE_VENDOR,
    LIBUSB_REQUEST_TYPE_RESERVED        = USB_TYPE_RESERVED,
};

enum libusb_request_recipient {
    LIBUSB_RECIPIENT_DEVICE             = USB_RECIP_DEVICE,
    LIBUSB_RECIPIENT_INTERFACE          = USB_RECIP_INTERFACE,
    LIBUSB_RECIPIENT_ENDPOINT           = USB_RECIP_ENDPOINT,
    LIBUSB_RECIPIENT_OTHER              = USB_RECIP_OTHER,
};

enum libusb_standard_request {
    LIBUSB_REQUEST_GET_STATUS           = USB_REQ_GET_STATUS,
    LIBUSB_REQUEST_CLEAR_FEATURE        = USB_REQ_CLEAR_FEATURE,
    LIBUSB_REQUEST_SET_FEATURE          = USB_REQ_SET_FEATURE,
    LIBUSB_REQUEST_SET_ADDRESS          = USB_REQ_SET_ADDRESS,
    LIBUSB_REQUEST_GET_DESCRIPTOR       = USB_REQ_GET_DESCRIPTOR,
    LIBUSB_REQUEST_SET_DESCRIPTOR       = USB_REQ_SET_DESCRIPTOR,
    LIBUSB_REQUEST_GET_CONFIGURATION    = USB_REQ_GET_CONFIGURATION,
    LIBUSB_REQUEST_SET_CONFIGURATION    = USB_REQ_SET_CONFIGURATION,
    LIBUSB_REQUEST_GET_INTERFACE        = USB_REQ_GET_INTERFACE,
    LIBUSB_REQUEST_SET_INTERFACE        = USB_REQ_SET_INTERFACE,
    LIBUSB_REQUEST_SYNCH_FRAME          = USB_REQ_SYNCH_FRAME,
};

enum libusb_descriptor_type {
    LIBUSB_DT_DEVICE                    = USB_DT_DEVICE,
    LIBUSB_DT_CONFIG                    = USB_DT_CONFIG,
    LIBUSB_DT_STRING                    = USB_DT_STRING,
    LIBUSB_DT_INTERFACE                 = USB_DT_INTERFACE,
    LIBUSB_DT_ENDPOINT                  = USB_DT_ENDPOINT,
    LIBUSB_DT_BOS                       = USB_DT_BOS,
    LIBUSB_DT_DEVICE_CAPABILITY         = USB_DT_DEVICE_CAPABILITY,
    LIBUSB_DT_SS_ENDPOINT_COMPANION     = USB_DT_SS_ENDPOINT_COMP,
};

enum libusb_descriptor_size {
    LIBUSB_DT_DEVICE_SIZE               = USB_DT_DEVICE_SIZE,
    LIBUSB_DT_CONFIG_SIZE               = USB_DT_CONFIG_SIZE,
    LIBUSB_DT_INTERFACE_SIZE            = USB_DT_INTERFACE_SIZE,
    LIBUSB_DT_ENDPOINT_SIZE             = USB_DT_ENDPOINT_SIZE,
    LIBUSB_DT_ENDPOINT_AUDIO_SIZE       = USB_DT_ENDPOINT_AUDIO_SIZE,
};

struct libusb_endpoint_descriptor {
    implement(usb_endpoint_desc_t)

    const unsigned char *extra;
    int extra_length;
};

struct libusb_interface_descriptor {
    implement(usb_interface_desc_t)

    const struct libusb_endpoint_descriptor *endpoint;
    const unsigned char *extra;
    int extra_length;
};

struct libusb_interface {
    const struct libusb_interface_descriptor *altsetting;
    int num_altsetting;
};

struct libusb_config_descriptor {
    implement(usb_config_desc_t)
    const unsigned char *desc;

    struct libusb_interface *interface;
    const unsigned char *extra;
    int extra_length;
};

struct libusb_control_setup {
    implement(usb_ctrlrequest_t)
};

#define LIBUSB_CONTROL_SETUP_SIZE       (sizeof(struct libusb_control_setup))

typedef int libusb_hotplug_callback_handle;

struct libusb_context {
    int dummy;
};
typedef struct libusb_context libusb_context;

struct libusb_device;
typedef struct libusb_device libusb_device;
typedef libusb_device libusb_device_handle;

typedef enum {
    LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED,
    LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
} libusb_hotplug_event;

typedef enum {
    LIBUSB_HOTPLUG_NO_FLAGS = 0,
    LIBUSB_HOTPLUG_ENUMERATE,
} libusb_hotplug_flag;

typedef int (*libusb_hotplug_callback_fn)(libusb_context *ctx,
        libusb_device *device, libusb_hotplug_event event, void *user_data);


struct libusb_pollfd {
    struct pollfd;
};

enum libusb_transfer_status {
    LIBUSB_TRANSFER_COMPLETED,
    LIBUSB_TRANSFER_ERROR,
    LIBUSB_TRANSFER_CANCELLED,
    LIBUSB_TRANSFER_NO_DEVICE,
    LIBUSB_TRANSFER_TIMED_OUT,
    LIBUSB_TRANSFER_STALL,
    LIBUSB_TRANSFER_OVERFLOW,
};

enum libusb_transfer_type {
    LIBUSB_TRANSFER_TYPE_CONTROL = 0,
    LIBUSB_TRANSFER_TYPE_ISOCHRONOUS = 1,
    LIBUSB_TRANSFER_TYPE_BULK = 2,
    LIBUSB_TRANSFER_TYPE_INTERRUPT = 3,
};

struct libusb_iso_packet_descriptor {
    unsigned int length;
    unsigned int actual_length;
    enum libusb_transfer_status status;
};

struct libusb_transfer;
typedef void (*libusb_transfer_cb_fn)(struct libusb_transfer *transfer);
struct libusb_transfer {
    libusb_device_handle *dev_handle;
    uint8_t flags;
    unsigned char endpoint;
    unsigned char type;
    unsigned int timeout;
    enum libusb_transfer_status status;
    int length;
    int actual_length;
    libusb_transfer_cb_fn callback;
    void *user_data;
    unsigned char *buffer;
    int num_iso_packets;
    struct libusb_iso_packet_descriptor iso_packet_desc[0];
};

int libusb_init(libusb_context **context);
void libusb_exit(struct libusb_context *ctx);
ssize_t libusb_get_device_list(libusb_context *ctx, libusb_device *** list);
void libusb_free_device_list(libusb_device **list, int unref_devices);
int libusb_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc);
int libusb_open(libusb_device *dev, libusb_device_handle **dev_handle);
void libusb_close(libusb_device_handle *dev_handle);
uint8_t libusb_get_device_address(libusb_device *dev);
uint8_t libusb_get_bus_number(libusb_device *dev);

struct libusb_transfer *libusb_alloc_transfer(int iso_packets);
void libusb_free_transfer(struct libusb_transfer *transfer);
int libusb_submit_transfer(struct libusb_transfer *transfer);
int libusb_cancel_transfer(struct libusb_transfer *transfer);
int libusb_control_transfer(libusb_device_handle *dev_handle,
    uint8_t bRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
    unsigned char *data, uint16_t wLength, unsigned int timeout);
int libusb_bulk_transfer(libusb_device_handle *dev_handle,
    unsigned char endpoint, unsigned char *data, int length,
    int *actual_length, unsigned int timeout);
int libusb_interrupt_transfer(libusb_device_handle *dev_handle,
    unsigned char endpoint, unsigned char *data, int length,
    int *actual_length, unsigned int timeout);

static inline void libusb_fill_control_transfer(
    struct libusb_transfer *transfer, libusb_device_handle *dev_handle,
    unsigned char *buffer, libusb_transfer_cb_fn callback, void *user_data,
    unsigned int timeout)
{
    // no idea why libusb_control_setup can not be used for c++ here
    struct usb_ctrlrequest_t *setup = (struct usb_ctrlrequest_t *)(void *)buffer;
    transfer->dev_handle = dev_handle;
    transfer->endpoint = 0;
    transfer->type = LIBUSB_TRANSFER_TYPE_CONTROL;
    transfer->timeout = timeout;
    transfer->buffer = buffer;
    if (setup)
        transfer->length = (int) (LIBUSB_CONTROL_SETUP_SIZE + le16_to_cpu(setup->wLength));
    transfer->user_data = user_data;
    transfer->callback = callback;
}

static inline void libusb_fill_bulk_transfer(struct libusb_transfer *transfer,
    libusb_device_handle *dev_handle, unsigned char endpoint,
    unsigned char *buffer, int length, libusb_transfer_cb_fn callback,
    void *user_data, unsigned int timeout)
{
    transfer->dev_handle = dev_handle;
    transfer->endpoint = endpoint;
    transfer->type = LIBUSB_TRANSFER_TYPE_BULK;
    transfer->timeout = timeout;
    transfer->buffer = buffer;
    transfer->length = length;
    transfer->user_data = user_data;
    transfer->callback = callback;
}

static inline void libusb_fill_interrupt_transfer(
    struct libusb_transfer *transfer, libusb_device_handle *dev_handle,
    unsigned char endpoint, unsigned char *buffer, int length,
    libusb_transfer_cb_fn callback, void *user_data, unsigned int timeout)
{
    transfer->dev_handle = dev_handle;
    transfer->endpoint = endpoint;
    transfer->type = LIBUSB_TRANSFER_TYPE_INTERRUPT;
    transfer->timeout = timeout;
    transfer->buffer = buffer;
    transfer->length = length;
    transfer->user_data = user_data;
    transfer->callback = callback;
}

static inline void libusb_fill_iso_transfer(struct libusb_transfer *transfer,
    libusb_device_handle *dev_handle, unsigned char endpoint,
    unsigned char *buffer, int length, int num_iso_packets,
    libusb_transfer_cb_fn callback, void *user_data, unsigned int timeout)
{
    transfer->dev_handle = dev_handle;
    transfer->endpoint = endpoint;
    transfer->type = LIBUSB_TRANSFER_TYPE_ISOCHRONOUS;
    transfer->timeout = timeout;
    transfer->buffer = buffer;
    transfer->length = length;
    transfer->num_iso_packets = num_iso_packets;
    transfer->user_data = user_data;
    transfer->callback = callback;
}

int libusb_get_string_descriptor_ascii(libusb_device_handle *dev_handle,
    uint8_t desc_index, unsigned char *data, int length);
int libusb_get_config_descriptor(libusb_device *dev, uint8_t config_index,
        struct libusb_config_descriptor **config);
int libusb_get_active_config_descriptor(libusb_device *dev,
        struct libusb_config_descriptor **config);
void libusb_free_config_descriptor(struct libusb_config_descriptor *config);
int libusb_get_descriptor(libusb_device_handle *dev_handle,
    uint8_t desc_type, uint8_t desc_index, unsigned char *data, int length);
int libusb_get_string_descriptor(libusb_device_handle *dev_handle,
    uint8_t desc_index, uint16_t langid, unsigned char *data, int length);
int libusb_set_interface_alt_setting(libusb_device_handle *dev_handle,
        int interface_number, int alternate_setting);
void libusb_free_pollfds(const struct libusb_pollfd **pollfds);

struct libusb_ss_endpoint_companion_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bMaxBurst;
    uint8_t  bmAttributes;
    uint16_t wBytesPerInterval;
} PACKED;

int libusb_get_ss_endpoint_companion_descriptor(
    struct libusb_context *ctx,
    const struct libusb_endpoint_descriptor *endpoint,
    struct libusb_ss_endpoint_companion_descriptor **ep_comp);
void libusb_free_ss_endpoint_companion_descriptor(
    struct libusb_ss_endpoint_companion_descriptor *ep_comp);

static inline unsigned char *libusb_get_iso_packet_buffer_simple(
    struct libusb_transfer *transfer, unsigned int packet)
{
    int _packet;
    if (packet > 0x80000000)
        return NULL;
    _packet = (int) packet;

    if (_packet >= transfer->num_iso_packets)
        return NULL;

    return transfer->buffer + ((int) transfer->iso_packet_desc[0].length * _packet);
}

static inline void libusb_set_iso_packet_lengths(
    struct libusb_transfer *transfer, unsigned int length)
{
    int i;
    for (i = 0; i < transfer->num_iso_packets; i++)
        transfer->iso_packet_desc[i].length = length;
}


void vsf_linux_libusb_startup(void);

#ifdef __cplusplus
}
#endif

#endif
