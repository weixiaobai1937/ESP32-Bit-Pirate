#pragma once

#include <cstdint>
#include <string>

enum class LanguageEnum : uint8_t {
    English = 0,
    Chinese = 1
};

class LanguageEnumMapper {
public:
    static const char* code(LanguageEnum language) {
        return language == LanguageEnum::Chinese ? "zh" : "en";
    }

    static LanguageEnum fromCode(const std::string& code) {
        return code == "zh" ? LanguageEnum::Chinese : LanguageEnum::English;
    }

    static const char* nativeName(LanguageEnum language) {
        return language == LanguageEnum::Chinese ? "中文" : "English";
    }
};
