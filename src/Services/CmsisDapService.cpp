#include "CmsisDapProtocol.h"

#if defined(BITPIRATE_USB_DAP) && BITPIRATE_USB_DAP

#include <Arduino.h>
#include <USB.h>
#include <esp32-hal-tinyusb.h>
#include <cstring>

namespace {

constexpr uint8_t DAP_PACKET_SIZE = 64;

#if BITPIRATE_USB_DAP_WCID
constexpr uint8_t MS_OS_VENDOR_CODE = 2;

// MS OS 2.0 descriptor set. Byte 20 is the vendor interface number and is
// filled in when the interface descriptor is loaded.
uint8_t msOs20Descriptor[] = {
    0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x06, 0x2E, 0x00,
    0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x24, 0x00,
    0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1C, 0x00,
    0x14, 0x00, 0x03, 0x00,
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static_assert(sizeof(msOs20Descriptor) == 0x2E, "MS OS 2.0 descriptor size");

uint8_t bosDescriptor[] = {
    0x05, 0x0F, 0x21, 0x00, 0x01,
    0x1C, 0x10, 0x05, 0x00,
    0xDF, 0x60, 0xDD, 0xD8, 0x89, 0x45, 0xC7, 0x4C,
    0x9C, 0xD2, 0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F,
    0x00, 0x00, 0x03, 0x06,
    0x2E, 0x00,
    MS_OS_VENDOR_CODE, 0x00,
};
static_assert(sizeof(bosDescriptor) == 33, "BOS descriptor size");
#endif

const char* dapSerial() {
    const char* serial = USB.serialNumber();
    return (serial && *serial) ? serial : "BP-DAP";
}

void sendDapResponse(const uint8_t* data, size_t length) {
    if (!length) {
        return;
    }
    tud_vendor_n_write(0, data, length);
#if CFG_TUD_VENDOR_TXRX_BUFFERED
    tud_vendor_n_write_flush(0);
#endif
}

}  // namespace

extern "C" uint16_t cmsis_dap_load_descriptor(uint8_t* destination, uint8_t* interfaceNumber) {
    const uint8_t stringIndex = tinyusb_add_string_descriptor("CMSIS-DAP v2");
    const uint8_t epIn = tinyusb_get_free_in_endpoint();
    const uint8_t epOut = tinyusb_get_free_out_endpoint();
    if (!epIn || !epOut) {
        return 0;
    }

#if BITPIRATE_USB_DAP_WCID
    msOs20Descriptor[20] = *interfaceNumber;
#endif

    uint8_t descriptor[TUD_VENDOR_DESC_LEN] = {
        TUD_VENDOR_DESCRIPTOR(*interfaceNumber, stringIndex, epOut, (uint8_t)(0x80 | epIn), DAP_PACKET_SIZE)
    };
    *interfaceNumber += 1;
    std::memcpy(destination, descriptor, sizeof(descriptor));
    return sizeof(descriptor);
}

extern "C" void tud_vendor_rx_cb(uint8_t itf, const uint8_t* buffer, uint32_t bufferSize) {
    (void)itf;

    uint8_t request[DAP_PACKET_SIZE];
    size_t length = 0;
    if (buffer && bufferSize) {
        length = bufferSize > sizeof(request) ? sizeof(request) : bufferSize;
        std::memcpy(request, buffer, length);
    }
#if CFG_TUD_VENDOR_TXRX_BUFFERED
    else {
        length = tud_vendor_n_read(0, request, sizeof(request));
    }
#endif
    if (!length) {
        return;
    }

    const CmsisDapInfo info = {
        "Bit Pirate",
        "Bit Pirate CMSIS-DAP",
        dapSerial(),
        0x01,  // SWD only
        DAP_PACKET_SIZE,
        1,
    };

    uint8_t response[DAP_PACKET_SIZE];
    const size_t responseLength =
        cmsisDapHandleCommand(request, length, response, sizeof(response), info);
    sendDapResponse(response, responseLength);
}

#if BITPIRATE_USB_DAP_WCID
// USB starts at boot, before UsbS3Service::configure() can raise bcdUSB to
// 2.1. Override the device descriptor so Windows requests the BOS descriptor.
extern "C" uint8_t const* tud_descriptor_device_cb(void) {
    static tusb_desc_device_t descriptor;
    descriptor.bLength = sizeof(tusb_desc_device_t);
    descriptor.bDescriptorType = TUSB_DESC_DEVICE;
    descriptor.bcdUSB = 0x0201;
    descriptor.bDeviceClass = USB.usbClass();
    descriptor.bDeviceSubClass = USB.usbSubClass();
    descriptor.bDeviceProtocol = USB.usbProtocol();
    descriptor.bMaxPacketSize0 = CFG_TUD_ENDOINT0_SIZE;
    descriptor.idVendor = USB.VID();
    descriptor.idProduct = USB.PID();
    descriptor.bcdDevice = USB.firmwareVersion();
    descriptor.iManufacturer = 0x01;
    descriptor.iProduct = 0x02;
    descriptor.iSerialNumber = 0x03;
    descriptor.bNumConfigurations = 0x01;
    return reinterpret_cast<uint8_t const*>(&descriptor);
}

// Strong overrides for the Arduino core's BOS/MS OS descriptors. The core's
// descriptor targets interface 0, which is MSC on this composite device.
// shortcut: relies on the project's -zmuldefs and project objects linking
// before the Arduino core; replace with custom TinyUSB descriptors later.
extern "C" uint8_t const* tud_descriptor_bos_cb(void) {
    return bosDescriptor;
}

extern "C" bool tud_vendor_control_xfer_cb(uint8_t rhport,
                                           uint8_t stage,
                                           tusb_control_request_t const* request) {
    if (request && request->bmRequestType_bit.type == TUSB_REQ_TYPE_VENDOR &&
        request->bRequest == MS_OS_VENDOR_CODE && request->wIndex == 7) {
        if (stage == CONTROL_STAGE_SETUP) {
            return tud_control_xfer(rhport, request, msOs20Descriptor, sizeof(msOs20Descriptor));
        }
        return true;
    }
    return false;
}
#endif  // BITPIRATE_USB_DAP_WCID

namespace {
struct CmsisDapUsbRegistration {
    CmsisDapUsbRegistration() {
        tinyusb_enable_interface(
            USB_INTERFACE_VENDOR, TUD_VENDOR_DESC_LEN, cmsis_dap_load_descriptor);
    }
} cmsisDapUsbRegistration;
}  // namespace

#endif  // BITPIRATE_USB_DAP
