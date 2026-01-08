#ifndef BREATHINGLED_H
#define BREATHINGLED_H

#include <FastLED.h>

class BreathingLed
{
public:
    BreathingLed(uint16_t numLeds); // 简化构造函数
    enum Mode
    {
        MODE_CONSTANT, // 常亮模式
        MODE_BREATHING // 呼吸模式
    };
    void setMode(Mode newMode);
    void begin();
    void update();
    void setSpeed(uint8_t speed);
    void setColor(CRGB newColor);
    void beginTurnOff();
    void turnOn();
    void setBreathRange(uint8_t min, uint8_t max);
    void setGlobalBrightness(uint8_t brightness);

private:
    Mode currentMode = MODE_BREATHING;
    void handleBreathing();
    enum State
    {
        BREATHING,
        FADING_OUT,
        OFF
    };
    State currentState = BREATHING;
    uint8_t fadeStartBrightness = 0;
    CRGB *leds;
    CRGB currentColor;
    uint16_t ledCount;
    uint8_t breathSpeed;
    uint8_t brightness;
    unsigned long prevMillis = 0;
    unsigned long fadeStartTime = 0;
    uint8_t minBrightness = 20;     // 呼吸最低亮度
    uint8_t maxBrightness = 200;    // 呼吸最高亮度
    uint8_t globalBrightness = 255; // 全局亮度限制
};

#endif
