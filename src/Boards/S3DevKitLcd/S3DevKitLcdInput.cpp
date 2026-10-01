#ifdef DEVICE_S3DEVKIT_LCD

#include "Boards/S3DevKitLcd/S3DevKitLcdInput.h"
#include "Data/InputKeys.h"
#include <Arduino.h>

S3DevKitLcdInput::S3DevKitLcdInput()
    : encoder(S3DEVKIT_LCD_ENC_A, S3DEVKIT_LCD_ENC_B, RotaryEncoder::LatchMode::TWO03),
      lastInput(KEY_NONE),
      lastButton(false),
      lastPos(0)
{
    encoder.setPosition(0);
    pinMode(S3DEVKIT_LCD_ENC_SW, INPUT_PULLUP);
}

void S3DevKitLcdInput::tick() {
    encoder.tick();

    int pos = encoder.getPosition();
    if (pos < lastPos) {
        lastInput = KEY_ARROW_RIGHT;
        lastPos = pos;
    } else if (pos > lastPos) {
        lastInput = KEY_ARROW_LEFT;
        lastPos = pos;
    } else if (!digitalRead(S3DEVKIT_LCD_ENC_SW) && !lastButton) {
        lastInput = KEY_OK;
        lastButton = true;
    } else if (digitalRead(S3DEVKIT_LCD_ENC_SW)) {
        lastButton = false;
    }
}

char S3DevKitLcdInput::readChar() {
    tick();
    char c = lastInput;
    lastInput = KEY_NONE;
    return c;
}

char S3DevKitLcdInput::handler() {
    while (true) {
        char c = readChar();
        if (c != KEY_NONE) return c;
        delay(5);
    }
}

void S3DevKitLcdInput::waitPress(uint32_t timeoutMs) {
    uint32_t start = millis();
    while (true) {
        if (readChar() != KEY_NONE) return;
        if (timeoutMs > 0 && (millis() - start) >= timeoutMs) return;
        delay(5);
    }
}

#endif
