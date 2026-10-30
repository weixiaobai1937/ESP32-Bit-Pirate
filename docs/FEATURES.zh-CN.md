# ESP32 Bit Pirate 功能总览

> 版本：固件 `1.8`（`GlobalState`）  
> 代码基线：分支 `pioarduino`，提交 `724f1097`  
> 范围：当前仓库内**已实现**的固件功能；`src/` 中明确标注未实现或注释掉的项单独列在第 11 节。

---

## 1. 一页速查：25 个模式 × 命令

| 模式 | 主要命令 |
|---|---|
| **HIZ** | 无模式专属命令；只使用全局命令 |
| **1WIRE** | `scan`、`ping`、`sniff`、`read`、`write id`、`write sp`、`temp`、`ibutton`、`eeprom`、`config` |
| **UART** | `scan`、`autobaud`、`ping`、`sniff [raw]`、`read`、`raw`、`write`、`bridge`、`at`、`emulator`、`trigger`、`spam`、`xmodem`、`swap`、`config` |
| **HDUART** | `bridge`、`config` |
| **I2C** | `scan`、`discovery`、`ping`、`identify`、`sniff [addr]`、`freq`、`slave`、`read`、`write`、`dump`、`regs`、`glitch`、`flood`、`health`、`monitor`、`trace`、`eeprom`、`recover`、`jam`、`swap`、`config` |
| **SPI** | `sniff`、`sdcard`、`slave`、`flash`、`eeprom`、`config` |
| **2WIRE** | `sniff`、`smartcard`、`config` |
| **3WIRE** | `eeprom`、`config` |
| **DIO** | `scan`、`pins`/`status`、`sniff`/`monitor`、`read`、`set`、`pullup`、`pulldown`、`pulse`、`servo`、`pwm`、`toggle`、`measure`、`jam`、`reset` |
| **LED** | `fill`、`set`、`blink`、`rainbow`、`chase`、`cycle`、`wave`、`reset`、`setprotocol`、`config` |
| **INFRARED** | `send`、`receive`、`setprotocol`、`devicebgone`、`remote`、`replay`、`record`、`load`、`jam`、`config` |
| **USB** | `storage`/`stick`、`keyboard`、`mouse`、`gamepad`、`sysctrl`、`host`、`adapters`/`adapter`、`reset`、`config` |
| **BLUETOOTH** | `scan`、`pair`、`sniff`、`spoof`、`status`、`server`、`keyboard`、`mouse`、`reset` |
| **WIFI** | `scan`、`connect`、`disconnect`、`status`、`ap`、`spam`、`spoof`、`waterfall`、`probe`、`ping`、`sniff`、`webui`、`ssh`、`telnet`、`nc`、`nmap`、`modbus`、`http`、`lookup`、`discovery`、`flood`、`repeater`/`extender`、`deauth`、`reset` |
| **ETHERNET** | `connect`、`status`、`ping`、`discovery`、`ssh`、`telnet`、`nc`、`nmap`、`modbus`、`http`、`lookup`、`reset`、`config` |
| **JTAG** | `scan swd`、`scan jtag`、`openocd`、`config` |
| **I2S** | `play`、`record`、`test`、`reset`、`config` |
| **CAN** | `sniff`、`send`、`receive`、`status`、`config` |
| **SUBGHZ** | `scan`、`sweep`、`send`、`receive`、`replay`、`jam`、`bruteforce`、`trace`、`waterfall`、`record`、`load`、`ear`、`setfrequency`、`config` |
| **RFID** | `read`、`write`、`clone`、`erase`、`config` |
| **RF24** | `scan`、`send`、`receive`、`sweep`、`jam`、`waterfall`、`setchannel`、`config` |
| **FM** | `sweep`、`trace`、`waterfall`、`broadcast`、`reset`、`config` |
| **CELL** | `modem`、`network`、`operator`、`sim`、`unlock`、`phonebook`、`sms`、`call`、`ussd`、`setmode`、`config` |
| **LORA** | `send`/`tx`、`spam`、`jam`、`receive`/`sniff`/`read`、`record`、`load`、`rssi`、`ear`、`scan`、`waterfall`、`cad`/`activity`、`airtime`/`toa`、`setfreq`/`setfrequency`、`status`、`meshtastic`/`mesh`、`config` |
| **EXPANDER** | 自动进入 UART 桥接会话（连接 ESP32-Bus-Expander） |

模式切换：`mode <name>`，简写 `m <name>`；也可以直接输入模式名（如 `i2c`、`uart`），解析器会自动转成 `mode i2c`。

---

## 2. 终端、CLI 与界面功能

### 2.1 三种终端入口

| 入口 | 说明 |
|---|---|
| **USB Serial** | 通过板载 USB CDC 串口使用 CLI；默认 115200；适合大数据量、低延迟 |
| **WiFi Web** | 浏览器通过 WebSocket 使用 CLI；支持 STA（连接现有 Wi-Fi）和 AP（设备开热点） |
| **Standalone** | 仅 Cardputer：设备屏幕作为终端、设备键盘输入；可脱离 PC 使用 |

启动时由 `TerminalTypeConfigurator` 选择；无屏板子（S3 DevKit、StampS3、UMProS3、Custom）用按键/无头方式选择。

Wi-Fi AP 模式附带：

