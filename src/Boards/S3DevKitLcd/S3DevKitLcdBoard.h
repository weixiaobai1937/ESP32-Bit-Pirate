#pragma once

#ifdef DEVICE_S3DEVKIT_LCD

#include "Boards/Common/Views/St7789SpiDeviceView.h"
#include "Boards/S3DevKitLcd/S3DevKitLcdInput.h"
#include "Boards/Common/Serial/BoardHostSerial.h"

class S3DevKitLcdBoard final {
public:
    S3DevKitLcdBoard();

    void initialize();
    IDeviceView& getDeviceView();
    IInput& getDeviceInput();
    IHostSerial& getHostSerial();

private:
    static St7789SpiConfig createDisplayConfig();

    BoardHostSerial hostSerial;
    St7789SpiConfig displayConfig;
    St7789SpiDeviceView deviceView;
    S3DevKitLcdInput deviceInput;
};

#endif
