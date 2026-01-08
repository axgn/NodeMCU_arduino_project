#include "TimerUtil.h"

// ========== 成员方法实现 ==========
void TimerUtil::start(unsigned long offsetMs)
{
    _startTime = millis();
}

bool TimerUtil::isElapsed(unsigned long durationMs)
{
    if (_startTime == 0)
    {
        return false;
    }
    return (millis() - _startTime) > durationMs;
}

unsigned long TimerUtil::remaining(unsigned long durationMs)
{
    unsigned long elapsed = millis() - _startTime;
    return (elapsed >= durationMs) ? 0 : (durationMs - elapsed);
}

unsigned long TimerUtil::elapsed()
{
    return millis() - _startTime;
}

// ========== 静态方法实现 ==========
unsigned long TimerUtil::getFutureTime(unsigned long msLater)
{
    return millis() + msLater;
}

bool TimerUtil::isTimeReached(unsigned long targetTime)
{
    return (long)(millis() - targetTime) >= 0;
}
