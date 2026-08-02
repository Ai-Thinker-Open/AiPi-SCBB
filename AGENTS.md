# AGENTS.md — AiPi-FWMod

## What this is

Multi-platform peripheral driver library with a configurable BSP (Board Support Package) abstraction. Built-in BSPs: **Ai-M6x** (Bouffalo BL616/BL618, bflb SDK) and **STM32F10x** (ARM Cortex-M3, HAL-based). Provides hardware abstraction for external components over I2C, SPI, PWM+DMA, UART, and GPIO. Depends on **FreeRTOS** and the platform SDK/HAL (e.g. `stm32f1xx_hal.h`). This is a **library**, not a complete firmware — there is no linker script or `main()` here.

## 环境准备（Windows）

C/C++ 编译和 menuconfig 依赖 MSYS2 终端，请先安装：

1. 下载安装 MSYS2：https://www.msys2.org/
2. 安装完成后打开 **MSYS2 UCRT64** 终端
3. 安装必要工具：
```bash
pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gcc make
```
4. 安装 Python 依赖（用于 menuconfig）：
```bash
pacman -S mingw-w64-ucrt-x86_64-python pip
pip install kconfiglib windows-curses
```
5. 将 `C:\msys64\ucrt64\bin` 添加到系统 PATH（编译工具链需要）

后续所有编译操作请在 **MSYS2 UCRT64** 终端中执行。

## 配置模块

### 方法 1: 图形化配置（推荐）

需要先安装依赖：
```bash
pip install kconfiglib windows-curses
```

**Windows：** 双击 `menuconfig.bat`，或在终端运行：
```bash
menuconfig.bat
```

**Linux/Mac：**
```bash
make menuconfig
```

在界面中勾选需要的模块，保存后会自动生成 `scbb_config.h`。

### 方法 2: 手动编辑配置文件

1. 在项目根目录创建 `scbb_config.h`（或先运行 `python menuconfig.py` 生成）
2. 添加需要的模块 `#define`
3. 重新编译

### 方法 3: CMake -D 参数

```bash
cmake -B build
cmake --build build
```

配置选项（通过 `-D` 传递）：

| 选项 | 默认 | 说明 |
|------|------|------|
| `SCBB_USE_BSP` | OFF | 使用内置 BSP（配合 `SCBB_BSP_PLATFORM` 选择平台） |
| `SCBB_BSP_PLATFORM` | `Ai-M6x` | 内置 BSP 平台（`Ai-M6x` / `stm32f10x`） |
| `SCBB_CH224A` | OFF | USB-PD 驱动 (I2C) |
| `SCBB_SHT3X` | OFF | 温湿度传感器 (I2C) |
| `SCBB_WS2812` | OFF | LED 灯条驱动 (PWM+DMA) |
| `SCBB_HXD039B2` | OFF | 红外编解码 (UART) |
| `SCBB_BSP_I2C_HEADER` | `stm32f10x_bsp_i2c.h` | 自定义 I2C BSP 头文件名 |
| `SCBB_BSP_UART_HEADER` | `stm32f10x_bsp_uart.h` | 自定义 UART BSP 头文件名 |
| `SCBB_BSP_GPIO_HEADER` | `stm32f10x_bsp_gpio.h` | 自定义 GPIO BSP 头文件名 |
| `SCBB_BSP_DELAY_HEADER` | `stm32f10x_delay.h` | 自定义延时 BSP 头文件名 |

## Directory structure

| Directory | Purpose | Protocol |
|-----------|---------|----------|
| `CH224A/` | USB-PD sink controller (voltage negotiation: 5–28V, PPS, AVS) | I2C (0x22) |
| `SHT3x/` | Temperature & humidity sensor | I2C (0x44) |
| `INA226/` | Voltage/current/power monitor | I2C (0x40) |
| `WS2812/` | Addressable RGB LED strip driver + HSV/RGB color utilities | PWM+DMA (TIM1 CH4) |
| `HXD039B2/` | IR encoder/decoder (AC remote control) | UART |
| `DHT11/` | Temperature & humidity sensor (One-Wire) | GPIO |
| `DS1302/` | RTC real-time clock (3-wire bit-bang) | GPIO |
| `RELAY/` | Relay driver (high-level active) | GPIO |
| `RD03_V2/` | mmWave radar presence & distance sensor | UART |
| `OLED_096_SPI/` | 0.96" OLED display (SSD1306, 128x64) | SPI |
| `ST7789V_LCD/` | ST7789V SPI LCD driver | SPI |
| `BSP/Ai-M6x/` | Bouffalo BL616/BL618 BSP: I2C, SPI, UART, GPIO, LCD, delay | — |
| `BSP/stm32f10x/` | STM32F10x BSP: I2C, SPI, UART, GPIO, PWM+DMA, delay | — |

