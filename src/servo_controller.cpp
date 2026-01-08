// servo_controller.cpp
#include "servo_controller.h"

ServoController::ServoController(int controlPin) : pin(controlPin), currentAngle(90) {}

void ServoController::setup()
{
    servo.attach(pin);
    servo.write(currentAngle);
}

void ServoController::setAngle(int angle)
{
    currentAngle = constrain(angle, -360, 360);
    servo.write(currentAngle);
    delay(15);
}
