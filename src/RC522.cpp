#include "RC522.h"

// 构造函数（初始化引脚配置）
RC522Module::RC522Module(uint8_t rstPin, uint8_t ssPin, uint8_t buzzerPin,
                         uint8_t ledPin, uint8_t addBtnPin, uint8_t delBtnPin)
    : _mfrc522(ssPin, rstPin), _rstPin(rstPin), _ssPin(ssPin),
      _buzzerPin(buzzerPin), _ledPin(ledPin),
      _addBtnPin(addBtnPin), _delBtnPin(delBtnPin),
      _eepromRFID(-1), _currentRFID(-1) {}

// 初始化硬件与数据
void RC522Module::begin()
{
  EEPROM.begin(1024);

  // 读取 EEPROM 中存储的卡号
  for (int i = 0; i < 4; i++)
  {
    _lb.byteData[i] = EEPROM.read(i);
  }
  _eepromRFID = _lb.longData;

  // 初始化引脚与 RFID 模块
  _initPins();
  SPI.begin();
  _mfrc522.PCD_Init();
  Serial.println("RFID Module Initialized");
}

// 主循环更新逻辑
void RC522Module::update()
{
  _handleAddButton();
  _handleDelButton(); // 处理删除按钮
  _readRFID();
}

// 初始化 GPIO 引脚
void RC522Module::_initPins()
{
  pinMode(_addBtnPin, INPUT);
  pinMode(_buzzerPin, OUTPUT);
  pinMode(_delBtnPin, INPUT_PULLUP); // 初始化删除按钮引脚
  digitalWrite(_buzzerPin, HIGH);
  pinMode(_ledPin, OUTPUT);
  digitalWrite(_ledPin, HIGH);
}

// 处理按钮事件
void RC522Module::_handleAddButton()
{
  if (digitalRead(_addBtnPin) == HIGH)
  {
    if (_currentRFID == -1)
    {
      Serial.println("No card detected!");
      _buzz(3);
    }
    else
    {
      // 保存当前卡号到 EEPROM
      _lb.longData = _currentRFID;
      _eepromRFID = _lb.longData;
      for (int i = 0; i < 4; i++)
      {
        EEPROM.write(i, _lb.byteData[i]);
      }
      EEPROM.commit();
      Serial.println("Card ID saved to EEPROM");
      _buzz(1);
    }
  }
}
// 新增删除按钮逻辑
void RC522Module::_handleDelButton()
{
  if (digitalRead(_delBtnPin) == LOW)
  {
    // 长鸣提示删除操作
    _buzz(3); // 1次，持续1秒

    // 清除EEPROM中的卡号
    _eepromRFID = -1;
    for (int i = 0; i < 4; i++)
    {
      EEPROM.write(i, 0xFF); // 写入-1的字节表示（0xFF填充）
    }
    EEPROM.commit();

    Serial.println("卡信息已删除");
    digitalWrite(_ledPin, HIGH); // 关闭LED
  }
}

// 读取 RFID 卡号
void RC522Module::_readRFID()
{
  if (!_mfrc522.PICC_IsNewCardPresent() || !_mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  _currentRFID = _bytesToLong(_mfrc522.uid.uidByte);
  _handleRFID(_currentRFID);
  _mfrc522.PICC_HaltA();
  _mfrc522.PCD_StopCrypto1();
}

// 处理 RFID 数据（验证逻辑）
void RC522Module::_handleRFID(long data)
{
  Serial.print("Detected Card ID: ");
  Serial.println(data);

  if (_eepromRFID == -1)
  {
    Serial.println("No master card registered!");
    _buzz(3);
  }
  else if (data == _eepromRFID)
  {

    Serial.println("Access Granted");
    _buzz(1);
    if (digitalRead(_ledPin) == HIGH)
      mqttflag = 1;
    else
      mqttflag = 0;
    digitalWrite(_ledPin, !digitalRead(_ledPin));
  }
  else
  {
    Serial.println("Access Denied");
    _buzz(2);
  }
}

// 蜂鸣器提示音
void RC522Module::_buzz(int times)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(_buzzerPin, LOW);
    delay(250);
    digitalWrite(_buzzerPin, HIGH);
    delay(50);
  }
}

// 转换字节数组为 long 类型
long RC522Module::_bytesToLong(byte *bytes)
{
  LongByte tmp;
  for (int i = 0; i < 4; i++)
  {
    tmp.byteData[i] = bytes[i];
  }
  return tmp.longData;
}
