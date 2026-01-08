#include <Arduino.h>
#include <MQUnifiedsensor.h>
#include "network_manager.h"
#include "timerutil.h"
#include "RC522.h"

// #include "ws218.h"
// #include "gp2y1014au.h"
// #include "dht11.h"
// 配置参数
const char* SSID = "HUAWEI_CHUANGSHI";
const char* WIFI_PASS = "12345678";
const char* MQTT_SERVER = "znjj.piedaochuan.top";
const int MQTT_PORT = 1883;

// 硬件定义
#define RST_PIN   D1
#define SS_PIN    D2
#define BUZZER_PIN D0
#define LED_PIN   D4
#define BTN_ADD_PIN   D8
#define BTN_DEL_PIN   D9

RC522Module rfidModule(RST_PIN, SS_PIN, BUZZER_PIN, LED_PIN, BTN_ADD_PIN, BTN_DEL_PIN);
NetworkManager network(SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT, 
                      "IDCardDevice", "zhihuijia", "123456");


bool isDone = false;

void handelControl() {
  if (rfidModule.mqttflag==-1)return;
  else if (rfidModule.mqttflag==1) {
    network.mqttClient.publish("smarthome/door", (byte*)String("1").c_str(), String("1").length(), false);  
    rfidModule.mqttflag=-1;
  }
  else if (rfidModule.mqttflag==0){
    network.mqttClient.publish("smarthome/door", (byte*)String("0").c_str(), String("0").length(), false);  
    rfidModule.mqttflag=-1;
  }
}


void setup() {
    Serial.begin(115200);
    network.connectWiFi();
    network.connectMQTT();

    rfidModule.begin();    
}


void loop() {
    network.maintainConnection();
    rfidModule.update();
    handelControl();
  }