- Captive Portal（DNS + 302 重定向）；
- 默认 AP SSID `ESP32-Bit-Pirate`，密码 `readytoboard`；
- 默认 AP IP `192.168.4.1`，通道 6，最多 4 个客户端；
- 可选 STA + AP 混合模式（连接已保存 Wi-Fi 的同时开热点）。

### 2.2 CLI 引擎

- **行编辑**：退格、左右移动光标、清行；
- **历史记录**：上下方向键、历史补全；
- **Tab 补全**：先查历史，再查内置命令词典；
- **命令纠错**：最多 1 个字符的增删改/相邻换位，且只在唯一候选时自动纠正；
- **模式简写**：直接输入 `i2c` 等价于 `mode i2c`；
- **别名**：`alias` 创建自定义快捷命令；
- **重复**：`repeat <count> <cmd>`，最多 100 次；
- **管道**：`cmd1 || cmd2 || cmd3`，最多 64 段；
- **延时**：`delay`、`delayms`、`delayus` 可在管道中插入等待；
- **Bytecode**：`[ ... ]`、`{ ... }`、`> ...` 指令块；
- **宏**：`( ... )` 语法已预留，但当前提示 `Macros Not Yet Implemented`。

### 2.3 全局命令

| 命令 | 功能 |
|---|---|
| `help` / `h` / `?` | 按模式显示帮助 |
| `mode` / `m` | 切换模式；不带参数时进入交互式模式选择 |
| `man` / `guide` | 打开固件指南（GuideShell） |
| `system` / `sys` | 系统信息面板（SysInfoShell） |
| `profile` | 保存/加载 GPIO 配置 |
| `alias` | 创建命令别名 |
| `hex` / `dec` | 十进制/十六进制/二进制转换 |
| `logic <gpio>` | 设备屏幕上的逻辑分析仪 |
| `analogic <gpio>` | 设备屏幕上的模拟绘图 |
| `wizard <gpio>` | GPIO 活动分析器 |
| `listen <gpio>` | 把 GPIO 活动映射为声音 |
| `repeat <count> <cmd>` | 重复执行命令 |
| `P` | 打开所有可用引脚的内部上拉 |
| `p` | 关闭内部上拉 |
| `delay` / `delayms` / `delayus` | 管道中的毫秒/微秒延时 |

### 2.4 设备屏幕 UI（有屏板子）

- 启动 logo / 欢迎页；
- 启动时的终端类型水平选择菜单；
- 当前模式的 **pinout 引脚映射**；
- 逻辑分析仪波形、模拟绘图、频谱瀑布图；
- Wi-Fi 状态行、DIO 拉电阻配置行；
- 屏幕旋转、亮度调节；
- 一次性 USB 适配器信息页；
- Cardputer 的终端模拟器（ANSI 解析、滚动缓冲、双缓冲渲染）；
- S3 DevKit 板载 RGB LED 用颜色指示当前模式（HIZ 关闭，I2C 蓝色，SPI 绿色，UART/HDUART 黄色，1WIRE 青色，其他白色）。

### 2.5 Web UI（浏览器端）

访问 `http://<设备IP>/`：

- WebSocket 终端：命令输入、输出显示、断线重连、单会话；
- 命令历史、自动补全、特殊按键（ESC / Ctrl-C / Tab / Ctrl-D / Ctrl-Z / Ctrl-X）；
- `bridge` / `keyboard` 桥接模式：原始双向透传；
- **文件管理器**：LittleFS 列表、上传（支持拖拽）、下载、删除、剩余空间显示；
- **Pirate Assistant**：
  - 用自然语言描述目标（例如“怎么读 AT24C EEPROM？”）；
  - 调用 DeepSeek API 生成命令建议；
  - API Key 保存在浏览器本地（localStorage），请求发送到 DeepSeek；
  - 可一键把建议命令发送到终端；
- 移动端自适应布局；
- WebSocket 断线弹窗提示（“One terminal session only”）。

> 注意：Web UI 的 DeepSeek 功能是**浏览器调用外部 API**，不是固件内置离线模型；使用前需要自己的 API Key。

### 2.6 项目附带的 Web 工具（不在固件内）

- Web Flasher：浏览器直接烧录固件；
- Web Serial Terminal：浏览器串口终端；
- Python Lab：浏览器里编写/运行 Python 串口脚本；
- Web Tools：SDR、红外等在线工具入口；
- M5Burner 固件源。

---

## 3. 模式功能详解

### 3.1 HIZ（高阻态，默认模式）

- 所有协议引脚保持高阻/安全状态；
- 只能使用全局命令；
- 用作“没有连接任何协议”的默认状态。

### 3.2 1WIRE

| 命令 | 功能 |
|---|---|
| `scan` | 搜索 1-Wire 设备，显示 ROM ID |
| `ping` | 检测设备是否存在 |
| `sniff` | 查看 1-Wire 总线流量 |
| `read` | 读取 ID + scratchpad |
| `write id [8 bytes]` | 写设备 ID |
| `write sp [8 bytes]` | 写 scratchpad |
| `temp` | 读取温度传感器 |
| `ibutton` | iButton Shell |
| `eeprom` | 1-Wire EEPROM Shell |
| `config` | 配置引脚/参数 |
| `[0xAA r:8]` | Bytecode 指令 |

**iButton Shell（RW1990）**：读取 ID、写入 ID、复制 ID。  
**1-Wire EEPROM Shell**：Probe、Analyze、Read、Write、Dump、Erase；默认 DS2431。

### 3.3 UART

