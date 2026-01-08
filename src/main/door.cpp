#include <Arduino.h>
#include "network_manager.h"
#include "pir_sensor.h"
#include "rgbled.h"
#include "timerutil.h"
#include "DHT.h"

// #include "ws218.h"
// #include "gp2y1014au.h"
// #include "dht11.h"
// 配置参数
const char *SSID = "HUAWEI_CHUANGSHI";
const char *WIFI_PASS = "12345678";
const char *MQTT_SERVER = "znjj.piedaochuan.top";
const int MQTT_PORT = 1883;

// 硬件定义
#define PIR_PIN D3
#define DHT11_IN D2
#define DOOR_PIN1 D0
#define DOOR_PIN2 D1

NetworkManager network(SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT,
                       "door", "zhihuijia", "123456");
PIRSensor motionSensor(PIR_PIN);
RGBLed led(D5, D6, D7);
DHT dht(DHT11_IN, DHT11);

TimerUtil fadeTimer;
TimerUtil dhtTimer;
TimerUtil doorTimer;

bool doorFlag = false;
bool isDone = false;
// MQTT回调函数处理接收消息
void callback(char *topic, byte *payload, unsigned int length)
{
  static bool flag = false;
  String message;
  for (int i = 0; i < length; i++)
    message += (char)payload[i];

  Serial.print("主题 [");
  Serial.print(topic);
  Serial.print("] 收到消息: ");
  Serial.println(message);

  if (strcmp(topic, "smarthome/door") == 0)
  {
    // 控制窗户
    if (doorFlag)
      return; // 防止重复触发
    static bool status = false;
    if (!status)
    {
      digitalWrite(DOOR_PIN1, HIGH);
      digitalWrite(DOOR_PIN2, LOW);
      status = true;
    }
    else
    {
      digitalWrite(DOOR_PIN2, HIGH);
      digitalWrite(DOOR_PIN1, LOW);
      status = false;
    }
    doorTimer.start();
    doorFlag = true;
  }
}

// 中断服务函数（必须ICACHE_RAM_ATTR）
ICACHE_RAM_ATTR void handlePIR()
{
  if (millis() - motionSensor.lastTriggerTime > 200)
  {
    motionSensor.motionDetected = true;
    motionSensor.lastTriggerTime = millis();
    Serial.println("检测到运动");
  }
}
// 人体检测感应触发呼吸灯函数
void breathing_lamps()
{
  if (motionSensor.checkMotion() && !isDone)
  {
    fadeTimer.start();
    isDone = true;
  }
  else if (fadeTimer.isElapsed(10000))
  {
    // 没有检测到运动
    led.setColor(255, 255, 255);
    isDone = false;
  }
  else if (isDone)
  {
    led.fadeToNext(); // 触发渐变
    led.update();
  }
  else
  {
    led.setColor(255, 255, 255);
  }
}
void sensor_data_upload()
{

  if (dhtTimer.isElapsed(30000))
  {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    dhtTimer.start();

    if (String(humidity) == "nan" || temperature == -1)
    {
      Serial.println("DHT11读取失败！");
    }
    else
    {
      String payload = String(humidity) + "," + String(temperature);
      Serial.printf("发送数据: %s\n", payload);
      network.mqttClient.publish("smarthome/livingroom/humidity", (byte *)String(humidity).c_str(), String(humidity).length(), true);
      network.mqttClient.publish("smarthome/livingroom/temperature", (byte *)String(temperature).c_str(), String(humidity).length(), true);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  network.connectWiFi();
  network.connectMQTT();
  network.mqttClient.setCallback(callback);
  pinMode(DOOR_PIN1, OUTPUT);
  pinMode(DOOR_PIN2, OUTPUT);
  digitalWrite(DOOR_PIN1, LOW);
  digitalWrite(DOOR_PIN2, LOW);
  dht.begin();
  led.begin();
  motionSensor.setup();
  dhtTimer.start();
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), handlePIR, RISING); // 上升沿触发
}

void loop()
{
  network.maintainConnection();
  breathing_lamps();
  sensor_data_upload();
  if (doorFlag && doorTimer.isElapsed(500))
  {
    digitalWrite(DOOR_PIN1, LOW);
    digitalWrite(DOOR_PIN2, LOW);
    doorFlag = false;
  }
}
