#ifndef GP2Y1014AU_H
#define GP2Y1014AU_H

#include <Arduino.h>

class GP2Y1014AU
{
public:
    // 构造函数（LED控制引脚，模拟输入引脚，采样次数）
    GP2Y1014AU(uint8_t ledPin, uint8_t analogPin, uint8_t samples = 20);

    // 初始化传感器
    void begin();

    // 获取原始ADC值（带时序控制）
    uint16_t readRaw();

    // 计算粉尘浓度（μg/m³）
    uint16_t getDustDensity();

    // 获取平均值滤波后的浓度
    uint16_t getAverageDensity();

private:
    uint8_t _ledPin;    // LED控制引脚
    uint8_t _analogPin; // 模拟输入引脚
    uint8_t _samples;   // 平均采样次数
    float _refVoltage;  // ADC参考电压
};

#endif
