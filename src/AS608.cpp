#include "AS608.h"

// 协议常量
const uint16_t AS608_HEADER = 0xEF01;
const uint32_t DEFAULT_ADDRESS = 0xFFFFFFFF;
const uint8_t CMD_CAPTURE = 0x01;
const uint8_t CMD_GEN_CHAR = 0x02;
const uint8_t CMD_SEARCH = 0x04;

AS608::AS608(uint8_t rxPin, uint8_t txPin, uint8_t touchPin)
    : fserial(rxPin, txPin), touchPin(touchPin) {}

bool AS608::begin()
{
  pinMode(touchPin, INPUT);
  fserial.begin(115200);
  return true;
}
// 发送指令并接收响应
bool AS608::sendCmd(const uint8_t *cmd, int cmdLen, uint8_t *resp, int respLen)
{
  // 发送指令
  fserial.write(cmd, cmdLen);

  // 接收响应
  unsigned long start = millis();
  int index = 0;
  while (millis() - start < 1000)
  {
    if (fserial.available())
    {
      resp[index++] = fserial.read();
      // Serial.print(resp[index-1], HEX);
      if (index == respLen)
      {
        return true;
      }
    }
  }

  return false;
}

// 指纹识别主函数
int AS608::identifyFinger()
{
  // 检查是否有手指按下
  if (digitalRead(touchPin) != HIGH)
    return -1;

  // 1. 采集图像
  // 发送命令，采集指纹图像
  uint8_t captureCmd[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x01, 0x00, 0x05};
  uint8_t captureResp[12] = {};
  if (!sendCmd(captureCmd, 12, captureResp, 12) || captureResp[9] != 0x00)
    return -2;

  // 2. 生成特征
  uint8_t genCharCmd[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x04, 0x02, 0x02, 0x00, 0x09};
  // 发送命令，生成指纹特征
  uint8_t genCharResp[12];
  if (!sendCmd(genCharCmd, 13, genCharResp, 12) || genCharResp[9] != 0x00)
    return -3;
  // 发送命令并接收响应

  // 3. 搜索指纹
  uint8_t searchCmd[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x04, 0x02, 0x00, 0x00, 0x01, 0x2C, 0x00, 0x3C};
  // 发送命令，搜索指纹
  uint8_t searchResp[16];
  if (!sendCmd(searchCmd, 17, searchResp, 16))
    return -4;
  // 发送命令并接收响应

  // 解析结果
  if (searchResp[9] == 0x00)
  {                                                // 确认码在索引9位置
    return (searchResp[10] << 8) | searchResp[11]; // ID在10-11字节
  }
  return -5;
}
