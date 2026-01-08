#include <Arduino.h>
#include <MQUnifiedsensor.h>
#include "network_manager.h"
#include "timerutil.h"
#include <SoftwareSerial.h>
#include "BreathingLed.h"

// 配置参数
const char *SSID = "HUAWEI_CHUANGSHI";
const char *WIFI_PASS = "12345678";
const char *MQTT_SERVER = "znjj.piedaochuan.top";
const int MQTT_PORT = 1883;

// 硬件定义

#define FAN D2
#define MQ2_IN A0

MQUnifiedsensor MQ2("NodeMCU", 5, 10, MQ2_IN, "MQ-2");
NetworkManager network(SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT,
                       "kitchen", "zhihuijia", "123456");
BreathingLed ledStrip(13);

TimerUtil MQ2Timer;

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
  if (strcmp(topic, "smarthome/kitchen/ledcolor") == 0)
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
  else if (strcmp(topic, "smarthome/kitchen/led") == 0)
  {
    if (message == "0")
      ledStrip.beginTurnOff();
    else if (message == "1")
      ledStrip.turnOn();
  }
  else if (strcmp(topic, "smarthome/kitchen/ledmode") == 0)
  {
    if (message == "0")
      ledStrip.setMode(BreathingLed::MODE_CONSTANT);
    else if (message == "1")
      ledStrip.setMode(BreathingLed::MODE_BREATHING);
  }
  else if (strcmp(topic, "smarthome/kitchen/ledbrightness") == 0)
  {
    uint8_t brightness = message.toInt();
    ledStrip.setGlobalBrightness(constrain(brightness, 1, 255));
  }
  else if (strcmp(topic, "smarthome/kitchen/fan") == 0)
  {
    if (message == "0")
      digitalWrite(FAN, LOW);
    else if (message == "1")
      digitalWrite(FAN, HIGH);
  }
}

void sensor_data_upload()
{
  if (MQ2Timer.isElapsed(5000))
  {
    MQ2.update();                 // Update data, the arduino will read the voltage from the analog pin
    float ppm = MQ2.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
    // MQ2.serialDebug(); // Will print the table on the serial port
    MQ2Timer.start();

    char ppm_str[10];            // 预留足够空间（如 "1234.5678" 占9字节）
    dtostrf(ppm, 7, 4, ppm_str); // 格式：总长度7，保留4位小数
    network.mqttClient.publish("smarthome/kitchen/mq2", (byte *)ppm_str, strlen(ppm_str), true);
  }
}

void MQ2Ibegin()
{
  MQ2.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ2.setA(574.25);
  MQ2.setB(-2.222); // Configure the equation to to calculate LPG concentration
  MQ2.init();

  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++)
  {
    MQ2.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ2.calibrate(9.83);
    Serial.print(".");
  }
  MQ2.setR0(calcR0 / 10);
  Serial.println("  done!.");

  if (isinf(calcR0))
  {
    Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1)
      ;
  }
  if (calcR0 == 0)
  {
    Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    while (1)
      ;
  }
}

void setup()
{
  Serial.begin(115200);
  network.connectWiFi();
  network.connectMQTT();
  network.mqttClient.setCallback(callback);

  pinMode(FAN, OUTPUT);
  digitalWrite(FAN, LOW);
  ledStrip.begin();
  MQ2Ibegin();
  MQ2Timer.start();
}

void loop()
{
  network.maintainConnection();
  sensor_data_upload();
  ledStrip.update();
}