| 命令 | 功能 |
|---|---|
| `scan` | 扫描 RX 引脚上的 UART 活动 |
| `autobaud` | 通过 RX 边沿自动检测波特率，可保存 |
| `ping` | 发送并等待回复 |
| `sniff` / `sniff raw` | 文本/十六进制监听 UART |
| `snifftxt` / `sniffraw` | 同上，显式别名 |
| `read` / `raw` | 接收 ASCII / 原始十六进制 |
| `readraw` / `readhex` | 原始读取别名 |
| `write [text]` | 发送文本，或 `hex{ AA BB }` |
| `bridge` | 全双工桥：终端 ↔ UART |
| `at` | AT 命令 Shell |
| `emulator` | UART 设备模拟 Shell |
| `trigger [pattern]` | 检测到模式后自动发送响应；支持文本/hex/通配符 |
| `spam [text] [ms]` | 每 N 毫秒重复发送文本 |
| `xmodem send <path>` | 通过 XMODEM 发送 SD 卡文件 |
| `xmodem recv <path>` | 通过 XMODEM 接收并写入 SD 卡 |
| `swap` | 交换 RX/TX |
| `config` | 配置引脚、波特率、数据位、校验、停止位、反相、流控 |
| `[ ... ]` | Bytecode 指令 |

**AT Shell** 内置 19 类命令模板：

```text
Config / Info / SIM & PIN / Telephony Network / Calls / SMS / Phonebook /
USSD / Data (PDP) / Time & Clock / Internet TCP/IP / HTTP(S) / GNSS/GPS /
LoRa / LoRaWAN / Audio / Power / PSM / Hardware I/O / MQTT / File Storage
```

每条模板支持参数校验、默认值、示例、危险命令二次确认。

### 3.4 HDUART（半双工 UART）

| 命令 | 功能 |
|---|---|
| `bridge` | 单线半双工双向会话 |
| `config` | 配置引脚、波特率、数据位、校验、停止位、反相 |
| `[0x1 D:10 r:255]` | Bytecode 指令 |

### 3.5 I2C

| 命令 | 功能 |
|---|---|
| `scan` | 扫描 1–126 地址，列出 ACK 设备 |
| `discovery` | 扫描并给出设备类型/地址识别报告 |
| `ping <addr>` | 检查单个地址 ACK |
| `identify <addr>` | 根据已知地址库识别设备 |
| `sniff [addr]` | 被动嗅探 I2C，支持地址过滤 |
| `freq` | 测量 SCL 频率、周期、HIGH/LOW、可靠性 |
| `slave <addr>` | 模拟 I2C 从机并记录主机写入 |
| `read <addr> [reg]` | 读寄存器 |
| `write <addr> [reg] [val]` | 写寄存器 |
| `dump <addr> [len]` | 连续读取寄存器 |
| `regs <addr> [len]` | 探测寄存器可读/可写性 |
| `glitch <addr>` | 执行 8 步 I2C 故障注入序列 |
| `flood <addr>` | 随机寄存器地址高频访问 |
| `health <addr>` | 时序/稳定性测试 |
| `monitor <addr> [ms]` | 轮询并显示寄存器变化 |
| `trace <addr> [reg] [ms]` | 跟踪单个寄存器 |
| `eeprom [addr]` | I2C EEPROM Shell |
| `recover` | 总线卡死恢复（bit-bang 时钟脉冲） |
| `jam` | 向 SCL/SDA 注入随机噪声 |
| `swap` | 交换 SDA/SCL |
| `config` | 配置引脚/频率 |
| `[ ... ]` | Bytecode 指令 |

**I2C EEPROM Shell**：Probe、Analyze、Read、Write、Dump ASCII、Dump RAW、Erase；支持 24x01 到 24xM02 共 14 种型号。

**I2C glitch 序列**：随机 junk flood、START flood、over-read、非法寄存器读、时钟拉伸混淆、快速 START/STOP、ACK 注入、SCL/SDA 随机噪声。

### 3.6 SPI

| 命令 | 功能 |
|---|---|
| `sniff` | 被动查看 SPI 流量 |
| `sdcard` | SD 卡 Shell（类似文件系统） |
| `slave` | 模拟 SPI 从机 |
| `flash` | SPI Flash Shell |
| `eeprom` | SPI EEPROM Shell |
| `config` | 配置 CS/CLK/MISO/MOSI/频率/WP/HOLD |
| `[0x9F r:3]` | Bytecode 指令 |

**SD Shell**：`ls`、`cd`、`mkdir`、`touch`、`rm`、`cat`、`echo`、`help`、`exit`，带文件类型图标和缓存。  
**SPI Flash Shell**：Probe、Analyze、Search string、Extract strings、Read、Write、Dump ASCII、Dump RAW、Erase。  
**SPI EEPROM Shell**：Probe、Analyze、Read、Write、Dump ASCII、Dump RAW、Erase；支持 25X010 到 25XM04 共 14 种型号。

### 3.7 2WIRE

| 命令 | 功能 |
|---|---|
| `sniff` | 监听 2-Wire CLK/IO 流量 |
| `smartcard` | SLE44XX 智能卡 Shell |
| `config` | 配置 CLK/IO/RST |
| `[ ... ]` | 指令入口存在，但当前未实现 |

**SmartCard Shell**：Probe（ATR）、Security check、Unlock card、PSC Set/Get、Write、Dump、Protect。

### 3.8 3WIRE

