#include <Arduino.h>
#include "network_manager.h"
#include "motor_controller.h"
#include "timerutil.h"
#include "BreathingLed.h"

// 配置参数
const char *SSID = "HUAWEI_CHUANGSHI";
const char *WIFI_PASS = "12345678";
const char *MQTT_SERVER = "znjj.piedaochuan.top";
const int MQTT_PORT = 1883;

// 硬件定义
#define AIRCON_PIN D7
#define CURTAIN_PIN1 D5 // 打开
#define CURTAIN_PIN2 D6 // 关闭
#define MOTOR_IN1 D2
#define MOTOR_IN2 D1

NetworkManager network(SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT,
                       "livingroom", "zhihuijia", "123456");
MotorController fan(MOTOR_IN1, MOTOR_IN2, 1023); // 使用10位PWM
BreathingLed ledStrip(70);

TimerUtil curtainTimer;
int lastMs = 0;

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
  if (strcmp(topic, "smarthome/livingroom/ledcolor") == 0)
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
  else if (strcmp(topic, "smarthome/livingroom/led") == 0)
  {
    if (message == "0")
      ledStrip.beginTurnOff();
    else if (message == "1")
      ledStrip.turnOn();
  }
  else if (strcmp(topic, "smarthome/livingroom/ledmode") == 0)
  {
    if (message == "0")
      ledStrip.setMode(BreathingLed::MODE_CONSTANT);
    else if (message == "1")
      ledStrip.setMode(BreathingLed::MODE_BREATHING);
  }
  else if (strcmp(topic, "smarthome/livingroom/ledbrightness") == 0)
  {
    uint8_t brightness = message.toInt();
    ledStrip.setGlobalBrightness(constrain(brightness, 1, 255));
  }
  else if (strcmp(topic, "smarthome/livingroom/airconditioner") == 0)
  {
    if (message == "0")
      digitalWrite(AIRCON_PIN, LOW);
    else if (message == "1")
      digitalWrite(AIRCON_PIN, HIGH);
  }
  else if (strcmp(topic, "smarthome/livingroom/fan") == 0)
  {
    // 控制风扇
    if (message == "0")
      fan.setSpeed(0);
    else if (message == "1")
      fan.setSpeed(500);
    else if (message == "2")
      fan.setSpeed(700);
    else if (message == "3")
      fan.setSpeed(900);
  }
  else if (strcmp(topic, "smarthome/livingroom/curtain") == 0)
  {
    static int sum = 0;
    lastMs = message.toInt();
    if (lastMs + sum > 5000)
    {
      lastMs = 5000 - sum;
      sum = 5000;
    }
    else if (lastMs + sum < 0)
    {
      lastMs = 0 - sum;
      sum = 0;
    }
    else
      sum += lastMs;
    curtainTimer.start();
    if (lastMs > 0)
      digitalWrite(CURTAIN_PIN1, HIGH), digitalWrite(CURTAIN_PIN2, LOW);
    else
      digitalWrite(CURTAIN_PIN2, HIGH), digitalWrite(CURTAIN_PIN1, LOW);
  }
}

void setup()
{
  Serial.begin(115200);
  network.connectWiFi();
  network.connectMQTT();
  network.mqttClient.setCallback(callback);

  pinMode(CURTAIN_PIN1, OUTPUT);
  pinMode(CURTAIN_PIN2, OUTPUT);
  digitalWrite(CURTAIN_PIN1, LOW); // 初始化为高电平（继电器断开）
  digitalWrite(CURTAIN_PIN2, LOW); // 初始化为高电平（继电器断开）

  pinMode(AIRCON_PIN, OUTPUT);
  digitalWrite(AIRCON_PIN, LOW); // 初始化为高电平（继电器断开）
  fan.setup();
  ledStrip.begin();
}

void loop()
{
  network.maintainConnection();
  ledStrip.update();
  if (abs(lastMs) > 0 && curtainTimer.isElapsed(abs(lastMs)))
  {
    digitalWrite(CURTAIN_PIN1, LOW);
    digitalWrite(CURTAIN_PIN2, LOW);
    lastMs = 0;
  }
}
