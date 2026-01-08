#pragma once
#include <Servo.h>

class ServoController
{
private:
    // 定义一个舵机对象
    Servo servo;
    // 定义控制舵机的引脚
    int pin;
    // 定义当前舵机的角度
    int currentAngle;

public:
    // 构造函数，初始化舵机控制引脚
    ServoController(int controlPin);
    // 初始化舵机
    void setup();
    // 设置舵机角度
    void setAngle(int angle);
    // 获取当前舵机角度
    int getCurrentAngle() const;
};
