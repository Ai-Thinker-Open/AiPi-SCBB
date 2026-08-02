<p align="center">
  <img src="docs/logo.png" alt="AiPi-SCBB" width="70%">
</p>

[![English](https://img.shields.io/badge/English-README-blue)](README.md)

Ai-Thinker 通用外设驱动库，为 I2C、SPI、PWM+DMA、UART、GPIO、单总线等协议的外部器件提供统一的硬件抽象层。

平台无关设计 — 库通过可配置的 BSP（板级支持包）宏实现硬件抽象，你可以为任何 MCU 平台提供自己的 BSP 实现。

内置 BSP：`BSP/Ai-M6x`（BL616/BL618，博流 Bouffalo SDK）和 `BSP/stm32f10x`（STM32 HAL）。

## 项目简介

AiPi-SCBB 是一个平台无关的外设驱动库，为外部器件提供统一的硬件抽象层。本仓库按模块组织：每个模块目录包含独立的驱动（`axk_*.c` / `axk_*.h`），所有硬件操作都通过 `scbb_config.h` 中配置的 BSP 宏完成。支持的器件列表见下方模块表。

## 支持的模块

| 模块 | 说明 | 协议 | 地址 |
|------|------|------|------|
| CH224A | USB-PD sink 控制器（电压协商：5-28V, PPS, AVS） | I2C | 0x22 |
| SHT3x | 温湿度传感器 | I2C | 0x44 |
| WS2812 | RGB LED 灯条驱动 + HSV/RGB 颜色工具 | PWM+DMA | — |
| HXD039B2 | 红外编解码器（空调遥控器） | UART+GPIO | — |
| ST7789V_LCD | ST7789V 驱动芯片 LCD 模块（1.47"/1.69"/1.9"/2.0"） | SPI+GPIO | — |
| RELAY | 继电器驱动（高电平有效） | GPIO | — |
| DHT11 | 温湿度传感器（单总线） | GPIO | — |
| RD03_V2 | 毫米波雷达（人体存在/距离检测） | UART | — |
| INA226 | 电压/电流/功率监测 | I2C | 0x40 |
| DS1302 | RTC 实时时钟（3 线位操作） | GPIO | — |
| OLED_096_SPI | 0.96" OLED 显示屏（SSD1306, 128x64） | SPI+GPIO | — |

## 环境准备

### Python 依赖（menuconfig 需要）

```bash
pip install kconfiglib windows-curses
```

### 编译工具

- CMake 3.15+
- GCC（或目标平台的任意 C 编译器）

## 快速开始

典型的项目使用流程：

```bash
pip install kconfiglib windows-curses   # 步骤 1：安装配置工具依赖
python menuconfig.py                     # 步骤 2：启用模块并生成 scbb_config.h
cmake -B build                           # 步骤 3：配置构建
cmake --build build                      # 步骤 4：编译
```

编译完成后，将生成的 `aipi_scbb` 库链接到你的工程（见[集成到项目](#集成到项目)）。更详细的使用说明见[配置模块](#配置模块)。

## 配置模块

### 方法 1：menuconfig.py（推荐）

运行图形化配置工具：

```bash
python menuconfig.py
```

<p align="center">
  <img src="docs/img/config_mod.jpg" alt="menuconfig 主界面" width="70%">
</p>

**操作步骤：**

1. 运行命令后进入 TUI 配置界面
2. 用 `↑` `↓` 方向键选择模块
3. 按 `Y` 启用模块（显示为 `[*]`）
4. 展开模块可配置 BSP 头文件和函数前缀（如 `my_platform_i2c.h`、`my_i2c`）

<p align="center">
  <img src="docs/img/enable_mod.png" alt="模块启用状态" width="70%">
</p>

5. 进入 `Output` 菜单设置 `scbb_config.h` 输出路径（默认：项目根目录的 `scbb_config.h`）

<p align="center">
  <img src="docs/img/scbb_config_path.jpg" alt="输出路径配置" width="70%">
</p>

6. 按 `S` 保存，再按 `Q` 退出

保存后会自动生成 `scbb_config.h`，CMake 会自动读取该文件确定启用的模块，无需手动传递 `-D` 参数。

### 方法 2：手动编辑配置文件

1. 在项目根目录创建 `scbb_config.h`（或先运行 `python menuconfig.py` 生成）
2. 取消注释或添加需要启用的模块：

```c
#define SCBB_CH224A_ENABLED 1   // 启用 CH224A
#define SCBB_SHT3X_ENABLED 1    // 启用 SHT3x
// #define SCBB_WS2812_ENABLED 1  // 取消注释以启用 WS2812
// #define SCBB_HXD039B2_ENABLED 1 // 取消注释以启用 HXD039B2
// #define SCBB_ST7789V_LCD_ENABLED 1 // 取消注释以启用 ST7789V_LCD
// #define SCBB_RELAY_ENABLED 1       // 取消注释以启用 RELAY
// #define SCBB_DHT11_ENABLED 1       // 取消注释以启用 DHT11
// #define SCBB_RD03_V2_ENABLED 1     // 取消注释以启用 RD03_V2
// #define SCBB_INA226_ENABLED 1      // 取消注释以启用 INA226
// #define SCBB_DS1302_ENABLED 1      // 取消注释以启用 DS1302
// #define SCBB_OLED_096_SPI_ENABLED 1 // 取消注释以启用 OLED_096_SPI
```

3. 重新编译

### 关于 `scbb_config.h`

`scbb_config.h` 是本项目的核心配置文件，它控制：

- **哪些模块被编译** — `#define SCBB_<MODULE>_ENABLED 1` 启用对应模块
- **BSP 抽象宏** — ACLL 宏将模块调用映射到你的硬件（如 `AXK_CH224A_I2C_ACLL` → `bsp_i2c_init()`）
- **BSP 头文件和前缀配置** — 每个模块使用的头文件名和函数前缀

此文件由 **menuconfig.py 自动生成**，并在 **CMake 配置时读取**。所有模块头文件（`axk_ch224.h`、`axk_sht3x.h` 等）都包含 `scbb_config.h` 以获取 BSP 配置。

> **不要手动编辑 `scbb_config.h`** — 重新运行 `python menuconfig.py` 即可重新生成。如需自定义配置，请在 menuconfig 中修改选项，或使用上述手动配置方法。

## 集成到项目

### 方式 A：CMake add_subdirectory（推荐）

将 AiPi-SCBB 放在你的项目目录下，然后在 `CMakeLists.txt` 中：

```cmake
add_subdirectory(AiPi-SCBB)

add_executable(your_app main.c)
target_link_libraries(your_app PRIVATE AiPi::SCBB)
```

### 方式 B：CMake FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    aipi_scbb
    GIT_REPOSITORY https://github.com/Ai-Thinker-Open/AiPi-SCBB.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(aipi_scbb)

add_executable(your_app main.c)
target_link_libraries(your_app PRIVATE AiPi::SCBB)
```

### 方式 C：手动添加源文件

如果使用非 CMake 构建系统（如 Keil、IAR、Makefile），需要手动添加：

1. 将所需模块的 `.c` 和 `.h` 文件加入工程：
   - `CH224A/axk_ch224.c` + `axk_ch224.h`
   - `SHT3x/axk_sht3x.c` + `axk_sht3x.h`
   - `WS2812/axk_ws2812.c` + `axk_ws2812.h` + `color_mode.c` + `color_mode.h`
   - `HXD039B2/axk_hxd039b2.c` + `axk_hxd039b2.h`
   - `ST7789V_LCD/axk_st7789v_lcd.c` + `axk_st7789v_lcd.h`
   - `RELAY/axk_relay.c` + `axk_relay.h`
   - `DHT11/axk_dht11.c` + `axk_dht11.h`
   - `RD03_V2/axk_rd03_v2.c` + `axk_rd03_v2.h`
   - `INA226/axk_ina226.c` + `axk_ina226.h`
   - `DS1302/axk_ds1302.c` + `axk_ds1302.h`
   - `OLED_096_SPI/axk_oled_096_spi.c` + `axk_oled_096_spi.h`

2. 如果启用 `SCBB_USE_BSP`，还需添加 `BSP/Ai-M6x/`（博流 BL616/BL618）或 `BSP/stm32f10x/`（STM32F10x）下对应的 BSP 源文件

3. 确保 `scbb_config.h` 在头文件搜索路径中

4. 提供以下外部依赖头文件（项目本身不包含）：

| 依赖 | 说明 |
|------|------|
| `log.h` | 日志宏（由你的固件提供） |
| `FreeRTOS.h` / `task.h` / `timers.h` | FreeRTOS 实时操作系统（可选，部分模块使用） |

## 使用示例

以下示例读取 DHT11 温湿度传感器（需启用 `SCBB_DHT11_ENABLED` 并提供 GPIO + 延时 BSP）：

```c
#include "axk_dht11.h"

float temp = 0.0f;
float humi = 0.0f;

int main(void) {
    int ret = axk_dht11_read(&temp, &humi);
    if (ret == 0) {
        /* temp 和 humi 现在包含有效读数 */
    }
    return ret;
}
```

每个模块头文件（`axk_*.h`）都说明了 API 及其返回值，更多使用示例可参考对应头文件。

## 更新代码

```bash
git pull origin master
```

更新后如需重新生成配置，重新运行 `python menuconfig.py`。

## 目录结构

```
AiPi-SCBB/
├── CH224A/           # USB-PD sink 控制器驱动（I2C）
├── SHT3x/            # 温湿度传感器驱动（I2C）
├── INA226/           # 电压/电流/功率监测驱动（I2C）
├── WS2812/           # RGB LED 灯条驱动（PWM+DMA）
├── HXD039B2/         # 红外编解码驱动（UART+GPIO）
├── DHT11/            # 温湿度传感器驱动（单总线）
├── DS1302/           # RTC 实时时钟驱动（3 线 GPIO）
├── RELAY/            # 继电器驱动（GPIO）
├── RD03_V2/          # 毫米波雷达驱动（UART）
├── OLED_096_SPI/     # 0.96" OLED 显示屏驱动（SPI）
├── ST7789V_LCD/      # ST7789V LCD 驱动（SPI）
├── BSP/              # 板级支持包
│   ├── Ai-M6x/       # 博流 BL616/BL618 板级支持包（I2C, SPI, UART, GPIO, LCD, delay）
│   └── stm32f10x/    # STM32F10x 板级支持包（I2C, SPI, UART, GPIO, PWM+DMA, delay）
├── scripts/          # 配置生成脚本
├── Kconfig           # menuconfig 配置定义
├── menuconfig.py     # 图形化配置工具入口
├── CMakeLists.txt    # CMake 构建文件
└── scbb_config.h     # 由 menuconfig 自动生成（git 忽略）
```

## FAQ

**Q：没有生成 `scbb_config.h`？**
A：运行 `python menuconfig.py`，按 `S` 保存后再按 `Q` 退出。CMake 会在配置阶段读取该文件。

**Q：编译器找不到 `bl616_bsp_*.h` / `stm32f10x_bsp_*.h`？**
A：将对应的 BSP 协议目录（`BSP/<平台>/<协议>`）加入头文件搜索路径；或在 CMake 中启用 `SCBB_USE_BSP`，路径会自动添加。

**Q：如何把本库移植到新的 MCU？**
A：实现模块头文件声明的 BSP 函数（GPIO、I2C、SPI、UART、延时、PWM+DMA），并通过 menuconfig 把模块的 BSP 头文件/前缀指向你的实现。

其他问题排查：请提交 GitHub Issue，并注明模块名称、目标平台和编译错误日志。

## 贡献指南

欢迎任何形式的贡献！请：

1. Fork 本仓库并创建功能分支。
2. 遵循 Ai-Thinker C 编码规范（见 `AGENTS.md`）——`axk_` 前缀、Doxygen 注释、统一格式。
3. 新增驱动时同步更新模块表和 README。
4. 提交 Pull Request，并在描述中说明改动内容。

Bug 报告和功能建议也可以直接提交 GitHub Issue。

## 许可证

[MIT License](LICENSE)
