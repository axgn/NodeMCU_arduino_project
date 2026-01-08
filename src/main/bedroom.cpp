#include <Arduino.h>
#include "network_manager.h"
#include "motor_controller.h"
#include "timerutil.h"
#include <GP2YDustSensor.h>
#include "BreathingLed.h"
#include "servo_controller.h"

// #include "ws218.h"
// #include "gp2y1014au.h"
// #include "dht11.h"
// 配置参数
const char *SSID = "HUAWEI_CHUANGSHI";
const char *WIFI_PASS = "12345678";
const char *MQTT_SERVER = "znjj.piedaochuan.top";
const int MQTT_PORT = 1883;

// 硬件定义
#define MOTOR_IN1 D2
#define MOTOR_IN2 D3
#define SENSOR_PIN A0 // 传感器连接NodeMCU的A0引脚（ADC输入）
#define HUMIDIFIER_PIN D6
#define SERVO_PIN D7

NetworkManager network(SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT,
                       "bedroom", "zhihuijia", "123456");
MotorController fan(MOTOR_IN1, MOTOR_IN2, 1023); // 使用10位PWM
GP2YDustSensor dustSensor(GP2YDustSensorType::GP2Y1010AU0F, D0, SENSOR_PIN, 12);
BreathingLed ledStrip(13);
ServoController windowServo(SERVO_PIN);

TimerUtil dustTimer;

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
  if (strcmp(topic, "smarthome/bedroom/ledcolor") == 0)
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
  else if (strcmp(topic, "smarthome/bedroom/led") == 0)
  {
    if (message == "0")
      ledStrip.beginTurnOff();
    else if (message == "1")
      ledStrip.turnOn();
  }
  else if (strcmp(topic, "smarthome/bedroom/ledmode") == 0)
  {
    if (message == "0")
      ledStrip.setMode(BreathingLed::MODE_CONSTANT);
    else if (message == "1")
      ledStrip.setMode(BreathingLed::MODE_BREATHING);
  }
  else if (strcmp(topic, "smarthome/bedroom/ledbrightness") == 0)
  {
    uint8_t brightness = message.toInt();
    ledStrip.setGlobalBrightness(constrain(brightness, 1, 255));
  }
  else if (strcmp(topic, "smarthome/bedroom/humidifier") == 0)
  {
    // 控制加湿器逻辑
    if (message == "0")
      digitalWrite(HUMIDIFIER_PIN, LOW);
    else if (message == "1")
      digitalWrite(HUMIDIFIER_PIN, HIGH);
  }
  else if (strcmp(topic, "smarthome/bedroom/fan") == 0)
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
  else if (strcmp(topic, "smarthome/bedroom/windows") == 0)
  {
    // 控制窗户
    windowServo.setAngle(message.toInt());
  }
}

void sensor_data_upload()
{

  if (dustTimer.isElapsed(5000))
  {
    static uint64_t lastDustTime = 6000 * 10;
    uint16 dustDensity = dustSensor.getDustDensity();
    Serial.printf("Density: %d ug/m3\n", dustDensity);
    network.mqttClient.publish("smarthome/airindex", (byte *)String(dustDensity).c_str(), String(dustDensity).length(), true);
    dustTimer.start();
    if (dustTimer.isTimeReached(lastDustTime))
    {
      lastDustTime += lastDustTime;
      uint16 dustAverage = dustSensor.getRunningAverage();
      float newBaseline = dustSensor.getBaselineCandidate();
      dustSensor.setBaseline(newBaseline);
      Serial.printf("1m Avg Dust Density: %d ug/m3; New baseline: %.4f\n", dustAverage, newBaseline);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  network.connectWiFi();
  network.connectMQTT();
  network.mqttClient.setCallback(callback);

  pinMode(HUMIDIFIER_PIN, OUTPUT);
  digitalWrite(HUMIDIFIER_PIN, LOW);
  windowServo.setup();
  fan.setup();
  ledStrip.begin();
  dustSensor.begin();
  dustSensor.setBaseline(0);
  dustTimer.start();
}

void loop()
{
  network.maintainConnection();
  sensor_data_upload();
  ledStrip.update();
}
