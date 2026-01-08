#pragma once
#include <Arduino.h>

class PIRSensor
{
private:
    int pin;
    static PIRSensor *instance;

public:
    PIRSensor(int sensorPin);
    void setup();
    bool checkMotion();
    volatile bool motionDetected;
    unsigned long lastTriggerTime;
};
