# HANDOFF — CMSIS-DAP/WinUSB prototype

更新时间：2026-10-30
分支：`pioarduino`
最后提交：`bd371850`
当前工作区：**未提交**（DAP 原型改动全部还在 working tree）

---

## 1. 目标

让 Python 脚本在同一个工作流里：

- 用 Bit Pirate 做 I2C/SPI/UART/GPIO；
- 用 pyOCD/OpenOCD 做目标芯片 SWD/JTAG 调试；
- 尽量只用一个 USB 设备。

当前约束：

- 保留 HID + MSC + CDC；
- 新增 CMSIS-DAP v2 vendor interface（WinUSB，不用 HID）；
- 先验证 Windows 能枚举、pyOCD 能打开，再接 SWD。

---

## 2. 当前结论

### 已完成

- 新增 `t-display-s3-dap` 构建环境。
- 保留 HID + MSC + CDC，新增 vendor interface。
- 新增最小 CMSIS-DAP 命令层：
  - `DAP_Info`
  - `DAP_Connect`
  - `DAP_Disconnect`
  - `DAP_TransferConfigure`
  - `DAP_TransferAbort`
  - `DAP_Delay`
  - `DAP_ResetTarget`
  - `DAP_SWJ_Pins`
  - `DAP_SWJ_Clock`
  - `DAP_SWJ_Sequence`
  - `DAP_SWD_Configure`
- 新增 `usb` 模式下的 `dap` 命令，用于确认接口状态。
- 新增 WCID（BOS + MS OS 2.0）代码，但当前默认关闭。
- 新增原生测试和 controller 测试。

### 验证结果

```text
native-tests:       647 / 647 passed
t-display-s3:       build success
t-display-s3-dap:   build success
DAP firmware:       3,962,848 B
app4M 剩余:         约 231 KB
```

### Windows 实机结果

已烧录过两版：

| 版本 | PID | WCID | Windows 结果 |
|---|---|---|---|
| DAP + WCID | `303A:1003` | 开 | USB Composite Device，Code 10 |
| DAP 无 WCID | `303A:1004` | 关 | USB Composite Device，Code 10 |
| 普通固件 | `303A:1002` | 无 | 正常（HID/MSC/CDC 可用） |

Code 10 的具体信息：

```text
Status:        Error
ProblemCode:   10  (CM_PROB_FAILED_START)
ProblemStatus: 0xC0000001 (STATUS_UNSUCCESSFUL)
Driver:        usb.inf / usbccgp
```

没有生成 `MI_00` / `MI_01` / `MI_02` 子设备，说明 `usbccgp` 在解析配置或启动接口阶段就失败了。

**WCID 已排除**：关掉 BOS/MS OS/device descriptor 覆盖后仍然 Code 10，所以问题在 vendor interface 或端点布局，不在 WCID 描述符。

当前设备运行的是 `303A:1004`，在 Windows 上是 `Not shared`，WSL 里没有对应 `/dev/ttyACM*`。

---

## 3. 当前 USB 形态

接口顺序：

| Interface | 功能 |
|---|---|
| 0 | MSC |
| 1 | HID（keyboard/mouse/gamepad/sysctrl 共用） |
| 2 | CMSIS-DAP v2 vendor / WinUSB 目标接口 |
| 3/4 | CDC control/data |

端点：

```text
HID      EP1 IN + EP1 OUT
MSC      EP2 IN + EP2 OUT
DAP      EP3 IN + EP4 OUT
CDC      OUT3, IN4, IN5
```

关键点：

- IN 端点用到 5/5：
  - EP1 HID
  - EP2 MSC
  - EP3 DAP
  - EP4 CDC data
  - EP5 CDC notification
- OUT 端点用到 4/6：
  - EP1 HID
  - EP2 MSC
  - EP3 CDC data
  - EP4 DAP
- DAP 的 `IN3` 与 CDC 的 `OUT3` 共用端点号；
- DAP 的 `OUT4` 与 CDC 的 `IN4` 共用端点号。

这个“跨方向同端点号”是当前 5 个 IN 端点预算下的必然结果。Windows Code 10 很可能与此有关，但还没有读到实际配置描述符确认。

---

## 4. 为什么 `usb dap` 不是必须的

Arduino `USB.h` 中：

```cpp
#define ARDUINO_USB_ON_BOOT (ARDUINO_USB_CDC_ON_BOOT | ARDUINO_USB_MSC_ON_BOOT | ARDUINO_USB_DFU_ON_BOOT)
```

