# AGENTS.md — AiPi-FWMod

## What this is

Peripheral driver library for **STM32F10x** (ARM Cortex-M3, HAL-based). Provides hardware abstraction for external components over I2C, PWM+DMA, and UART. Depends on **FreeRTOS** and **STM32 HAL** (`stm32f1xx_hal.h`). This is a **library**, not a complete firmware — there is no linker script or `main()` here.

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

1. 复制 `scbb_config_template.h` 为 `scbb_config.h`
2. 取消注释需要的模块 `#define`
3. 重新编译

### 方法 3: CMake -D 参数

```bash
cmake -B build
cmake --build build
```

配置选项（通过 `-D` 传递）：

| 选项 | 默认 | 说明 |
|------|------|------|
| `SCBB_USE_BSP` | OFF | 使用内置 STM32F10x BSP |
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
| `WS2812/` | Addressable RGB LED strip driver + HSV/RGB color utilities | PWM+DMA (TIM1 CH4) |
| `HXD039B2/` | IR encoder/decoder (AC remote control) | UART |
| `STM32F10x_bsp/` | Board Support Package: I2C, PWM+DMA, UART, GPIO, delay | — |

BSP sub-directories: `i2c/` (bit-banged I2C), `pwm_dma/` (WS2812 DMA driver), `uart/` (UART), `gpio/` (GPIO), `delay/` (us/ms delay via `HAL_Delay_us`/`HAL_Delay`).

## I2C abstraction pattern

All I2C modules use `__has_include` to require `stm32f10x_bsp_i2c.h`. Each module defines its own call macro:

```c
// Pattern used by CH224A and SHT3x:
#define AXK_CH224_I2C_ACLL(_func, ...) bsp_i2c_##_func(__VA_ARGS__)
```

This token-pasting macro expands `bsp_i2c_start()`, `bsp_i2c_send_byte()`, etc. **Do not call BSP functions directly** from module code — always go through the macro.

BSP I2C API (bit-banged on PB6=SDA, PB7=SCL):
- `bsp_i2c_init`, `bsp_i2c_start`, `bsp_i2c_stop`
- `bsp_i2c_send_byte`, `bsp_i2c_read_byte`
- `bsp_i2c_wait_ack`, `bsp_i2c_send_ack`

## WS2812 PWM+DMA pattern

WS2812 uses a similar macro abstraction: `AXK_WS2812_ACLL(_func, ...)` → `bsp_pwm_dma_##_func(...)`. Data format is **GRB** (not RGB) when transmitted to the LED strip.

LED strip state is held in a global `axk_ws2812_strip_dev` pointer. Call `axk_ws2812_init()` with a stack-allocated `axk_ws2812_strip_t` before using any LED functions. Brightness is applied as an HSV `v` channel multiplier, not direct RGB scaling.

Note: `axk_ws2812.h` forward-declares `bsp_pwm_dma_deinit()` even though it lives in `stm32f10x_pwm_dma.c`. This cross-module extern is intentional.

## UART pattern (HXD039B2)

HXD039B2 uses UART protocol: `AXK_HXD039B2_UART_ACLL(_func, ...) bsp_uart_##_func(__VA_ARGS__)`. The module also uses GPIO for power control and busy detection: `AXK_HXD039B2_GPIO_ACLL(_func, ...) bsp_gpio_##_func(__VA_ARGS__)`.

## Naming conventions

- Module prefix: `axk_` (e.g. `axk_ch224_init`, `axk_sht3x_read`, `axk_ws2812_set_pixel_color`)
- BSP prefix: `bsp_` (e.g. `bsp_i2c_start`, `bsp_pwm_dma_init`)
- Color types: `color_t` (RGB), `hsv_color_t` (HSV) — defined in `WS2812/color_mode.h`
- Header guards are inconsistent: `__AXK_SHT3X_H__` (double), `_AXK_CH224_H_` (single), `COLOR_MODE_H` (none). BSP uses double-underscore. Match the file you're editing.

## Adding a new I2C peripheral

1. Create `ModuleName/axk_module.h` and `.c`
2. Add `#if __has_include("stm32f10x_bsp_i2c.h")` guard and `AXK_MODULE_I2C_ACLL` macro
3. Use `bsp_i2c_*` functions through the macro — never call them directly
4. Follow the I2C transaction pattern: start → send addr+write → wait_ack → send reg → wait_ack → restart → send addr+read → read bytes → NACK → stop

## External dependencies (not in this repo)

- `log.h` — logging macros (`log_error`, etc.) provided by the host firmware
- `tim.h` — HAL timer handle (`htim1`) for PWM+DMA
- `FreeRTOS.h`, `task.h`, `timers.h` — RTOS primitives
- `stm32f1xx_hal.h` — STM32 HAL

## Gotchas

- WS2812 data order is **GRB**, not RGB — the `convert_rgb_to_ws2812_data` function handles the swap
- WS2812 max LED count is hardcoded to `60` (`WS2812_MAX_NUM`)
- SHT3x CRC uses polynomial `0x31` — do not change without checking the datasheet
- CH224 PPS/AVS voltage encoding uses `value * 10` as a byte — precision is 0.1V steps
- `color_mode.c` includes FreeRTOS headers (`task.h`, `timers.h`) even though it's a pure math module — this is a coupling you'll need to work around if porting
- `axk_ws2812_init` mallocs `dev` array based on `led_count`; no bounds check against `WS2812_MAX_NUM` at the module level (BSP clamps in `bsp_pwm_dma_with_num`)
- CH224A header guard `_AXK_CH224_H_` differs from SHT3x/WS2812 — don't normalize guards across modules without checking include paths
