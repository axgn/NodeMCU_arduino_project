#include <Arduino.h>
#include "network_manager.h"
#include "timerutil.h"
#include <SoftwareSerial.h>
#include "AS608.h"
#include "lightsensor.h"

// 配置参数
const char* SSID = "HUAWEI_CHUANGSHI";
const char* WIFI_PASS = "12345678";
const char* MQTT_SERVER = "znjj.piedaochuan.top";
const int MQTT_PORT = 1883;

// 硬件定义
#define FINGER_RX D7
#define FINGER_TX D8
#define TOUCH_PIN D0


NetworkManager network(SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT, 
                      "fingerDevice", "zhihuijia", "123456");
AS608 finger(FINGER_RX, FINGER_TX, TOUCH_PIN);
LightSensor lightSensor(17, 50, 950, 5); 


TimerUtil fingerTimer;
TimerUtil LightTimer;


bool isDone = false;


void verifyFingerprint() {
  if(!fingerTimer.isElapsed(500))  return;
  fingerTimer.start();
  int result = finger.identifyFinger();
  
  if(result >= 0 ) {
    Serial.print("识别成功！ID:");
    Serial.println(result);
    network.mqttClient.publish("smarthome/door", (byte*)String("1").c_str(), String("1").length(), false);  
  } else if(result == -1);
   else {
    Serial.print("识别错误：");
    Serial.println(result);
  }  
}

void sensor_data_upload() {
  if(LightTimer.isElapsed(1000)) {
    int raw = lightSensor.readRaw();
    int percent = lightSensor.readPercent();
    Serial.printf("Raw: %d | Filtered: %d%%\n", raw, percent);
    LightTimer.start();
    network.mqttClient.publish("smarthome/livingroom/light", (byte*)String(percent).c_str(), String(percent).length(), true);  
  }
}

void setup() {
    Serial.begin(115200);
    network.connectWiFi();
    network.connectMQTT();


    finger.begin();
    lightSensor.begin();

    LightTimer.start();
    fingerTimer.start();
}


void loop() {
    network.maintainConnection();
    verifyFingerprint();
    sensor_data_upload();
}