项目定义了 `ARDUINO_USB_CDC_ON_BOOT=1`，所以 `app_main()` 在 `setup()` 前就会调用 `USB.begin()`。
也就是说，DAP 构建开机时就会枚举 HID/MSC/CDC/vendor；`usb dap` 只是确认状态，不是启动开关。

---

## 5. 当前代码状态

### 新增文件

| 文件 | 作用 |
|---|---|
| `src/Services/CmsisDapProtocol.h` | 纯 CMSIS-DAP 命令解析，可原生测试 |
| `src/Services/CmsisDapService.cpp` | vendor interface 注册、端点分配、DAP 收发、WCID 覆盖（当前关闭） |
| `test/Services/test_CmsisDapProtocol.cpp` | DAP parser 测试 |

### 修改文件

| 文件 | 改动 |
|---|---|
| `platformio.ini` | 新增 `t-display-s3-dap` |
| `src/Boards/Common/pins_arduino.h` | DAP PID / 产品名 |
| `src/Interfaces/IUsbS3Service.h` | 新增 `dapBegin()` |
| `src/Services/UsbS3Service.h/.cpp` | 实现 `dapBegin()` |
| `src/Controllers/UsbS3Controller.h/.cpp` | 新增 `usb dap` |
| `src/Shells/HelpShell.cpp` | 新增 `dap` 帮助行 |
| `src/Data/TranslationsHelp.h` | 中文帮助翻译 |
| `src/Data/TranslationsControllers.h` | 中文状态翻译 |
| `test/Services/FakeUsbS3Service.h` | fake `dapBegin()` |
| `test/Controllers/test_UsbS3Controller.cpp` | `dap` 命令测试 |
| `test/test_main.cpp` | 注册 parser 测试 |

### PID 规则

`src/Boards/Common/pins_arduino.h`：

```text
普通构建:              0x1002
BITPIRATE_USB_DAP=1
  BITPIRATE_USB_DAP_WCID=1:  0x1003
  BITPIRATE_USB_DAP_WCID=0:  0x1004
```

当前 `t-display-s3-dap` 使用：

```ini
-DBITPIRATE_USB_DAP=1
-DBITPIRATE_USB_DAP_WCID=0
```

### WCID 代码

`CmsisDapService.cpp` 中有一块 `#if BITPIRATE_USB_DAP_WCID`：

- 覆盖 `tud_descriptor_device_cb()`，把 `bcdUSB` 提到 `0x0201`；
- 覆盖 `tud_descriptor_bos_cb()`，返回 MS OS 2.0 BOS；
- 覆盖 `tud_vendor_control_xfer_cb()`，返回 MS OS 2.0 descriptor set；
- MS OS descriptor 的 `bFirstInterface` 在 vendor descriptor load 时写入实际 interface number。

这部分依赖项目全局的 `-Wl,-zmuldefs` 和“项目对象先于 Arduino core 链接”的顺序，属于原型 shortcut。开启后当前仍 Code 10，所以默认关闭。

---

## 6. 下一步：先读实际配置描述符

当前 1004 在 Windows 上未共享到 WSL。需要管理员 PowerShell：

```powershell
usbipd list
usbipd bind --busid 7-3
usbipd attach --wsl --busid 7-3
```

`busid` 以 `usbipd list` 里 `303a:1004` 那一行为准。
attach 成功后，WSL 会出现 `/sys/bus/usb/devices/1-1/descriptors`，直接解析二进制配置描述符，确认：

- interface number 是否真的是 0/1/2/3/4；
- vendor 是否真的是 IN3/OUT4；
- HID/MSC/CDC 描述符长度是否和 `wTotalLength` 一致；
- 是否存在 endpoint 地址重复或描述符长度错位。

如果不想 attach，也可以用 Windows 的 USBView / USBPcap；当前机器没装。

---

## 7. 下一步：端点隔离测试

按优先级：

### 7.1 试 DAP OUT5 / OUT6

把 vendor OUT 从 EP4 改成 EP5 或 EP6，避开与 CDC `IN4` 的同号复用：

```cpp
uint8_t epOut = tinyusb_get_free_out_endpoint();  // 4
epOut = tinyusb_get_free_out_endpoint();          // 5
```

只剩 `IN3` 与 CDC `OUT3` 同号。如果 Code 10 消失，说明 Windows/DCD 对某一方向的复用敏感。

### 7.2 临时去掉 CDC，验证是否端点复用导致

DAP + HID + MSC，无 CDC：

- 关闭 `ARDUINO_USB_CDC_ON_BOOT`
- 关闭 `CONFIG_TINYUSB_CDC_ENABLED`
- vendor 会拿到 `IN3 + OUT3` duplex，不再跨方向复用

