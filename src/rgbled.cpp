#include "rgbled.h"
#include "timerutil.h"
// 预定义颜色库
const uint8_t RAINBOW[][3] = {
    {255, 0, 0},   // 红
    {255, 128, 0}, // 橙
    {255, 255, 0}, // 黄
    {0, 255, 0},   // 绿
    {0, 255, 255}, // 青
    {0, 0, 255},   // 蓝
    {255, 0, 255}, // 紫
    {128, 0, 255}  // 品红
};
RGBLed::RGBLed(uint8_t rPin, uint8_t gPin, uint8_t bPin, bool commonCathode)
{
  pins[0] = rPin;
  pins[1] = gPin;
  pins[2] = bPin;
  isCommonCathode = commonCathode;
  fadeDuration = 1000;
  colors = nullptr;
  colorCount = 0;
  colorIndex = 0;
  memset(currentColor, 0, 3);
}
void RGBLed::begin()
{

  pinMode(pins[0], OUTPUT);
  pinMode(pins[1], OUTPUT);
  pinMode(pins[2], OUTPUT);
  analogWriteRange(255);
  setColor(0, 0, 0);
  setFadeDuration(1000);
  setColorSequence(RAINBOW, sizeof(RAINBOW) / 3);
}

void RGBLed::setColor(uint8_t r, uint8_t g, uint8_t b)
{
  currentColor[0] = r;
  currentColor[1] = g;
  currentColor[2] = b;

  analogWrite(pins[0], isCommonCathode ? r : 255 - r);
  analogWrite(pins[1], isCommonCathode ? g : 255 - g);
  analogWrite(pins[2], isCommonCathode ? b : 255 - b);
}

void RGBLed::setColorSequence(const uint8_t (*colorArray)[3], uint16_t count)
{
  colors = colorArray;
  colorCount = count;
  colorIndex = 0;
}

void RGBLed::fadeToNext()
{
  if (!colors || colorCount == 0)
    return;

  // 如果不在渐变状态，初始化渐变参数
  if (!isFading)
  {
    memcpy(startColor, currentColor, 3);
    memcpy(targetColor, colors[colorIndex], 3);
    fadeStartTime = millis();
    fadeStep = 0;
    isFading = true;
  }
}

void RGBLed::update()
{
  if (!isFading)
    return;

  const uint16_t totalSteps = 50;
  const uint16_t stepDuration = fadeDuration / totalSteps;

  // 计算当前步骤
  uint16_t elapsed = millis() - fadeStartTime;
  uint16_t currentStep = elapsed / stepDuration;

  // 限制步骤范围
  currentStep = min(currentStep, totalSteps);

  // 更新颜色
  if (currentStep > fadeStep)
  {
    fadeStep = currentStep;
    float ratio = (float)fadeStep / totalSteps;

    uint8_t r = startColor[0] + (targetColor[0] - startColor[0]) * ratio;
    uint8_t g = startColor[1] + (targetColor[1] - startColor[1]) * ratio;
    uint8_t b = startColor[2] + (targetColor[2] - startColor[2]) * ratio;

    setColor(r, g, b);
  }

  // 渐变完成处理
  if (fadeStep >= totalSteps)
  {
    isFading = false;
    colorIndex = (colorIndex + 1) % colorCount;
  }
}

// 设置RGBLed的渐变持续时间
void RGBLed::setFadeDuration(uint16_t duration)
{
  fadeDuration = duration;
}