| 命令 | 功能 |
|---|---|
| `eeprom` | 3-Wire EEPROM Shell |
| `config` | 配置 CS/SK/DI/DO |

**3-Wire EEPROM Shell**：选择型号、选择 8/16 位组织方式，然后 Probe、Read、Write、Dump、Erase。支持 93xx 系列常见型号。

### 3.9 DIO（数字 I/O）

| 命令 | 功能 |
|---|---|
| `scan` | 检测引脚活动 |
| `pins` / `status` | 显示所有可用引脚状态 |
| `sniff <gpio>` / `monitor <gpio>` | 跟踪引脚翻转 |
| `read <gpio>` | 读取电平 |
| `set <gpio> <H/L/I/O>` | 设为高/低/输入/输出 |
| `pullup <gpio>` | 内部上拉 |
| `pulldown <gpio>` | 内部下拉 |
| `pulse <gpio> <us>` | 输出指定宽度脉冲 |
| `servo <gpio> <angle>` | 舵机角度 |
| `pwm <gpio> [freq duty%]` | PWM 输出 |
| `toggle <gpio> <ms>` | 周期翻转 |
| `measure <gpio> [ms]` | 测量频率/占空比 |
| `jam <gpio> [min max]` | 随机高低电平 |
| `reset <gpio>` | 恢复默认 |

### 3.10 LED

| 命令 | 功能 |
|---|---|
| `fill <color>` | 所有 LED 填充颜色 |
| `set <index> <color>` | 设置单个 LED |
| `blink` | 闪烁 |
| `rainbow` | 彩虹 |
| `chase` | 追逐灯效 |
| `cycle` | 颜色循环 |
| `wave` | 波浪 |
| `reset` | 全部关闭 |
| `setprotocol` | 选择 LED 协议 |
| `config` | 配置数据/时钟引脚、数量、亮度 |

支持 FastLED 的多类可寻址 LED：WS2812/WS2812B/WS2811/WS2813/WS2815/WS2816、SK6812/SK6822、APA104/APA106、TM1803/1804/1809/1812/1829、UCS1903/1904/2903/1912、LPD6803/8806/1886、P9813、SM16703/16716 等。

### 3.11 INFRARED（红外）

| 命令 | 功能 |
|---|---|
| `send <dev> sub <cmd>` | 按协议/设备/子设备/功能码发送 |
| `receive` | 接收并解码红外信号 |
| `setprotocol` | 选择红外协议 |
| `devicebgone` | 遍历发送所有电视/设备的 ON/OFF 码 |
| `remote` | 通用遥控 Shell |
| `replay [count]` | 录制一段红外帧并回放，可无限/指定次数 |
| `record` | 录制解码命令到 LittleFS `.ir` 文件 |
| `load` | 从 LittleFS 加载 `.ir` 文件并发送 |
| `jam` | 发送随机红外信号 |
| `config` | 配置 TX/RX 引脚、协议 |

**Universal Remote Shell**：ON/OFF、MUTE、PLAY、PAUSE、VOL+、VOL-、CH+、CH-，每组遍历多种品牌协议。  
红外协议覆盖 80+ 种（README 口径），具体列表在 `InfraredProtocolDefinitions.h`。

### 3.12 USB

| 命令 | 功能 |
|---|---|
| `storage` / `stick` | 把 SD 卡挂载成 USB MSC 存储 |
| `keyboard [text]` | 发送字符串；`keyboard` 进入 HID 键盘桥 |
| `mouse [action]` | 鼠标移动/点击；`mouse jiggle` 随机移动 |
| `gamepad [key]` | 模拟 USB 游戏手柄按键 |
| `sysctrl [action]` | USB 系统控制：睡眠、唤醒、关机 |
| `host` | 切换为 USB Host，连接键盘/鼠标/游戏手柄等并打印描述符 |
| `adapters` / `adapter` | 进入一次性 USB 适配器 Shell |
| `reset` | 重置 USB 接口 |
| `config` | 配置 VID/PID/厂商/产品字符串 |

**USB Host** 限制：切换到 Host 后 CDC/HID/MSC 不可用，通常需要 WiFi Web 终端操作；当前停止 Host 后不能完全恢复，需要重启（源码 TODO）。

### 3.13 BLUETOOTH

| 命令 | 功能 |
|---|---|
| `scan` | 扫描 BLE 设备 10 秒 |
| `pair <mac>` | 连接设备并枚举服务 |
| `sniff` | 嗅探蓝牙数据 |
| `spoof <mac>` | 伪造 MAC |
| `status` | 显示模式、连接状态、MAC |
| `server [name]` | 启动 BLE HID 服务端 |
| `keyboard [text]` | BLE 键盘发送/桥接 |
| `mouse [action]` | BLE 鼠标移动/点击/抖动 |
| `reset` | 重置蓝牙接口 |

**Mouse Shell**：上/下/左/右移动、左键、右键、配置步长、jiggle。

### 3.14 WIFI

