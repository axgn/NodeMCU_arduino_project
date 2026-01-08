#include <Arduino.h>
#include "network_manager.h"
#include "BreathingLed.h"

// #include "ws218.h"
// #include "gp2y1014au.h"
// #include "dht11.h"
// 配置参数
const char *SSID = "HUAWEI_CHUANGSHI";
const char *WIFI_PASS = "12345678";
const char *MQTT_SERVER = "znjj.piedaochuan.top";
const int MQTT_PORT = 1883;

// 硬件定义

#define FAN D2

NetworkManager network(SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT,
                       "bathroom", "zhihuijia", "123456");
BreathingLed ledStrip(10);

bool isDone = false;
// MQTT回调函数处理接收消息
void callback(char *topic, byte *payload, unsigned int length)
{
  String message;
  for (int i = 0; i < length; i++)
    message += (char)payload[i];

  Serial.print("主题 [");
  Serial.print(topic);
  Serial.print("] 收到消息: ");
  Serial.println(message);

  // 根据主题执行操作（示例）
  if (strcmp(topic, "smarthome/bathroom/ledcolor") == 0)
  {
    // 控制LED逻辑
    char colorStr[length + 1];
    memcpy(colorStr, payload, length);
    colorStr[length] = '\0';

    uint8_t r, g, b;
    if (sscanf(colorStr, "%hhu,%hhu,%hhu", &r, &g, &b) == 3)
    {
      ledStrip.setColor(CRGB(r, g, b));
    }
  }
  else if (strcmp(topic, "smarthome/bathroom/led") == 0)
  {
    if (message == "0")
      ledStrip.beginTurnOff();
    else if (message == "1")
      ledStrip.turnOn();
  }
  else if (strcmp(topic, "smarthome/bathroom/ledmode") == 0)
  {
    if (message == "0")
      ledStrip.setMode(BreathingLed::MODE_CONSTANT);
    else if (message == "1")
      ledStrip.setMode(BreathingLed::MODE_BREATHING);
  }
  else if (strcmp(topic, "smarthome/bathroom/ledbrightness") == 0)
  {
    uint8_t brightness = message.toInt();
    ledStrip.setGlobalBrightness(constrain(brightness, 1, 255));
  }
  else if (strcmp(topic, "smarthome/bathroom/fan") == 0)
  {
    // 控制加湿器逻辑
    if (message == "0")
      digitalWrite(FAN, LOW);
    else if (message == "1")
      digitalWrite(FAN, HIGH);
  }
}

void setup()
{
  Serial.begin(115200);
  network.connectWiFi();
  network.connectMQTT();
  network.mqttClient.setCallback(callback);

  ledStrip.begin();
  digitalWrite(FAN, LOW);
  pinMode(FAN, OUTPUT);
}

void loop()
{
  network.maintainConnection();
  ledStrip.update();
}
