#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <string>

// Flash-resident English -> Chinese dictionary.
// Keys are the exact UI strings (without surrounding whitespace).
struct TranslationEntry {
    const char* en;
    const char* zh;
};

#include "TranslationsHelp.h"
#include "TranslationsGuide.h"
#include "TranslationsShells.h"
#include "TranslationsControllers.h"

inline constexpr TranslationEntry translations[] = {
    // System Shell menu
    {"System summary", "系统摘要"},
    {"Hardware info", "硬件信息"},
    {"Memory", "内存"},
    {"Screen", "屏幕"},
    {"Partitions", "分区"},
    {"LittleFS", "LittleFS"},
    {"NVS", "NVS"},
    {"Network", "网络"},
    {"Debug", "调试"},
    {"Reboot", "重启"},
    {"Bootloader", "Bootloader"},
    {"Language", "语言"},
    {"Exit", "退出"},
    {"📊 System summary", "📊 系统摘要"},
    {"📟 Hardware info", "📟 硬件信息"},
    {"📦 Memory", "📦 内存"},
    {"📺 Screen", "📺 屏幕"},
    {"🧩 Partitions", "🧩 分区"},
    {"📁 LittleFS", "📁 LittleFS"},
    {"🧰 NVS", "🧰 NVS"},
    {"🌐 Network", "🌐 网络"},
    {"🐞 Debug", "🐞 调试"},
    {"🔄 Reboot", "🔄 重启"},
    {"🔌 Bootloader", "🔌 Bootloader"},
    {"🌐 Language", "🌐 语言"},
    {"🚪 Exit", "🚪 退出"},

    // Section headers
    {"=== System Shell ===", "=== 系统 Shell ==="},
    {"=== System Summary ===", "=== 系统摘要 ==="},
    {"=== Hardware Info ===", "=== 硬件信息 ==="},
    {"=== Memory ===", "=== 内存 ==="},
    {"=== Device Screen ===", "=== 设备屏幕 ==="},
    {"=== Partitions ===", "=== 分区 ==="},
    {"=== LittleFS ===", "=== LittleFS ==="},
    {"=== LittleFS Manager ===", "=== LittleFS 管理 ==="},
    {"=== LittleFS Files ===", "=== LittleFS 文件 ==="},
    {"=== LittleFS Delete ===", "=== LittleFS 删除 ==="},
    {"=== Non Volatile Storage ===", "=== 非易失存储 (NVS) ==="},
    {"=== Network Info ===", "=== 网络信息 ==="},
    {"=== Debug Logs ===", "=== 调试日志 ==="},
    {"=== USB Bootloader Mode ===", "=== USB Bootloader 模式 ==="},

    // System summary / hardware / memory labels
    {"Model", "型号"},
    {"Uptime", "运行时间"},
    {"Temperature", "温度"},
    {"Screen", "屏幕"},
    {"Reset reason", "复位原因"},
    {"Stack total", "栈总量"},
    {"Heap total", "堆总量"},
    {"PSRAM total", "PSRAM 总量"},
    {"Flash total", "Flash 总量"},
    {"Firmware", "固件"},
    {"Build date", "编译日期"},
    {"Infrared", "红外"},
    {"IDF version", "IDF 版本"},
    {"Arduino core", "Arduino 核心"},
    {"CPU cores", "CPU 核心数"},
    {"CPU freq", "CPU 频率"},
    {"CPU temp", "CPU 温度"},
    {"Features", "特性"},
    {"Revision", "版本号"},
    {"Full revision", "完整版本号"},
    {"Flash speed", "Flash 速度"},
    {"Flash mode", "Flash 模式"},
    {"Flash chip ID", "Flash 芯片 ID"},
    {"Sketch total", "固件大小"},
    {"Sketch free", "固件剩余"},
    {"Sketch usage", "固件占用"},
    {"Sketch MD5", "固件 MD5"},
    {"Heap free", "堆空闲"},
    {"Heap used", "堆已用"},
    {"Min free heap", "最小空闲堆"},
    {"Max alloc heap", "最大可分配堆"},
    {"PSRAM free", "PSRAM 空闲"},
    {"PSRAM used", "PSRAM 已用"},
    {"Min free PSRAM", "最小空闲 PSRAM"},
    {"Max alloc PSRAM", "最大可分配 PSRAM"},
    {"Brightness", "亮度"},
    {"Set brightness (0-100%)", "设置亮度 (0-100%)"},
    {"Total", "总计"},
    {"Used", "已用"},
    {"Free", "空闲"},
    {"Files", "文件数"},
    {"Base MAC", "基础 MAC"},
    {"AP MAC", "AP MAC"},
    {"STA MAC", "STA MAC"},
    {"IP", "IP"},
    {"Subnet", "子网"},
    {"Gateway", "网关"},
    {"DNS1", "DNS1"},
    {"DNS2", "DNS2"},
    {"Hostname", "主机名"},
    {"SSID", "SSID"},
    {"BSSID", "BSSID"},
    {"RSSI", "RSSI"},
    {"Channel", "信道"},
    {"Mode", "模式"},
    {"Status", "状态"},
    {"Prov enabled", "配网已启用"},

    // Reset reasons / short values
    {"Power-On", "上电"},
    {"External", "外部复位"},
    {"Software", "软件复位"},
    {"Panic", "异常"},
    {"Interrupt WDT", "中断看门狗"},
    {"Task WDT", "任务看门狗"},
    {"Other WDT", "其他看门狗"},
    {"Deep-Sleep Wake", "深睡眠唤醒"},
    {"Brownout", "欠压"},
    {"SDIO", "SDIO"},
    {"Unknown", "未知"},
    {"version", "版本"},
    {"current", "当前"},
    {"bright", "亮度"},
    {"Yes", "是"},
    {"No", "否"},

    // Prompts and messages
    {"Select action", "选择操作"},
    {"Select language", "选择语言"},
    {"Language saved.", "语言已保存。"},
    {"Exiting System Shell...", "正在退出系统 Shell..."},
    {"Exiting LittleFS...", "正在退出 LittleFS..."},
    {"Enter LittleFS manager?", "进入 LittleFS 管理器？"},
    {"LittleFS mounted successfully.", "LittleFS 挂载成功。"},
    {"LittleFS not mounted.", "LittleFS 未挂载。"},
    {"Failed to get LittleFS space info.", "获取 LittleFS 空间信息失败。"},
    {"No files in LittleFS.", "LittleFS 中没有文件。"},
    {"No files to delete.", "没有可删除的文件。"},
    {"LittleFS: Exiting delete menu...", "LittleFS：正在退出删除菜单..."},
    {"Select file to delete", "选择要删除的文件"},
    {"Delete cancelled.", "删除已取消。"},
    {"File does not exist anymore.", "文件已不存在。"},
    {"Delete failed.", "删除失败。"},
    {"Deleted", "已删除"},
    {"Failed", "失败"},
    {"Deleting files...", "正在删除文件..."},
    {"Delete ALL complete.", "全部删除完成。"},
    {"This will delete ALL files in LittleFS.", "这将删除 LittleFS 中的所有文件。"},
    {"Type 'DELETE ALL' to confirm.", "输入 'DELETE ALL' 以确认。"},
    {"Confirm", "确认"},
    {"Debug output enabled. Logs will now be printed to serial.", "调试输出已启用，日志将打印到串口。"},
    {"This will enable debug output to serial,", "这将启用串口调试输出，"},
    {"allow you to see the ESP debug logs.", "以便查看 ESP 调试日志。"},
    {"Enable debug output to serial?", "启用串口调试输出？"},
    {"This mode restarts the ESP32 into USB BOOT MODE.", "此模式会将 ESP32 重启进入 USB BOOT 模式。"},
    {"Use it when you want to flash the device firmware.", "需要烧录设备固件时使用。"},
    {"Rebooting, your session will be lost...", "正在重启，当前会话将丢失..."},
    {"Rebooting into USB bootloader mode, the terminal will close...", "正在重启进入 USB Bootloader 模式，终端将关闭..."},
};

template <size_t N>
inline const char* lookupInSorted(const TranslationEntry (&table)[N], const char* text) {
    const TranslationEntry* begin = table;
    const TranslationEntry* end = table + N;
    const TranslationEntry* it = std::lower_bound(
        begin, end, text,
        [](const TranslationEntry& entry, const char* value) {
            return std::strcmp(entry.en, value) < 0;
        }
    );
    if (it != end && std::strcmp(it->en, text) == 0) return it->zh;
    return nullptr;
}

inline const char* lookupTranslation(const char* text) {
    if (!text) return nullptr;
    for (const auto& entry : translations) {
        if (std::strcmp(entry.en, text) == 0) return entry.zh;
    }
    if (const char* translated = lookupInSorted(helpTranslations, text)) return translated;
    if (const char* translated = lookupInSorted(guideTranslations, text)) return translated;
    if (const char* translated = lookupInSorted(shellTranslations, text)) return translated;
    if (const char* translated = lookupInSorted(controllerTranslations, text)) return translated;
    return nullptr;
}
