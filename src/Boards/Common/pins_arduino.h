#pragma once

#include_next <pins_arduino.h>

// Override the default USB PID for boards to avoid conflicts with Windows drivers cache

#undef USB_PID
#ifdef BITPIRATE_USB_DAP
#if defined(BITPIRATE_USB_DAP_WCID) && BITPIRATE_USB_DAP_WCID
#define USB_PID 0x1003
#else
#define USB_PID 0x1004
#endif
#else
#define USB_PID 0x1002
#endif

#undef USB_PRODUCT
#ifdef BITPIRATE_USB_DAP
#define USB_PRODUCT "ESP32 Bit Pirate DAP"
#else
#define USB_PRODUCT "ESP32 Bit Pirate"
#endif
