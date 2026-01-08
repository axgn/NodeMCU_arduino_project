#include "BreathingLed.h"
#define LED_PIN D0
BreathingLed::BreathingLed(uint16_t numLeds)
    : ledCount(numLeds)
{
    leds = new CRGB[ledCount];
    currentColor = CRGB::White;
    breathSpeed = 10;
    brightness = 200;
    currentState = OFF;
}

void BreathingLed::begin()
{
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, ledCount)
        .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);
    setBreathRange(20, 200);
    setGlobalBrightness(150);
    setColor(CRGB(255, 0, 0)); // 设置为红色呼吸
    FastLED.clear(true);       // 初始化时清空灯带
    FastLED.show();
}

void BreathingLed::update()
{
    handleBreathing();
}

void BreathingLed::setSpeed(uint8_t speed)
{
    breathSpeed = constrain(speed, 1, 100);
}

void BreathingLed::setColor(CRGB newColor)
{
    currentColor = newColor;
}

// 修改后的呼吸处理函数
void BreathingLed::handleBreathing()
{
    if (millis() - prevMillis < 30)
        return;

    switch (currentState)
    {
    case BREATHING:
    {
        if (currentMode == MODE_BREATHING)
        {
            // 呼吸模式
            uint8_t basePulse = beatsin8(breathSpeed);
            uint8_t scaledPulse = map(basePulse, 0, 255, minBrightness, maxBrightness);
            CRGB fadedColor = currentColor;
            fadedColor.nscale8(scaledPulse);
            fadedColor.nscale8(globalBrightness);
            fill_solid(leds, ledCount, fadedColor);
        }
        else
        {
            // 常亮模式
            CRGB solidColor = currentColor;
            solidColor.nscale8(globalBrightness);
            fill_solid(leds, ledCount, solidColor);
        }
        FastLED.show();
        break;
    }

    case FADING_OUT:
    {
        // 渐暗关闭过程
        int currentBrightness = fadeStartBrightness * (1.0 - (millis() - fadeStartTime) / 2000.0);
        currentBrightness = max(currentBrightness, 0);

        CRGB fadedColor = currentColor;
        fadedColor.nscale8(currentBrightness);
        fill_solid(leds, ledCount, fadedColor);
        FastLED.show();
        if (currentBrightness == 0)
        {
            currentState = OFF;
            FastLED.clear();
        }
        break;
    }

    case OFF:
        // 保持关闭状态
        return;
    }
    prevMillis = millis();
}

// 新增关闭方法
void BreathingLed::beginTurnOff()
{
    if (currentState != OFF)
    {
        currentState = FADING_OUT;
        fadeStartTime = millis();
        fadeStartBrightness = beatsin8(breathSpeed); // 捕获当前亮度值
    }
}

// 新增开启方法
void BreathingLed::turnOn()
{
    currentState = BREATHING;
    fadeStartBrightness = 0;
    FastLED.show();
}

void BreathingLed::setBreathRange(uint8_t min, uint8_t max)
{
    minBrightness = constrain(min, 0, 255);
    maxBrightness = constrain(max, minBrightness, 255);
}

void BreathingLed::setGlobalBrightness(uint8_t brightness)
{
    globalBrightness = constrain(brightness, 1, 255);
    FastLED.setBrightness(globalBrightness);
}
void BreathingLed::setMode(Mode newMode)
{
    if (currentMode != newMode)
    {
        currentMode = newMode;

        // 模式切换时强制刷新
        if (currentState == BREATHING)
        {
            FastLED.show();
        }
    }
}
