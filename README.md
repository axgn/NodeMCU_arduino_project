# 项目名称

NodeMCU Arduino 项目

## 项目简介

本项目旨在使用 NodeMCU 和 Arduino 平台开发一个智能家居系统。通过集成多种传感器和模块，实现对家庭环境的监控和控制。

## 文件结构

```sh
platformio.ini          # PlatformIO 配置文件
include/                # 头文件目录
  AS608.h              # 指纹传感器模块头文件
  breathingled.h        # 呼吸灯模块头文件
  gp2y1014au.h          # 灰尘传感器模块头文件
  lightsensor.h         # 光线传感器模块头文件
  motor_controller.h    # 电机控制模块头文件
  network_manager.h     # 网络管理模块头文件
  pir_sensor.h          # 人体红外传感器模块头文件
  RC522.h               # RFID 模块头文件
  rgbled.h              # RGB LED 模块头文件
  servo_controller.h    # 舵机控制模块头文件
  timerutil.h           # 定时器工具模块头文件
  ws218.h               # WS218 LED 模块头文件
lib/                    # 库文件目录
src/                    # 源代码目录
  AS608.cpp            # 指纹传感器模块实现
  breathingled.cpp      # 呼吸灯模块实现
  gp2y1014au.cpp        # 灰尘传感器模块实现
  lightsensor.cpp       # 光线传感器模块实现
  motor_controller.cpp  # 电机控制模块实现
  network_manager.cpp   # 网络管理模块实现
  pir_sensor.cpp        # 人体红外传感器模块实现
  RC522.cpp             # RFID 模块实现
  rgbled.cpp            # RGB LED 模块实现
  servo_controller.cpp  # 舵机控制模块实现
  timerutil.cpp         # 定时器工具模块实现
  main/                 # 主程序目录
    bathroom.cpp        # 浴室相关逻辑
    bedroom.cpp         # 卧室相关逻辑
    door.cpp            # 门禁相关逻辑
    fingerprint.cpp     # 指纹识别逻辑
    history.cpp         # 历史记录逻辑
    IDcard.cpp          # 身份卡逻辑
    kitchen.cpp         # 厨房相关逻辑
    livingroom.cpp      # 客厅相关逻辑
test/                   # 测试目录
```

## 环境配置

1. 安装 [PlatformIO](https://platformio.org/)
2. 克隆项目到本地：

   ```bash
   git clone <仓库地址>
   ```

3. 使用 PlatformIO 打开项目目录。
4. 根据需要修改 `platformio.ini` 文件中的配置。

## 功能模块

- **指纹识别模块**：通过 AS608 模块实现指纹录入与识别。
- **呼吸灯模块**：控制 LED 灯的亮度变化，模拟呼吸效果。
- **灰尘传感器模块**：使用 GP2Y1014AU 监测空气中的灰尘浓度。
- **光线传感器模块**：检测环境光强度。
- **电机控制模块**：控制电机的启停与转速。
- **网络管理模块**：管理设备的 WiFi 连接。
- **人体红外传感器模块**：检测人体活动。
- **RFID 模块**：通过 RC522 实现身份识别。
- **RGB LED 模块**：控制 RGB 灯的颜色与亮度。
- **舵机控制模块**：控制舵机的旋转角度。
- **定时器工具模块**：提供定时功能。

## 运行项目

1. 连接 NodeMCU 到电脑。
2. 在 PlatformIO 中选择对应的环境（如 `nodemcuv2`）。
3. 使用ctrl+alt+b编译，ctrl+alt+u上传。

## 贡献

欢迎提交 Issue 和 Pull Request 来改进本项目。

## 许可证

本项目采用 MIT 许可证。详情请参阅 LICENSE 文件。
