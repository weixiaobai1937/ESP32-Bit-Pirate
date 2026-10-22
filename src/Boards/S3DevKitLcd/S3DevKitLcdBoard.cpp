#ifdef DEVICE_S3DEVKIT_LCD

#include "Boards/S3DevKitLcd/S3DevKitLcdBoard.h"
#include <Arduino.h>

St7789SpiConfig S3DevKitLcdBoard::createDisplayConfig() {
    St7789SpiConfig config;

    // The Bit Pirate target bus always uses Arduino's SPI object (FSPI /
    // SPI2_HOST on the ESP32-S3), so the panel is driven by the other general
    // purpose controller (HSPI / SPI3_HOST) to keep the two buses apart.
    // Services therefore keep using Arduino SPI (getSharedSpiInstance()).
    config.spiHost = SPI3_HOST;
    config.useSharedSpi = false;

    // ST7789 dupont wiring, same pins as the LILYGO T-Embed S3 profile.
    config.pinSclk = 12;
    config.pinMosi = 11;
    config.pinMiso = -1;
    config.pinCs = 10;
    config.pinDc = 13;
    config.pinReset = 9;
    config.pinBacklight = 15;
    config.pinPower = -1;

    // 240x280 visible panel inside a 240x320 controller RAM (Y gap of 20).
    config.panelWidth = 240;
    config.panelHeight = 280;
    config.memoryWidth = 240;
    config.memoryHeight = 320;
    config.offsetX = 0;
    config.offsetY = 20;

    config.writeFrequency = 40000000;
    config.rotation = 0;
    config.invert = true;
    config.rgbOrder = true;
    config.selectionHelpLine1 = "Rotate: move  Press: OK";

    return config;
}

S3DevKitLcdBoard::S3DevKitLcdBoard()
    : displayConfig(createDisplayConfig()),
      deviceView(displayConfig) {}

void S3DevKitLcdBoard::initialize() {
    deviceView.initialize();
    deviceView.logo();
    deviceInput.waitPress(3000);
    deviceView.clear();
}

IDeviceView& S3DevKitLcdBoard::getDeviceView() {
    return deviceView;
}

IInput& S3DevKitLcdBoard::getDeviceInput() {
    return deviceInput;
}

IHostSerial& S3DevKitLcdBoard::getHostSerial() {
    return hostSerial;
}

#endif
