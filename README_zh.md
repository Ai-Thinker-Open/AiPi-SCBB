# AiPi-SCBB

[![English](https://img.shields.io/badge/English-README-blue)](README.md)

Ai-Thinker STM32F10x 外设驱动库，为 I2C、PWM+DMA、UART 协议的外部器件提供统一的硬件抽象层。

基于 STM32 HAL 和 FreeRTOS 构建，适用于 AiPi 系列开发板。

## 支持的模块

| 模块 | 说明 | 协议 | 地址 |
|------|------|------|------|
| CH224A | USB-PD sink 控制器（电压协商：5-28V, PPS, AVS） | I2C | 0x22 |
| SHT3x | 温湿度传感器 | I2C | 0x44 |
| WS2812 | RGB LED 灯条驱动 + HSV/RGB 颜色工具 | PWM+DMA | — |
| HXD039B2 | 红外编解码器（空调遥控器） | UART+GPIO | — |

## 环境准备

### Python 依赖（menuconfig 需要）

```bash
pip install kconfiglib windows-curses
```

### Windows 编译环境（MSYS2）

1. 安装 [MSYS2](https://www.msys2.org/)
2. 打开 **MSYS2 UCRT64** 终端
3. 安装工具链：

```bash
pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gcc make
```

4. 安装 Python：

```bash
pacman -S mingw-w64-ucrt-x86_64-python pip
```

5. 将 `C:\msys64\ucrt64\bin` 添加到系统 PATH

### Linux / macOS

确保已安装 `cmake`、`gcc` 和 Python 3.x。

## 配置模块

### 方法 1：menuconfig.py（推荐）

```bash
python menuconfig.py
```

操作流程：

1. 运行命令后进入图形化配置界面
2. 用方向键选择模块，按 `Y` 勾选启用
3. 按 `S` 保存配置
4. 按 `Q` 退出

保存后会自动生成 `scbb_config.h`，CMake 会自动读取该文件确定启用的模块，无需手动传递 `-D` 参数。

### 方法 2：手动编辑配置文件

1. 将 `config/scbb_config.h` 复制到项目根目录
2. 取消注释需要启用的模块：

```c
#define SCBB_CH224A_ENABLED 1   // 启用 CH224A
#define SCBB_SHT3X_ENABLED 1    // 启用 SHT3x
// #define SCBB_WS2812_ENABLED 1  // 取消注释以启用 WS2812
// #define SCBB_HXD039B2_ENABLED 1 // 取消注释以启用 HXD039B2
```

3. 重新编译

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

2. 如果启用 `SCBB_USE_BSP`，还需添加 `STM32F10x_bsp/` 下对应的 BSP 源文件

3. 确保 `scbb_config.h` 在头文件搜索路径中

4. 提供以下外部依赖头文件（项目本身不包含）：

| 依赖 | 说明 |
|------|------|
| `stm32f1xx_hal.h` | STM32 HAL 库 |
| `FreeRTOS.h` / `task.h` / `timers.h` | FreeRTOS 实时操作系统 |
| `log.h` | 日志宏（由宿主固件提供） |
| `tim.h` | HAL 定时器句柄（`htim1`，用于 PWM+DMA） |

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
├── WS2812/           # RGB LED 灯条驱动（PWM+DMA）
├── HXD039B2/         # 红外编解码驱动（UART+GPIO）
├── STM32F10x_bsp/    # 板级支持包（I2C, PWM+DMA, UART, GPIO, delay）
├── config/           # scbb_config.h 模板
├── scripts/          # 配置生成脚本
├── Kconfig           # menuconfig 配置定义
├── menuconfig.py     # 图形化配置工具入口
├── CMakeLists.txt    # CMake 构建文件
└── scbb_config.h     # 由 menuconfig 自动生成（git 忽略）
```

## 许可证

[MIT License](LICENSE)
