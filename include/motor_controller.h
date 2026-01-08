#pragma once
#include <Arduino.h>

class MotorController
{
private:
    int in1Pin;       // 电机控制引脚1
    int in2Pin;       // 电机控制引脚2
    int pwmRange;     // PWM范围（默认255）
    int currentSpeed; // 当前速度值

public:
    /**
     * @brief 构造函数
     * @param pin1 电机控制引脚1（PWM）
     * @param pin2 电机控制引脚2（PWM）
     * @param range PWM分辨率（默认255）
     */
    MotorController(int pin1, int pin2, int range = 255);
    void setup();

    void setSpeed(int speed);

    void emergencyStop();

    void setPwmFrequency(int freq);

    int getCurrentSpeed() const;
};