| 命令 | 功能 |
|---|---|
| `scan` | 扫描 Wi-Fi 网络 |
| `connect` | 连接网络（交互式/已保存凭据） |
| `disconnect` | 断开 |
| `status` | 显示模式、连接、IP、SSID |
| `ap <ssid> <password>` | 开热点；可 AP+STA |
| `spam` | 发送随机 Beacon |
| `spoof sta/ap <mac>` | 伪造 Station/AP MAC |
| `waterfall` | 设备屏幕显示信道活动 |
| `probe` | 尝试连接周围开放网络，寻找互联网出口 |
| `ping <host>` | ICMP ping |
| `sniff` | 被动抓包，自动跳信道 1–13 |
| `webui` | 显示 Web UI 地址 |
| `ssh [host] [user] [pw] [port]` | SSH 会话 |
| `telnet <host> [port]` | Telnet 会话 |
| `nc <host> <port>` | Netcat 会话 |
| `nmap <host> [-p ports]` | 端口扫描 |
| `modbus <host> [port]` | Modbus TCP Shell |
| `http get <url>` | HTTP(S) GET |
| `http analyze <url>` | HTTP 分析报告 |
| `lookup mac/ip <addr>` | MAC/IP 查询 |
| `discovery [timeout]` | 局域网设备发现 |
| `flood [channel]` | 信道包洪泛 |
| `repeater` / `extender` | Wi-Fi 中继/扩展 |
| `deauth [ssid]` | 取消认证攻击 |
| `reset` | 重置 Wi-Fi |

> 这些无线测试/攻击能力仅应在**你拥有或获得明确授权**的网络和设备上使用。

### 3.15 ETHERNET（W5500）

| 命令 | 功能 |
|---|---|
| `connect` | DHCP 连接 |
| `status` | W5500/网络状态 |
| `ping <host>` | ICMP |
| `discovery [timeout]` | 局域网发现 |
| `ssh` / `telnet` / `nc` / `nmap` / `modbus` / `http` / `lookup` | 与 WiFi 模式共享的网络工具箱 |
| `reset` | 重置接口 |
| `config` | 配置 CS/RST/SCK/MISO/MOSI/IRQ/MAC |

### 3.16 JTAG

| 命令 | 功能 |
|---|---|
| `scan swd` | 扫描 SWDIO/SWCLK 并读取 IDCODE |
| `scan jtag` | 扫描 TDI/TDO/TCK/TMS/TRST |
| `openocd` | 重启进入 OpenOCD Bus Pirate 适配器 |
| `config` | 配置扫描引脚组 |

### 3.17 I2S

| 命令 | 功能 |
|---|---|
| `play <freq> [ms]` | 播放正弦波 |
| `record` | 持续读取麦克风 |
| `test speaker` / `test mic` | 扬声器/麦克风基本测试 |
| `reset` | 恢复默认 |
| `config` | 配置 BCLK/LRCK/DATA/采样率/位深/音量 |

内置 PCM 测试音数据（`PcmSoundTestComplete.h`）。

### 3.18 CAN（MCP2515）

| 命令 | 功能 |
|---|---|
| `sniff` | 打印所有接收帧 |
| `send [id]` | 发送指定 ID 的帧 |
| `receive [id]` | 按 ID 捕获帧 |
| `status` | CAN 控制器状态 |
| `config` | 配置 CS/SCK/SI/SO 和速率 |

### 3.19 SUBGHZ（CC1101）

| 命令 | 功能 |
|---|---|
| `scan` | 按频段扫描，寻找活跃频率 |
| `sweep` | 频段活动分析 |
| `setfrequency` / `setfreq` | 设置工作频率 |
| `send <payload> [te]` | 发送帧 |
| `receive` | 接收原始/解码信号 |
| `decode` / `raw` | 旧版别名 |
| `replay` | 录制并回放原始信号 |
| `jam` | 单频/多频干扰 |
| `bruteforce` | 12 位密钥暴力尝试 |
| `trace` | 屏幕显示 RX 信号轨迹 |
| `waterfall` | 屏幕显示频率峰值 |
| `record` | 记录到 `.sub` 文件 |
| `load` | 加载 `.sub` 文件 |
| `ear` | RSSI 映射为声音 |
| `config` | 配置 SCK/MISO/MOSI/CS/GDO/频率/功率/调制/带宽 |

支持 RAW、BinRAW、RcSwitch、Princeton 等协议解析。

### 3.20 RFID（PN532）

| 命令 | 功能 |
|---|---|
| `read` | 读取标签 UID/ATQA/SAK/类型 |
| `write` | 写 UID（magic card）或 Block/Page 数据 |
| `clone` | 克隆 Mifare UID |
| `erase` | 擦除标签 |
| `config` | 配置 I2C SDA/SCL |

### 3.21 RF24（NRF24L01）

| 命令 | 功能 |
|---|---|
| `scan` | 扫描 0–125 信道，寻找最佳活动信道 |
| `send` | 配置信道/地址/CRC/速率/动态载荷后发送 |
| `receive` | 按信道/地址/管道/CRC/速率接收 |
| `sweep` | 信道活动分析 |
| `jam` | 按频段组/全信道/单信道干扰 |
| `waterfall` | 屏幕显示信道峰值 |
| `setchannel` | 设置工作信道 |
| `config` | 配置 CSN/CE/SCK/MISO/MOSI |

### 3.22 FM（Si4713）

| 命令 | 功能 |
|---|---|
| `sweep` | 频率扫描 |
| `trace [freq]` | 观察频率信号 |
| `waterfall` | 频率峰值瀑布 |
| `broadcast` | FM 广播 Shell |
| `reset` | 通过 RST 引脚复位 |
| `config` | 配置 I2C 引脚/参数 |

**FM Broadcast Shell**：查看电台状态、开始/停止发射、设置频率、自动找频、设置功率、设置 RDS PS 名称、设置 RDS 文本、切换 TA。

