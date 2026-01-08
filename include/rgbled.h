#pragma once
#include <Arduino.h>

class RGBLed
{
private:
  uint8_t pins[3]; // [0]:R, [1]:G, [2]:B
  bool isCommonCathode;
  uint16_t fadeDuration;
  uint8_t currentColor[3];
  const uint8_t (*colors)[3]; // 颜色数组指针
  uint16_t colorCount;
  uint16_t colorIndex;
  uint8_t fadeStep = 0;         // 当前渐变步骤
  uint32_t fadeStartTime = 0;   // 渐变开始时间
  uint8_t startColor[3] = {0};  // 渐变起始颜色
  uint8_t targetColor[3] = {0}; // 渐变目标颜色
public:
  bool isFading = false; // 渐变状态标志

  // 构造函数（去除了C++11特性）
  RGBLed(uint8_t rPin, uint8_t gPin, uint8_t bPin, bool commonCathode = true);
  void begin();
  void setColor(uint8_t r, uint8_t g, uint8_t b);
  void setColorSequence(const uint8_t (*colorArray)[3], uint16_t count);
  void fadeToNext();
  void setFadeDuration(uint16_t duration);
  void update(); // 新增更新方法
};
