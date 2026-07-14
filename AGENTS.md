# AGENTS.md — AiPi-FWMod

## What this is

Peripheral driver library for **STM32F10x** (ARM Cortex-M3, HAL-based). Provides hardware abstraction for external components over I2C, PWM+DMA, and UART. Depends on **FreeRTOS** and **STM32 HAL** (`stm32f1xx_hal.h`). This is a **library**, not a complete firmware — there is no build system, linker script, or `main()` here.

## CMake 构建

通过 CMake Presets 配置构建：

```bash
# 查看所有可用 presets
cmake --list-presets

# 使用 preset 配置（如只启用 HXD039B2）
cmake --preset hxd039b2

# 启用所有模块
cmake --preset all

# 使用自定义 BSP
cmake --preset all-custom-bsp

# 构建
cmake --build build
```

可用 Presets：

| Preset | 说明 |
|--------|------|
| `default` | 默认配置，使用内置 BSP |
| `custom-bsp` | 禁用内置 BSP，使用自定义实现 |
| `ch224a` | 只启用 CH224A |
| `sht3x` | 只启用 SHT3x |
| `ws2812` | 只启用 WS2812 |
| `hxd039b2` | 只启用 HXD039B2 |
| `all` | 启用所有模块 |
| `all-custom-bsp` | 启用所有模块，使用自定义 BSP |

CMake 选项：

| 选项 | 默认 | 说明 |
|------|------|------|
| `SCBB_USE_BSP` | ON | 使用内置 STM32F10x BSP，OFF 时需提供自定义 BSP |
| `SCBB_CH224A` | OFF | USB-PD 驱动 (I2C) |
| `SCBB_SHT3X` | OFF | 温湿度传感器 (I2C) |
| `SCBB_WS2812` | OFF | LED 灯条驱动 (PWM+DMA) |
| `SCBB_HXD039B2` | OFF | 红外编解码 (UART) |

BSP 说明：

- `SCBB_USE_BSP=ON`：自动包含 `STM32F10x_bsp/` 下的 I2C、PWM+DMA、Delay
- `SCBB_USE_BSP=OFF`：用户需自行提供 `bsp_i2c_*`、`bsp_pwm_dma_*`、`delay_ms` 等函数实现

## 外部工程引用

### 方法 1: Git Submodule + add_subdirectory

```bash
# 在外部工程中添加子模块
git submodule add git@github.com:Ai-Thinker-Open/AiPi-SCBB.git libs/scbb
```

外部工程 `CMakeLists.txt`：

```cmake
# 添加 SCBB 子目录并配置模块
set(SCBB_HXD039B2 ON CACHE BOOL "" FORCE)
set(SCBB_SHT3X ON CACHE BOOL "" FORCE)
set(SCBB_USE_BSP OFF CACHE BOOL "" FORCE)  # 使用自定义 BSP
add_subdirectory(libs/scbb)

# 链接到你的目标
target_link_libraries(your_target PRIVATE AiPi::SCBB)
```

### 方法 2: FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    scbb
    GIT_REPOSITORY git@github.com:Ai-Thinker-Open/AiPi-SCBB.git
    GIT_TAG master
)

# 配置模块（FetchContent 调用前设置）
set(SCBB_HXD039B2 ON)
set(SCBB_USE_BSP OFF)

FetchContent_MakeAvailable(scbb)

target_link_libraries(your_target PRIVATE AiPi::SCBB)
```

### 方法 3: 手动复制源码

直接复制所需模块目录（如 `HXD039B2/`、`SHT3x/`）到外部工程，在自己的 `CMakeLists.txt` 中编译。

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
