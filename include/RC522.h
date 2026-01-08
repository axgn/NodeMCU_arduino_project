#ifndef RC522MODULE_H
#define RC522MODULE_H

#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

class RC522Module
{
public:
  // 构造函数（初始化引脚）
  RC522Module(uint8_t rstPin, uint8_t ssPin, uint8_t buzzerPin,
              uint8_t ledPin, uint8_t addBtnPin, uint8_t delBtnPin); // 修改构造函数
  // 初始化方法
  void begin();

  // 主循环更新方法（需在 Arduino loop() 中调用）
  void update();
  int8_t mqttflag = -1;

private:
  // RFID 相关对象与变量
  MFRC522 _mfrc522;
  uint8_t _rstPin, _ssPin;

  // 外设引脚
  uint8_t _buzzerPin, _ledPin, _addBtnPin, _delBtnPin; // 新增删除按钮引脚

  // 数据存储结构
  union LongByte
  {
    long longData;
    byte byteData[4];
  };
  LongByte _lb;

  // EEPROM 存储的卡号与当前读取的卡号
  long _eepromRFID;
  long _currentRFID;

  // 私有方法
  void _initPins();
  void _handleAddButton(); // 原保存按钮逻辑
  void _handleDelButton(); // 新增删除按钮逻辑
  void _readRFID();
  void _handleRFID(long data);
  void _buzz(int times);

  // 转换卡号字节为 long 类型
  long _bytesToLong(byte *bytes);
};

#endif
