#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

// Minimal CMSIS-DAP command layer. This only answers probe discovery and
// connect/disconnect so pyOCD can open the interface; SWD/JTAG transfers are
// added later.

struct CmsisDapInfo {
    const char* vendor;
    const char* product;
    const char* serial;
    uint8_t capabilities;
    uint16_t maxPacketSize;
    uint8_t maxPacketCount;
};

enum : uint8_t {
    CMSIS_DAP_INFO = 0x00,
    CMSIS_DAP_CONNECT = 0x02,
    CMSIS_DAP_DISCONNECT = 0x03,
    CMSIS_DAP_TRANSFER_CONFIGURE = 0x04,
    CMSIS_DAP_TRANSFER_ABORT = 0x07,
    CMSIS_DAP_DELAY = 0x09,
    CMSIS_DAP_RESET_TARGET = 0x0A,
    CMSIS_DAP_SWJ_PINS = 0x10,
    CMSIS_DAP_SWJ_CLOCK = 0x11,
    CMSIS_DAP_SWJ_SEQUENCE = 0x12,
    CMSIS_DAP_SWD_CONFIGURE = 0x13,
};

inline size_t cmsisDapStringInfo(const char* value, uint8_t* response, size_t responseSize) {
    const size_t length = value ? std::strlen(value) + 1 : 0;
    if (length + 2 > responseSize) {
        return 0;
    }
    response[0] = CMSIS_DAP_INFO;
    response[1] = static_cast<uint8_t>(length);
    if (length) {
        std::memcpy(response + 2, value, length);
    }
    return length + 2;
}

inline size_t cmsisDapHandleCommand(const uint8_t* request,
                                    size_t requestSize,
                                    uint8_t* response,
                                    size_t responseSize,
                                    const CmsisDapInfo& info) {
    if (!request || !response || requestSize < 1 || responseSize < 2) {
        return 0;
    }

    switch (request[0]) {
        case CMSIS_DAP_INFO: {
            if (requestSize < 2) {
                return 0;
            }
            switch (request[1]) {
                case 0x01: return cmsisDapStringInfo(info.vendor, response, responseSize);
                case 0x02: return cmsisDapStringInfo(info.product, response, responseSize);
                case 0x03: return cmsisDapStringInfo(info.serial, response, responseSize);
                case 0x04: return cmsisDapStringInfo("2.0.0", response, responseSize);
                case 0x05:  // Current SWJ mode: SWD
                    if (responseSize < 3) return 0;
                    response[0] = CMSIS_DAP_INFO;
                    response[1] = 1;
                    response[2] = 1;
                    return 3;
                case 0xF0:
                    if (responseSize < 3) return 0;
                    response[0] = CMSIS_DAP_INFO;
                    response[1] = 1;
                    response[2] = info.capabilities;
                    return 3;
                case 0xFE:
                    if (responseSize < 3) return 0;
                    response[0] = CMSIS_DAP_INFO;
                    response[1] = 1;
                    response[2] = info.maxPacketCount;
                    return 3;
                case 0xFF:
                    if (responseSize < 4) return 0;
                    response[0] = CMSIS_DAP_INFO;
                    response[1] = 2;
                    response[2] = static_cast<uint8_t>(info.maxPacketSize & 0xFF);
                    response[3] = static_cast<uint8_t>(info.maxPacketSize >> 8);
                    return 4;
                default:
                    response[0] = CMSIS_DAP_INFO;
                    response[1] = 0;
                    return 2;
            }
        }

        case CMSIS_DAP_CONNECT: {
            const uint8_t port = requestSize >= 2 ? request[1] : 0;
            response[0] = CMSIS_DAP_CONNECT;
            response[1] = (port == 0 || port == 1) ? 1 : 0;
            return 2;
        }

        case CMSIS_DAP_DISCONNECT:
            response[0] = CMSIS_DAP_DISCONNECT;
            response[1] = 1;
            return 2;

        case CMSIS_DAP_TRANSFER_CONFIGURE:
            response[0] = CMSIS_DAP_TRANSFER_CONFIGURE;
            response[1] = 1;
            return 2;

        case CMSIS_DAP_TRANSFER_ABORT:
            response[0] = CMSIS_DAP_TRANSFER_ABORT;
            response[1] = 1;
            return 2;

        case CMSIS_DAP_DELAY:
            response[0] = CMSIS_DAP_DELAY;
            response[1] = 1;
            return 2;

        case CMSIS_DAP_RESET_TARGET:
            if (responseSize < 3) return 0;
            response[0] = CMSIS_DAP_RESET_TARGET;
            response[1] = 1;
            response[2] = 0;
            return 3;

        case CMSIS_DAP_SWJ_PINS:
            response[0] = CMSIS_DAP_SWJ_PINS;
            response[1] = 0;
            return 2;

        case CMSIS_DAP_SWJ_CLOCK:
            response[0] = CMSIS_DAP_SWJ_CLOCK;
            response[1] = 1;
            return 2;

        case CMSIS_DAP_SWJ_SEQUENCE:
            response[0] = CMSIS_DAP_SWJ_SEQUENCE;
            response[1] = 1;
            return 2;

        case CMSIS_DAP_SWD_CONFIGURE:
            response[0] = CMSIS_DAP_SWD_CONFIGURE;
            response[1] = 1;
            return 2;

        default:
            response[0] = request[0];
            response[1] = 0;
            return 2;
    }
}
