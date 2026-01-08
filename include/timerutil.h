#pragma once
#include <Arduino.h>

class TimerUtil
{
private:
    unsigned long _startTime = 0; // 开始时间戳

public:
    // 方法声明
    void start(unsigned long offsetMs = 0);
    bool isElapsed(unsigned long durationMs);
    unsigned long remaining(unsigned long durationMs);
    unsigned long elapsed();

    // 静态方法声明
    static unsigned long getFutureTime(unsigned long msLater);
    static bool isTimeReached(unsigned long targetTime);
};
