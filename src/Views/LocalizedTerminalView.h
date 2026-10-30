#pragma once

#include "Interfaces/ITerminalView.h"
#include "Services/LanguageService.h"

// Translates terminal text without changing the selected terminal backend.
class LocalizedTerminalView final : public ITerminalView {
public:
    explicit LocalizedTerminalView(ITerminalView& inner) : inner(inner) {}

    void initialize() override { inner.initialize(); }

    void welcome(TerminalTypeEnum& terminalType, std::string& terminalInfos) override {
        inner.welcome(terminalType, terminalInfos);
    }

    void print(const std::string& text) override {
        // print() is also used for raw protocol data; keep it untranslated.
        // Prompts that need translation go through UserInputManager or println().
        inner.print(text);
    }

    void print(const uint8_t data) override {
        inner.print(data);
    }

    void println(const std::string& text) override {
        if (GlobalState::getInstance().getLanguage() == LanguageEnum::English) {
            inner.println(text);
            return;
        }
        inner.println(LanguageService::translate(text));
    }

    void printPrompt(const std::string& mode = "HIZ") override {
        inner.printPrompt(mode);
    }

    void waitPress() override { inner.waitPress(); }
    void clear() override { inner.clear(); }

private:
    ITerminalView& inner;
};
