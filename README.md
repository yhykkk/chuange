# STM32 示例工程 (创格杯示例)

本工程基于 STM32F1 系列 HAL 库（STM32F103 系列），演示了 OLED 显示、HC-SR04 超声波测距（基于定时器输入捕获）和 PWM 输出的用法。

## 快速概览

- MCU: STM32F103 系列（工程中包含 `startup_stm32f103c8tx.s` 和链接脚本）。
- 项目结构（与主要文件）:
  - `Core/Src/main.c` - 程序入口与主循环（包含 OLED / HCSR04 / PWM 的宏开关）。
  - `Core/Inc/gpio.h`, `Core/Src/gpio.c` - 引脚初始化与映射。
  - `Core/Src/i2c.c` - I2C 驱动（用于 OLED）。
  - `Core/Src/oled.c`, `Core/Inc/oled.h` - OLED 驱动与显示函数。
  - `Core/Src/tim.c` - 定时器配置（TIM1、TIM2）。
  - `Core/Src/font.c` - 字体数据。
  - `Debug/` - 构建输出与 makefile（用于编译）。

## 特性（由宏控制）

在 `Core/Src/main.c` 文件中通过宏控制功能启用：

- `OLED`：初始化并循环刷新 OLED 屏幕（通过 I2C1）。
- `HCSR04`：使用 TIM1 的输入捕获功能测量 HC-SR04 超声波模块的距离，并将结果显示到 OLED。
- `PWM`：初始化 TIM2 的 PWM 并设置比较值（占空比）。

在实际使用时，你可以在 `main.c` 顶部通过注释/取消注释这些宏来启用或禁用相应功能。

## 主要引脚（请以 `Core/Inc/gpio.h` 或 `Core/Src/gpio.c` 为准）

- HC-SR04 Trig: 代码中示例为 `GPIOA PIN 9`（见 `main.c` 中的 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, ...)`）。
- HC-SR04 Echo: 使用 TIM1 的输入捕获通道（代码读取 TIM_CHANNEL_3 和 TIM_CHANNEL_4 的捕获值），请参阅 `tim.c` 与 `gpio.c` 中具体的引脚映射。
- OLED: 通过 `I2C1`（SCL/SDA）连接。具体引脚在 `i2c.c` / `gpio.c` 中定义。
- 其余外设与引脚映射请检查 `Core/Inc/gpio.h` 和 `Core/Src/gpio.c`。

## main.c 工作流程（要点）

1. 调用 `HAL_Init()` 和 `SystemClock_Config()`（使用 HSE + PLL x9，详见 `SystemClock_Config`）。
2. 初始化外设：`MX_GPIO_Init()`, `MX_I2C1_Init()`, `MX_TIM1_Init()`, `MX_TIM2_Init()`。
3. 根据宏启用相应模块：
   - `OLED`：调用 `OLED_Init()`，在主循环中使用 `OLED_NewFrame()` / `OLED_PrintString()` / `OLED_ShowFrame()` 更新显示。
   - `HCSR04`：启动 TIM1 基本计数与输入捕获（通道3/4），通过触发 Trig（PA9）产生脉冲，然后在捕获回调 `HAL_TIM_IC_CaptureCallback` 中读取上升沿和下降沿计数并计算距离，最后在 OLED 上显示结果。
     - 距离计算公式（代码）：
       distance = ((downEdge - upEdge) * 0.034) / 2
       （单位 cm，基于声速 340 m/s = 0.034 cm/μs，注意定时器时基）
   - `PWM`：初始化并启动 TIM2 PWM 输出，通过 `__HAL_TIM_SetCompare` 设置占空比。

## 调试与常见问题

- 如果无法看到 OLED 显示：
  - 检查 I2C 总线的 SCL/SDA 引脚和电源（3.3V）是否正确连接。
  - 使用示波器或逻辑分析仪确认 I2C 通信是否发出。
  - 检查 `i2c.c` 中时序和时钟配置。
- 超声测距结果异常：
  - 确认 Echo 线接到对应的 TIM1 捕获通道（与 `tim.c` / `gpio.c` 中的配置一致）。
  - 检查定时器时基（预分频、计数频率），计算公式假设定时器计数单位为微秒或相应的时间单位。
  - 确认触发脉冲宽度与模块规格一致（一般 Trig 需要至少 10μs 高脉冲）。
- 烧录失败：检查 ST-Link 驱动、权限（以管理员运行 PowerShell）或设备电源/连接。
- 
## 参考文件

- `Core/Src/main.c`（主程序）
- `Core/Src/font.c`, `Core/Src/oled.c`（OLED 驱动）

---