### 3.23 CELL（SIMCom 模块）

| 命令 | 功能 |
|---|---|
| `modem` | 模块信息 |
| `network` | 网络注册信息 |
| `operator` | 可用运营商 |
| `sim` | SIM 卡信息 |
| `unlock` | 用 PIN 解锁 SIM |
| `phonebook` | SIM 电话簿 |
| `sms` | SMS Shell |
| `call` | 通话 Shell |
| `ussd [code]` | 发送 USSD，可选 DCS 编码 |
| `setmode` | CFUN=1 正常 / CFUN=4 飞行模式 |
| `config` | 配置 UART 引脚/波特率 |

**SMS Shell**：列出（可按未读/已读/已发送过滤）、读取、删除（单条/全部已读/全部）、发送。  
**Call Shell**：拨号、接听、挂断、列出当前通话（CLCC）。

### 3.24 LORA（SX1262）

| 命令 | 功能 |
|---|---|
| `send` / `tx [payload]` | 发送文本或 `hex{ AA BB }` |
| `spam [payload] [ms]` | 周期性发送 |
| `jam [seconds]` | 实验室 CW 干扰 1–60 秒 |
| `receive` / `sniff` / `read` | 监听数据包 |
| `record` | 接收并保存数据包 |
| `load` | 发送保存的 `.lora` 数据包 |
| `rssi [ms]` | 监视信道 RSSI |
| `ear` | RSSI 映射为声音 |
| `scan` | 扫描最佳频率 |
| `waterfall` | 屏幕显示频率活动 |
| `cad [ms]` | 信道活动检测 |
| `airtime [bytes]` | 计算 1–255 字节的空中时间 |
| `setfreq [MHz]` / `setfrequency` | 设置频率 |
| `status` | 显示配置文件和计数器 |
| `meshtastic` / `mesh` | Meshtastic Shell |
| `config` | 配置 SCK/MISO/MOSI/CS/RST/BUSY/DIO1 和 LoRa 参数 |

**Meshtastic Shell**：状态、选择预设、设置频率、设置/清除信道密钥、发送文本、接收数据包、解析 hex 帧。

### 3.25 EXPANDER

