#include "LightSensor.h"

LightSensor::LightSensor(int analogPin,
                         int minVal, int maxVal, int filterWindow)
{
  _analogPin = analogPin;
  _minVal = minVal;
  _maxVal = maxVal;
  setFilterWindow(filterWindow); // 初始化滤波设置
}

void LightSensor::begin()
{
  // 初始化滤波数组
  for (int i = 0; i < MAX_FILTER_WINDOW; i++)
  {
    _readings[i] = 0;
  }
}

void LightSensor::setFilterWindow(int window)
{
  if (window > MAX_FILTER_WINDOW)
    window = MAX_FILTER_WINDOW;
  if (window < 1)
    window = 1;
  _filterWindow = window;
}

int LightSensor::readRaw()
{
  return analogRead(_analogPin); // 原始数据
}

int LightSensor::readFilteredRaw()
{
  // 移除最旧数据
  _sum -= _readings[_index];
  // 读取新数据
  int newValue = analogRead(_analogPin);
  _readings[_index] = newValue;
  _sum += newValue;
  // 更新索引
  _index = (_index + 1) % _filterWindow;
  // 计算平均值
  return _sum / _filterWindow;
}

int LightSensor::readPercent()
{
  int filteredRaw = readFilteredRaw();
  // 反转映射方向：将 _maxVal（弱光）映射到 0%，_minVal（强光）映射到 100%
  int percent = map(filteredRaw, _maxVal, _minVal, 0, 100);
  return constrain(percent, 0, 100);
}
bool LightSensor::readDigital()
{
  return digitalRead(_digitalPin);
}

void LightSensor::setCalibration(int minVal, int maxVal)
{
  _minVal = minVal;
  _maxVal = maxVal;
}