如果这样能枚举，基本可以确定是 CDC 端点复用问题。

### 7.3 如果确认是端点复用

可选方案：

1. 去掉 HID 或 MSC，给 vendor 一个完整的 duplex 端点；
2. 自定义 CDC descriptor，把 CDC 的 OUT/IN 移到不与 vendor 冲突的端点；
3. 用第二块 ESP32-S3 刷 XRDAP-ESP/NexLink 做专用 probe；
4. 回到“单设备但不同时使用 I2C 和 debug”的 boot mode 方案。

### 7.4 如果配置描述符本身有错

优先检查：

- vendor descriptor 是否真的 23 字节；
- `bNumInterfaces` 是否为 5；
- CDC IAD 的 `bFirstInterface` / `bInterfaceCount`；
- 端点地址是否和 CDC 的硬编码 `0x03/0x84/0x85` 冲突；
- `TUD_VENDOR_DESCRIPTOR` 的 OUT/IN 顺序。

---

## 8. 常用命令

### 测试

```bash
~/.platformio/penv/bin/pio test -e native-tests
```

### 编译

```bash
~/.platformio/penv/bin/pio run -e t-display-s3
~/.platformio/penv/bin/pio run -e t-display-s3-dap
```

### 烧录

板子先进入下载模式：

1. 按住 BOOT；
2. 按一下 RST/EN；
3. 松开 BOOT；
4. 确认 `303A:1001` / `/dev/ttyACM0`。

```bash
~/.platformio/penv/bin/pio run -e t-display-s3-dap -t upload --upload-port /dev/ttyACM0
```

烧完后 RTS 硬复位不一定能让固件跑起来，需要手动按一下 RST/EN。

### WSL attach

```bash
usbipd.exe list
usbipd.exe attach --wsl --busid <busid>
```

### 设备端确认

进入 `usb` 模式后执行：

```text
dap
```

预期：

```text
USB DAP: CMSIS-DAP v2 / WinUSB interface started.
USB DAP: Run 'pyocd list' on the host.
```

---

## 9. 后续接 SWD 时要做什么

当前 `DAP_Transfer` 未实现，pyOCD 即使 open/connect 成功，读 DP/寄存器/内存也会失败。

需要补：

1. `DAP_Transfer` / `DAP_TransferBlock` 命令解析；
2. SWD 时序（GPIO 直接寄存器操作，独立任务/核心，短临界区）；
3. `DAP_SWJ_Sequence` 的真实 SWJ 序列；
4. `DAP_SWJ_Clock` 的时钟分频；
5. nRESET 引脚；
6. 目标 DP/AP 访问；
7. 多目标、JTAG、flash 算法暂不做。

T-Display S3 的默认 JTAG 扫描脚是：

```text
3, 10, 11, 12, 13, 16, 17
```

其中 16/17 是 I2C，10–13 是屏幕/SPI，3 是 SD/LoRa 相关。SWD 引脚需要和 I2C 同时使用时，必须另找空闲脚或改线。

---

## 10. 风险 / 注意事项

- 当前 DAP 构建**未通过 Windows 枚举验证**，不要把它当作可用功能。
- `IN3/OUT4` 是 5 个 IN 端点预算下的唯一选择；如果 Windows 不接受跨方向端点号复用，保留 HID+MSC+CDC 的单设备方案会卡住。
- WCID 覆盖依赖 `-zmuldefs` 链接顺序，升级 Arduino core 后可能失效。
- app4M 只剩约 231 KB，接完整 SWD 核心前建议改 6MB app 分区。
- `usb dap` 不是启动开关；USB 开机即启动。
- `eim_config.toml` 是本地文件，保持未跟踪，不要提交。
- 不要提交 Wi-Fi 密码、API key、私有网络地址。

---

## 11. 关键文件索引

```text
platformio.ini
src/Boards/Common/pins_arduino.h
src/Services/CmsisDapService.cpp
src/Services/CmsisDapProtocol.h
src/Services/UsbS3Service.cpp
src/Services/UsbS3Service.h
src/Interfaces/IUsbS3Service.h
src/Controllers/UsbS3Controller.cpp
src/Controllers/UsbS3Controller.h
src/Shells/HelpShell.cpp
src/Data/TranslationsHelp.h
src/Data/TranslationsControllers.h
test/Services/test_CmsisDapProtocol.cpp
test/Services/FakeUsbS3Service.h
test/Controllers/test_UsbS3Controller.cpp
test/test_main.cpp
```