- 通过 UART 连接外部 [ESP32-Bus-Expander](https://github.com/geo-tp/ESP32-Bus-Expander)；
- 自动握手（`[[BP-HANDSHAKE-OK]]`）；
- 握手成功后把所有命令透传给扩展板；
- 输入 `exit` 返回 Bit Pirate。

---

## 4. Shell 子功能总表

| Shell | 功能 |
|---|---|
| `HelpShell` | 按模式列出全部命令 |
| `GuideShell` | Quick start、命令示例、Web 终端、指令语法、Python 自动化、文件系统、Pirate Assistant、USB 适配器、安全提示 |
| `SysInfoShell` | 系统摘要、硬件、内存、屏幕、分区、LittleFS、NVS、网络、调试日志、重启、进入 Bootloader |
| `ProfileShell` | 保存/加载 `.profile` GPIO 配置 |
| `UsbAdapterShell` | 9 种一次性 USB 适配器 |
| `I2cEepromShell` | I2C EEPROM 探测/读/写/dump/擦除（14 型号） |
| `SpiFlashShell` | SPI Flash 探测/分析/搜索/字符串/读/写/dump/擦除 |
| `SpiEepromShell` | SPI EEPROM 探测/读/写/dump/擦除（14 型号） |
| `SdCardShell` | SD 卡文件系统 Shell |
| `SmartCardShell` | SLE44XX 智能卡探测/安全/解锁/PSC/写/dump/保护 |
| `OneWireEepromShell` | 1-Wire EEPROM 探测/分析/读/写/dump/擦除 |
| `ThreeWireEepromShell` | 3-Wire EEPROM 探测/读/写/dump/擦除 |
| `IbuttonShell` | RW1990 iButton 读/写/复制 ID |
| `UartAtShell` | 19 类 AT 命令模板 |
| `UartEmulationShell` | UART 设备模拟 |
| `UniversalRemoteShell` | 通用红外遥控（ON/OFF/MUTE/PLAY/PAUSE/VOL/CH） |
| `ModbusShell` | FC01/02/03/04/05/06/0F/16 + 轮询监控 + Unit ID/目标切换 |
| `MeshtasticShell` | Meshtastic 状态/预设/频率/密钥/发送/接收/解析 |
| `CellSmsShell` | SMS 列表/读取/删除/发送 |
| `CellCallShell` | 拨号/接听/挂断/通话列表 |
| `FmBroadcastShell` | FM 电台状态/发射/频率/功率/RDS/TA |
| `MouseShell` | 鼠标移动/左右键/步长/jiggle |
| `AliasManager` | 命令别名管理 |

---

## 5. 嗅探、分析与绘图

| 功能 | 说明 |
|---|---|
| **I2C 嗅探** | RMT 双通道 SCL/SDA，DMA，ISR→环形缓冲，任务解码，地址过滤 |
| **I2C 频率测量** | SCL 频率、周期、HIGH/LOW、样本可靠性 |
| **UART 嗅探** | 双硬件串口，文本/原始两种模式 |
| **SPI 嗅探** | 被动查看 SPI 流量 |
| **1-Wire 嗅探** | 查看 1-Wire 总线流量 |
| **2-Wire 嗅探** | CLK/IO 流量、START/STOP/数据事件 |
| **CAN 嗅探** | 打印所有 CAN 帧 |
| **Wi-Fi 嗅探** | 被动抓包，自动跳信道 |
| **Wi-Fi 瀑布图** | 设备屏幕信道活动 |
| **蓝牙嗅探** | BLE 数据嗅探 |
| **Sub-GHz 分析** | 频率扫描、频段 sweep、trace、waterfall、RSSI→audio |
| **RF24 分析** | 信道 scan、sweep、waterfall、jam |
| **LoRa 分析** | RSSI、CAD、waterfall、airtime、scan |
| **FM 分析** | sweep、trace、waterfall |
| **逻辑分析仪** | `logic <gpio>`，屏幕波形 |
| **模拟绘图** | `analogic <gpio>`，屏幕曲线 |
| **GPIO 活动分析** | `wizard <gpio>` 周期性报告 |
| **GPIO 声音化** | `listen <gpio>` 把活动变成声音 |
| **二进制分析** | dump、搜索、字符串提取（SPI Flash Shell 等） |
| **红外录制/回放** | `.ir` 文件、实时 replay |
| **Sub-GHz 录制/回放** | `.sub` 文件、replay |
| **LoRa 录制/发送** | `.lora` 文件、record/load |

---

## 6. 一次性 USB 适配器（9 种）

| # | 适配器 | 主机侧工具 | 功能 |
|---|---|---|---|
| 1 | USB-UART Bridge | picocom、PuTTY、Web Serial | CDC ↔ `Serial1` 全双工桥；CDC line coding 映射波特率/数据位/校验/停止位 |
| 2 | Flashrom Serprog | flashrom | SPI Flash 编程器 |
| 3 | AVRDUDE Bus Pirate | avrdude | AVR ISP（Bus Pirate 二进制 SPI 协议） |
| 4 | SUMP Logic Analyzer | PulseView / sigrok | 最多 8 通道逻辑分析仪 |
| 5 | OpenOCD Bus Pirate | OpenOCD | JTAG/SWD 调试 |
| 6 | USB IR Toy | LIRC `mode2` | 红外接收/发射 |
| 7 | SubGHz Raw CDC | 串口工具/脚本 | CC1101 原始 OOK，ASCII 命令 |
| 8 | BPIO2 | Bus Pirate 客户端 | FlatBuffers + COBS，SPI/I2C/GPIO；兼容 BPIO2 v2 |
| 9 | SDR CDC | ESP-SDR 工具 | 2.2–2.8GHz 原始 RF/IQ，实验性 |

进入方式：`usb` 模式 → `adapters` → 选择适配器 → 配置引脚 → 设备重启进入专用模式；再次复位/按键返回普通固件。

---

## 7. 网络工具箱（WiFi / Ethernet 共享）

| 功能 | 命令 | 说明 |
|---|---|---|
| ICMP ping | `ping <host>` | 次数/超时/间隔可配 |
| 局域网发现 | `discovery [timeout]` | ICMP 扫描 + 结果 |
| SSH | `ssh [host] [user] [pw] [port]` | LibSSH-ESP32 |
| Telnet | `telnet <host> [port]` | 原始 Telnet |
| Netcat | `nc <host> <port>` | TCP/UDP 调试 |
| Nmap | `nmap <host> [-p ports]` | 端口扫描 |
| Modbus TCP | `modbus <host> [port]` | FC01/02/03/04/05/06/0F/16 + 监控 |
| HTTP | `http get <url>`、`http analyze <url>` | GET 与分析报告 |
| MAC/IP 查询 | `lookup mac/ip <addr>` | 地址查询 |
| Wi-Fi 扫描 | `scan` | SSID/RSSI/加密等 |
| Wi-Fi 连接/热点 | `connect`、`ap`、`disconnect` | STA/AP/AP+STA |
| Wi-Fi 抓包 | `sniff` | 自动跳信道 |
| Wi-Fi 瀑布/探测 | `waterfall`、`probe` | 信道活动、开放网络探测 |
| Wi-Fi 攻击测试 | `deauth`、`spam`、`flood`、`spoof` | 仅限授权测试 |
| Wi-Fi 中继 | `repeater` / `extender` | 转发流量 |
| Web UI 地址 | `webui` | 显示当前访问地址 |

---

## 8. 存储、配置与系统管理

### 8.1 NVS

- Wi-Fi SSID/密码；
- 一次性 USB 适配器模式；
- 各适配器的引脚/频率等一次性配置；
- 其他少量跨重启键值。

### 8.2 LittleFS

- `.profile`：完整 GPIO/协议配置；
- `.ir`：红外录制文件；
- `.sub`：Sub-GHz 录制文件；
- `.lora`：LoRa 数据包；
- 其他脚本/数据文件；
- Web 文件管理器可列出、上传、下载、删除；
- `ProfileShell` 保存/加载；
- `SysInfoShell` 查看空间/NVS/LittleFS。

### 8.3 SD 卡

- `spi` 模式 → `sdcard` Shell；
- `uart` 模式 → XMODEM 收发文件；
- `usb` 模式 → `storage` 把 SD 卡挂载为 USB 存储；
- 文件缓存/数量限制由 `GlobalState` 控制。

### 8.4 系统信息

`SysInfoShell` 提供：

- 系统摘要；
- 芯片/硬件信息；
- Heap/PSRAM/Stack；
- 分区表；
- 屏幕信息；
- LittleFS；
- NVS 统计/条目；
- 网络状态；
- 调试日志开关；
- 软/硬重启；
- 进入 USB Bootloader。

### 8.5 配置管理

- `config`：每个模式交互式配置引脚/速率/参数；
- `swap`：I2C/UART 交换引脚；
- `profile`：保存/加载整套配置；
- `P` / `p`：全局上拉开关；
- `wizard`：引脚活动诊断；
- `system`：系统信息与重启。

---

## 9. 脚本与自动化

- **Bus Pirate bytecode**：`[ ... ]`、`{ ... }`、`> ...`，支持 `Write`/`Read`/`Start`/`Stop`/`DelayUs`/`DelayMs`/`SetClkHigh`/`AuxHigh`/`Adc` 等；
- **字节/字符串/hex/重复**：`0xAA`、`123`、`'A'`、`"Hello"`、`r:8`、`rrrr`；
- **命令管道**：`cmd1 || cmd2 || cmd3`；
- **重复**：`repeat 10 cmd`；
- **别名**：`alias`；
- **Python 自动化**：通过串口发送 CLI 命令（外部 Python 脚本，不是固件内置解释器）；
- **Web Serial / Python Lab**：浏览器端串口工具；
- **XMODEM**：UART 文件收发；
- **文件驱动**：`.profile`、`.ir`、`.sub`、`.lora`。

Bytecode 实际支持情况：

| 模式 | Bytecode |
|---|---|
| 1WIRE | ✅ |
| UART | ✅ |
| HDUART | ✅ |
| I2C | ✅ |
| SPI | ✅ |
| 2WIRE | ❌ 提示 TODO |
| 3WIRE | ❌ 提示未实现 |
| LED | ❌ 提示未实现 |

---

## 10. 支持的硬件与生态

### 10.1 官方列出的板子

- ESP32-S3 DevKit（N8 / N8R2 / N8R8 / N16R8 等）
- ESP32-S3 Zero / SuperMini
- M5 AtomS3 Lite
- M5 Cardputer
- M5 Cardputer ADV
- M5 StampS3
- M5 Stick S3
- LILYGO T-Display S3
- LILYGO T-Embed
- LILYGO T-Embed CC1101
- LILYGO T-Embed CC1101 Plus
- Heltec WiFi LoRa 32 V4（仓库也有 V3 env）
- Heltec Vision Master T190
- Seeed Studio Xiao ESP32-S3
- Waveshare ESP32-S3-GEEK
- ESP32-S3 DevKitC-1 N16R8 + ST7789 + EC11
- Unexpected Maker ProS3
- 其他 ESP32-S3 板子：≥4MB Flash，可用 `custom` 或通用固件，但引脚可能需自行调整

### 10.2 生态项目

- **ESP32-Bus-Expander**：通过 UART 扩展 Wi-Fi 5GHz/Zigbee 等；
- **ESP32-Bit-Pirate-Dock**：兼容原版 Bus Pirate 转接板；
- **Web Flasher**：浏览器烧录；
- **Web Serial Tools**：浏览器串口终端；
- **Python Lab**：浏览器 Python 脚本；
- **Python 脚本仓库**：串口自动化示例；
- **M5Burner**：M5 设备固件分发。

---

## 11. 未实现 / 有限制的功能

| 项 | 状态 |
|---|---|
| 宏 `( ... )` | 语法识别，但提示 `Macros Not Yet Implemented` |
| UART `glitch` | 提示 `Uart Glicher: Not Yet Implemented` |
| 2WIRE bytecode | 提示 TODO |
| 3WIRE bytecode | 提示未实现 |
| LED bytecode | 提示未实现 |
| LED `scan` | 代码存在但控制器中被注释（RMT 通道限制） |
| USB Host 退出 | 停止 Host 后不能完全恢复，需要重启（源码 TODO） |
| Sub-GHz `bruteforce` | 部分协议提示未实现 |
| WebSocket | 单客户端；新连接会关闭旧连接；HTTP/WS 无鉴权 |
| Web UI DeepSeek | 需要自己的 API Key；请求发往 DeepSeek；Key 仅存浏览器本地 |
| 无线攻击类命令 | `deauth`、`spam`、`flood`、`jam` 等仅限授权测试，可能违反当地法规 |

---

## 12. 规模统计（便于评估功能面）

| 项 | 数量 |
|---|---|
| 工作模式 | 25（含 HIZ） |
| Controller | 25 |
| Service | 40 |
| Shell | 22 |
| Transformer | 12 |
| Analyzer | 4 |
| USB 适配器 | 9 |
| 固件构建环境 | 20 + 1 原生测试 |
| 源文件 | 约 432 个 |
| 源码行数 | 约 83k 行 |
| 原生测试文件 | 57 个，约 12.5k 行 |

---

## 附：主要来源

- `src/Enums/ModeEnum.h`：模式列表
- `src/Shells/HelpShell.cpp`：各模式命令与说明
- `src/Controllers/*.cpp`：实际命令分派与参数
- `src/Shells/*`：子 Shell 功能
- `src/Adapters/*`、`src/Configurators/BootModeConfigurator.cpp`：USB 适配器
- `src/Servers/*`、`webui/*`：Web UI 与 WebSocket
- `src/Services/*`：协议/硬件能力
- `README.md`：板卡与生态说明
- `docs/ARCHITECTURE.zh-CN.md`：架构与流程图
