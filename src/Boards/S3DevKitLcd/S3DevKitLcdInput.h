#pragma once

#ifdef DEVICE_S3DEVKIT_LCD

#include "Interfaces/IInput.h"
#include <RotaryEncoder.h>
#include <Arduino.h>

// EC11 on the LILYGO T-Embed pins: A=GPIO2, B=GPIO1, switch=GPIO0.
// GPIO0 is also the DevKit BOOT button (both are in parallel to GND), so the
// encoder push and BOOT both report KEY_OK. The RotaryEncoder library enables
// the internal pull-ups on A/B, and the switch uses INPUT_PULLUP here.
#define S3DEVKIT_LCD_ENC_A   2
#define S3DEVKIT_LCD_ENC_B   1
#define S3DEVKIT_LCD_ENC_SW  0

class S3DevKitLcdInput final : public IInput {
public:
    S3DevKitLcdInput();

    char handler() override;
    char readChar() override;
    void waitPress(uint32_t timeoutMs) override;

private:
    void tick();

    RotaryEncoder encoder;
    char lastInput;
    bool lastButton;
    int lastPos;
};

#endif
