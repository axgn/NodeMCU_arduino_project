#include <SoftwareSerial.h>

class AS608
{
public:
  AS608(uint8_t rxPin, uint8_t txPin, uint8_t touchPin);
  bool begin();
  int identifyFinger();

private:
  SoftwareSerial fserial;
  const uint8_t touchPin;
  uint8_t lastError = 0;

  bool sendCmd(const uint8_t *cmd, int cmdLen, uint8_t *resp, int respLen);
};
