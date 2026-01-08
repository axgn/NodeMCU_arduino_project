#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>

class LightSensor
{
public:
  // 构造函数：增加滤波窗口大小参数（默认关闭滤波）
  LightSensor(int analogPin,
              int minVal = 50, int maxVal = 950,
              int filterWindow = 1); // filterWindow=1表示不滤波

  // 初始化传感器
  void begin();

  // 读取原始值（无滤波）
  int readRaw();

  // 读取滤波后的原始值
  int readFilteredRaw();

  // 读取滤波后的光照百分比
  int readPercent();

  // 设置校准范围
  void setCalibration(int minVal, int maxVal);

  // 设置滤波窗口大小（1~10）
  void setFilterWindow(int window);

  // 读取数字信号
  bool readDigital();

private:
  int _analogPin;
  int _digitalPin;
  int _minVal;
  int _maxVal;

  // 滤波相关变量
  static const int MAX_FILTER_WINDOW = 10; // 最大滤波窗口
  int _readings[10];                       // 存储历史数据
  int _index = 0;                          // 当前数据索引
  int _filterWindow = 1;                   // 滤波窗口大小
  long _sum = 0;                           // 数据总和（用于快速计算平均值）
};
#endif
