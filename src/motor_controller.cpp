#include "motor_controller.h"

MotorController::MotorController(int pin1, int pin2, int range)
    : in1Pin(pin1), in2Pin(pin2), pwmRange(range), currentSpeed(0)
{
}

void MotorController::setup()
{
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);
    analogWriteRange(pwmRange);
    emergencyStop();
}

void MotorController::setSpeed(int speed)
{
    speed = constrain(speed, -pwmRange, pwmRange);
    currentSpeed = speed;

    if (speed > 0)
    {
        analogWrite(in1Pin, abs(speed));
        digitalWrite(in2Pin, LOW);
    }
    else if (speed < 0)
    {
        digitalWrite(in1Pin, LOW);
        analogWrite(in2Pin, abs(speed));
    }
    else
    {
        digitalWrite(in1Pin, LOW);
        digitalWrite(in2Pin, LOW);
    }
}

void MotorController::emergencyStop()
{
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, LOW);
    currentSpeed = 0;
}

void MotorController::setPwmFrequency(int freq)
{
    analogWriteFreq(freq);
}

int MotorController::getCurrentSpeed() const
{
    return currentSpeed;
}
