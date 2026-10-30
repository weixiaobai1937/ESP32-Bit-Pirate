# AGENTS.md

Instructions for coding agents working in this repository. Read this file and
`HANDOFF.md` before starting a task.

## Project

ESP32 Bit Pirate is Arduino/PlatformIO firmware for ESP32-S3 boards. The main
loop reads terminal commands and dispatches them through controllers, services,
shells, and views. It is not a multi-task application; USB, Wi-Fi, and a few
services run small background tasks.

- Branch: `pioarduino`
- Last known good commit: `bd371850`
- Build system: PlatformIO with the pinned pioarduino platform in
  `platformio.ini`
- Language: C++17 (native tests use `-std=gnu++17`)
- Test framework: Unity on the host

Start with:

- `README.md`
- `CONTRIBUTING.md`
- `docs/ARCHITECTURE.zh-CN.md`
- `platformio.ini`

## Commands

PlatformIO is installed in the project environment:

```bash
~/.platformio/penv/bin/pio test -e native-tests
~/.platformio/penv/bin/pio run -e t-display-s3
~/.platformio/penv/bin/pio run -e t-display-s3-dap
```

As of 2026-10-30, native tests are `647/647` and both T-Display builds pass.

Flash the T-Display S3:

```bash
~/.platformio/penv/bin/pio run -e t-display-s3-dap -t upload --upload-port /dev/ttyACM0
```

The board must be in USB download mode (`303A:1001`, `/dev/ttyACM0`). Hold
**BOOT**, press **RST/EN**, release **BOOT**. RTS hard reset does not boot this
board in the current WSL/USBIP setup; press **RST/EN** after flashing.

In WSL, attach a shared device with:

```bash
usbipd.exe list
usbipd.exe attach --wsl --busid <busid>
```

`usbipd bind` requires an Administrator PowerShell.

## Code layout

| Path | Responsibility |
|---|---|
| `src/Controllers/` | Command workflows, validation, coordination |
| `src/Services/` | Hardware/protocol implementations |
| `src/Shells/` | User-facing command groups |
| `src/Views/` | Terminal/display output |
| `src/Inputs/` | Terminal/physical input |
| `src/Interfaces/` | Contracts between components |
| `src/Providers/` | Dependency construction and shared instances |
| `src/States/` | Global runtime state |
| `src/Data/` | Static tables, including translations |
| `test/` | Native Unity tests and fakes |

Keep the existing layering: Controller -> Service -> hardware. Do not put
hardware access directly in a controller. Use interfaces and fakes so native
tests still compile.

## Conventions

- Keep changes small and consistent with surrounding code.
- Avoid new global/static allocations, containers, or large buffers. RAM is
  tight; prefer `constexpr`, flash-resident tables, and stack buffers.
- Do not add dependencies for small tasks.
- Run `git diff --check` before finishing.
- Do not commit `.pio/`, build artifacts, local logs, or `eim_config.toml`.
  `eim_config.toml` is intentionally local and untracked.
- Do not include Wi-Fi passwords, API keys, or private network details in
  committed files.

## Localization

UI strings are translated at runtime by `LanguageService` using exact English
keys in:

- `src/Data/Translations.h`
- `src/Data/TranslationsHelp.h`
- `src/Data/TranslationsGuide.h`
- `src/Data/TranslationsShells.h`
- `src/Data/TranslationsControllers.h`

Rules:

- Add translation entries in sorted order; help/controller tables use binary
  search.
- Keep CLI commands and arguments in English. Translate only descriptions and
  explanatory fields.
- When adding a HelpShell line, update both `src/Shells/HelpShell.cpp` and the
  relevant translation table.
- Chinese UI is selected at runtime and persisted in NVS under `language`.

## Tests

Native tests include production `.cpp` files from `test/test_main.cpp`; add new
suites there. When changing an interface:

- update the real implementation;
- update the fake in `test/Services/`;
- add or adjust the controller test.

Run:

```bash
~/.platformio/penv/bin/pio test -e native-tests
```

## USB and CMSIS-DAP prototype

The experimental `t-display-s3-dap` environment adds a vendor interface for
CMSIS-DAP v2 while keeping HID, MSC, and CDC.

Current endpoint layout:

```text
HID      EP1 IN + EP1 OUT
MSC      EP2 IN + EP2 OUT
DAP      EP3 IN + EP4 OUT
CDC      OUT3, IN4, IN5
```

PID mapping in `src/Boards/Common/pins_arduino.h`:

| Build | PID | Notes |
|---|---|---|
| normal | `0x1002` | HID + MSC + CDC |
| DAP + WCID | `0x1003` | `BITPIRATE_USB_DAP_WCID=1` |
| DAP without WCID | `0x1004` | Current default (`BITPIRATE_USB_DAP_WCID=0`) |

Current known issue: Windows enumerates `303A:1003` and `303A:1004` as a USB
Composite Device but reports Code 10 (`CM_PROB_FAILED_START`,
`STATUS_UNSUCCESSFUL`). The normal `303A:1002` build works. Disabling WCID did
not fix it, so the failure is in the vendor interface or its endpoint layout.
See `HANDOFF.md` for the diagnosis and next tests.

The DAP layer is intentionally incomplete:

- `src/Services/CmsisDapProtocol.h` handles discovery, connect/disconnect, and
  basic configuration only.
- `DAP_Transfer` / `DAP_TransferBlock` are not implemented yet; SWD register
  and memory access will fail until they are.
- `usb dap` inside USB mode starts/confirms the interface; USB already starts
  at boot because `ARDUINO_USB_ON_BOOT` is derived from
  `ARDUINO_USB_CDC_ON_BOOT` in `USB.h`.

WCID overrides in `CmsisDapService.cpp` target interface 2 and rely on the
project's global `-Wl,-zmuldefs` plus project objects linking before the
Arduino core. This is a prototype shortcut; replace it with custom TinyUSB
descriptors before treating it as production code.

## Hardware notes

- T-Display S3 uses `app4M_spiffs_12M_16MB.csv`; the current DAP image is about
  3.96 MB, leaving roughly 231 KB free in the app partition.
- T-Display pins in use include the display SPI bus and I2C on GPIO 16/17.
  Check `PROTECTED_PINS` in `platformio.ini` before assigning SWD/JTAG pins.
- Do not assume the target MCU responds to I2C while halted; its firmware stops
  when the core is halted.
