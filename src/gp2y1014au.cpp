#include "GP2Y1014AU.h"

GP2Y1014AU::GP2Y1014AU(uint8_t ledPin, uint8_t analogPin, uint8_t samples)
    : _ledPin(ledPin),
      _analogPin(analogPin),
      _samples(samples),
      _refVoltage(3.3) {} // 根据实际供电调整

void GP2Y1014AU::begin()
{
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW); // 初始关闭LED
}

uint16_t GP2Y1014AU::readRaw()
{
    // 严格遵循传感器时序要求
    digitalWrite(_ledPin, HIGH);
    delayMicroseconds(280); // 关键时序1：LED激活时间

    uint16_t adcValue = analogRead(_analogPin);

    delayMicroseconds(19); // 关键时序2：补偿ADC采样时间
    digitalWrite(_ledPin, LOW);

    delayMicroseconds(9680); // 关键时序3：保持总周期10ms

    return adcValue;
}

uint16_t GP2Y1014AU::getDustDensity()
{
    float voltage = (readRaw() * _refVoltage / 1024.0) * 2; // 修正系数
    Serial.println(readRaw());

    int density = (0.17 * voltage - 0.1) * 1000; // 转换公式

    // 数据限幅
    if (density < 0)
        return 0;
    if (density > 500)
        return 500;
    return density;
}

uint16_t GP2Y1014AU::getAverageDensity()
{
    uint32_t total = 0;
    for (uint8_t i = 0; i < _samples; i++)
    {
        total += getDustDensity();
        delay(5); // 间隔5ms采样
    }
    return total / _samples;
}
