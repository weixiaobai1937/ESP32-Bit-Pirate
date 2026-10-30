#pragma once

#include <string>

#include "Data/Translations.h"
#include "Enums/LanguageEnum.h"
#include "Interfaces/INvsService.h"
#include "States/GlobalState.h"

// Small runtime translation helper. English is the source language and the
// default; unknown strings pass through unchanged.
class LanguageService {
public:
    static constexpr const char* NVS_KEY = "language";

    static const char* tr(const char* text) {
        if (!text || GlobalState::getInstance().getLanguage() != LanguageEnum::Chinese) {
            return text;
        }
        const char* translated = lookupTranslation(text);
        return translated ? translated : text;
    }

    static std::string translate(const std::string& text) {
        if (text.empty() || GlobalState::getInstance().getLanguage() != LanguageEnum::Chinese) {
            return text;
        }

        std::string out;
        size_t start = 0;
        while (start < text.size()) {
            const size_t pos = text.find('\n', start);
            const size_t end = (pos == std::string::npos) ? text.size() : pos;
            out += translateLine(text.substr(start, end - start));
            if (pos == std::string::npos) break;
            out += '\n';
            start = pos + 1;
        }
        return out;
    }

    static void load(INvsService& nvs) {
        nvs.open();
        const std::string code = nvs.getString(NVS_KEY, "en");
        nvs.close();
        GlobalState::getInstance().setLanguage(LanguageEnumMapper::fromCode(code));
    }

    static void save(INvsService& nvs, LanguageEnum language) {
        nvs.open();
        nvs.saveString(NVS_KEY, LanguageEnumMapper::code(language));
        nvs.close();
        GlobalState::getInstance().setLanguage(language);
    }

private:
    static std::string translateLine(const std::string& line) {
        const size_t first = line.find_first_not_of(" \t\r");
        if (first == std::string::npos) return line;

        const size_t last = line.find_last_not_of(" \t\r");
        const std::string trimmed = line.substr(first, last - first + 1);

        if (const char* exact = lookupTranslation(trimmed.c_str())) {
            return line.substr(0, first) + exact + line.substr(last + 1);
        }

        // HelpShell lines are "command - description"; keep the command intact
        // and translate the description.
        const size_t dash = trimmed.find(" - ");
        if (dash != std::string::npos) {
            const std::string description = trimmed.substr(dash + 3);
            if (const char* translated = lookupTranslation(description.c_str())) {
                return line.substr(0, first) + trimmed.substr(0, dash + 3) +
                       translated + line.substr(last + 1);
            }
        }

        // Translate the label of "Label : value" lines while keeping the value.
        const size_t colon = trimmed.find(':');
        if (colon != std::string::npos) {
            std::string label = trimmed.substr(0, colon);
            const size_t labelFirst = label.find_first_not_of(" \t");
            if (labelFirst != std::string::npos) {
                const size_t labelLast = label.find_last_not_of(" \t");
                label = label.substr(labelFirst, labelLast - labelFirst + 1);
                if (const char* translated = lookupTranslation(label.c_str())) {
                    const size_t gapStart = labelLast + 1;
                    const size_t gapLength = colon - gapStart;
                    return line.substr(0, first) + translated +
                           trimmed.substr(gapStart, gapLength) +
                           trimmed.substr(colon) +
                           line.substr(last + 1);
                }
            }
        }

        return line;
    }
};
