
// pir_sensor.cpp
#include "pir_sensor.h"

PIRSensor *PIRSensor::instance = nullptr;

PIRSensor::PIRSensor(int sensorPin) : pin(sensorPin), motionDetected(false), lastTriggerTime(0)
{
    instance = this;
}

void PIRSensor::setup()
{
    pinMode(pin, INPUT);
}

bool PIRSensor::checkMotion()
{
    if (motionDetected)
    {
        motionDetected = false;
        return true;
    }
    return false;
}
