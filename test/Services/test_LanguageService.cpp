#include <unity.h>

#include "FakeNvsService.h"
#include "../States/TestGlobalState.h"
#include "../Views/FakeTerminalView.h"
#include "../../src/Views/LocalizedTerminalView.h"

void runLanguageServiceTests() {
    auto& state = GlobalState::getInstance();

    state.setLanguage(LanguageEnum::English);
    TEST_ASSERT_EQUAL_STRING(
        "System summary",
        LanguageService::translate("System summary").c_str()
    );

    state.setLanguage(LanguageEnum::Chinese);
    TEST_ASSERT_EQUAL_STRING(
        "系统摘要",
        LanguageService::translate("System summary").c_str()
    );
    TEST_ASSERT_EQUAL_STRING(
        "型号         : ESP32-S3",
        LanguageService::translate("Model         : ESP32-S3").c_str()
    );
    TEST_ASSERT_EQUAL_STRING(
        "\n=== 系统 Shell ===",
        LanguageService::translate("\n=== System Shell ===").c_str()
    );
    TEST_ASSERT_EQUAL_STRING(
        "scan                 - 扫描 1-Wire 设备",
        LanguageService::translate("scan                 - Scan 1-Wire devices").c_str()
    );
    TEST_ASSERT_EQUAL_STRING(
        "🚀 快速开始",
        LanguageService::translate("🚀 Quick start").c_str()
    );
    TEST_ASSERT_EQUAL_STRING(
        "未知命令。可用的 I2C 命令：",
        LanguageService::translate("Unknown command. Available I2C commands:").c_str()
    );
    TEST_ASSERT_EQUAL_STRING(
        "unknown-value",
        LanguageService::translate("unknown-value").c_str()
    );

    FakeTerminalView inner;
    LocalizedTerminalView view(inner);
    view.println("System summary");
    TEST_ASSERT_TRUE(inner.contains("系统摘要"));

    FakeNvsService nvs;
    LanguageService::save(nvs, LanguageEnum::Chinese);
    TEST_ASSERT_EQUAL_STRING("zh", nvs.values["language"].c_str());

    state.setLanguage(LanguageEnum::English);
    LanguageService::load(nvs);
    TEST_ASSERT_EQUAL(LanguageEnum::Chinese, state.getLanguage());
}