Each platform lives under `BSP/<platform>/<protocol>/`. Protocol sub-directories: `i2c/`, `spi/`, `uart/`, `gpio/`, `pwm_dma/` (WS2812 DMA driver), `delay/` (us/ms delay), plus platform extras such as `BSP/Ai-M6x/lcd/` and `BSP/Ai-M6x/freertos/`.

## I2C abstraction pattern

All I2C modules include the platform BSP header via a macro from `scbb_config.h` (e.g. `SCBB_CH224A_I2C_HEADER` → `stm32f10x_bsp_i2c.h` or `bl616_bsp_i2c.h`). Each module defines its own call macro:

```c
// Pattern used by CH224A and SHT3x:
#define AXK_CH224A_I2C_ACLL(_func, ...) bsp_i2c##_##_func(__VA_ARGS__)
```

This token-pasting macro expands `bsp_i2c_start()`, `bsp_i2c_send_byte()`, etc. **Do not call BSP functions directly** from module code — always go through the macro.

BSP I2C API (STM32 bit-bang on PB6=SDA, PB7=SCL):
- `bsp_i2c_init`, `bsp_i2c_start`, `bsp_i2c_stop`
- `bsp_i2c_send_byte`, `bsp_i2c_read_byte`
- `bsp_i2c_wait_ack`, `bsp_i2c_send_ack`

## WS2812 PWM+DMA pattern

WS2812 uses a similar macro abstraction: `AXK_WS2812_ACLL(_func, ...)` → `bsp_pwm_dma_##_func(...)`. Data format is **GRB** (not RGB) when transmitted to the LED strip.

LED strip state is held in a global `axk_ws2812_strip_dev` pointer. Call `axk_ws2812_init()` with a stack-allocated `axk_ws2812_strip_t` before using any LED functions. Brightness is applied as an HSV `v` channel multiplier, not direct RGB scaling.

Note: `axk_ws2812.h` forward-declares `bsp_pwm_dma_deinit()` even though it lives in `BSP/stm32f10x/pwm_dma/stm32f10x_pwm_dma.c`. This cross-module extern is intentional.

## UART pattern (HXD039B2)

HXD039B2 uses UART protocol: `AXK_HXD039B2_UART_ACLL(_func, ...) bsp_uart_##_func(__VA_ARGS__)`. The module also uses GPIO for power control and busy detection: `AXK_HXD039B2_GPIO_ACLL(_func, ...) bsp_gpio_##_func(__VA_ARGS__)`.

## Naming conventions

- Module prefix: `axk_` (e.g. `axk_ch224_init`, `axk_sht3x_read`, `axk_ws2812_set_pixel_color`)
- BSP prefix: `bsp_` (e.g. `bsp_i2c_start`, `bsp_pwm_dma_init`)
- Color types: `axk_color_t` (RGB), `axk_hsv_color_t` (HSV) — defined in `WS2812/color_mode.h`
- Header guards: 模块使用 `AXK_XXX_H`（如 `AXK_CH224_H`），BSP 使用 `BL616_BSP_XXX_H` / `STM32F10X_BSP_XXX_H`。新建头文件按此规范

## Adding a new I2C peripheral

1. Create `ModuleName/axk_module.h` and `.c`
2. Add the BSP header include guard (`#ifdef SCBB_<MODULE>_I2C_HEADER` → `#include SCBB_<MODULE>_I2C_HEADER`) and `AXK_MODULE_I2C_ACLL` macro
3. Use `bsp_i2c_*` functions through the macro — never call them directly
4. Follow the I2C transaction pattern: start → send addr+write → wait_ack → send reg → wait_ack → restart → send addr+read → read bytes → NACK → stop

## External dependencies (not in this repo)

- `log.h` — logging macros (`log_error`, etc.) provided by the host firmware
- `tim.h` — HAL timer handle (`htim1`) for PWM+DMA
- `FreeRTOS.h`, `task.h`, `timers.h` — RTOS primitives
- `stm32f1xx_hal.h` — STM32 HAL

## Gotchas

- WS2812 data order is **GRB**, not RGB — the `convert_rgb_to_ws2812_data` function handles the swap
- WS2812 max LED count is `60` — 模块层 `AXK_WS2812_MAX_NUM`，BSP 层 `WS2812_MAX_NUM`
- SHT3x CRC uses polynomial `0x31` — do not change without checking the datasheet
- CH224 PPS/AVS voltage encoding uses `value * 10` as a byte — precision is 0.1V steps
- `color_mode.c` includes FreeRTOS headers (`task.h`, `timers.h`) even though it's a pure math module — this is a coupling you'll need to work around if porting
- `axk_ws2812_init` 校验 `led_count <= AXK_WS2812_MAX_NUM`；`dev` 为 NULL 时使用模块静态缓冲（不动态分配）
- CH224A header guard `_AXK_CH224_H_` differs from SHT3x/WS2812 — don't normalize guards across modules without checking include paths
