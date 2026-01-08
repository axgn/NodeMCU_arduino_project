#include <Arduino.h>
#include <MQUnifiedsensor.h>
#include "network_manager.h"
#include "servo_controller.h"
#include "pir_sensor.h"
#include "rgbled.h"
#include "motor_controller.h"
#include "timerutil.h"
#include "DHT.h"
#include <GP2YDustSensor.h>
#include <SoftwareSerial.h>
#include "AS608.h"
#include "BreathingLed.h"
#include "lightsensor.h"
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
#define SERVO_PIN D4
#define PIR_PIN D7
#define MOTOR_IN1 D3
#define MOTOR_IN2 D1
#define DHT11_IN D2
#define SENSOR_PIN A0   // 传感器连接NodeMCU的A0引脚（ADC输入）
#define FINGER_RX D7
#define FINGER_TX D8
#define TOUCH_PIN D0
#define MQ2_IN A0
#define RST_PIN   D1
#define SS_PIN    D2
#define BUZZER_PIN D0
#define LED_PIN   D4
#define BTN_ADD_PIN   D8
#define BTN_DEL_PIN   D9

RC522Module rfidModule(RST_PIN, SS_PIN, BUZZER_PIN, LED_PIN, BTN_ADD_PIN, BTN_DEL_PIN);
MQUnifiedsensor MQ2("NodeMCU",5, 10, MQ2_IN, "MQ-2");
NetworkManager network(SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT, 
                      "livingroom", "zhihuijia", "123456");
ServoController windowServo(SERVO_PIN);
PIRSensor motionSensor(PIR_PIN);
RGBLed led(D5, D6, D4);
MotorController fan(MOTOR_IN1, MOTOR_IN2, 1023); // 使用10位PWM
DHT dht(DHT11_IN, DHT11);
GP2YDustSensor dustSensor(GP2YDustSensorType::GP2Y1010AU0F, D0, SENSOR_PIN,12);
AS608 finger(FINGER_RX, FINGER_TX, TOUCH_PIN);
BreathingLed ledStrip(D1, 120);
LightSensor lightSensor(17, 50, 950, 5); 


TimerUtil fadeTimer;
TimerUtil dhtTimer;
TimerUtil dustTimer;
TimerUtil fingerTimer;
TimerUtil LightTimer;
TimerUtil MQ2Timer;
TimerUtil fastledTimer;

bool isDone = false;
// MQTT回调函数处理接收消息
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i=0; i<length; i++) message += (char)payload[i];
  
  Serial.print("主题 [");
  Serial.print(topic);
  Serial.print("] 收到消息: ");
  Serial.println(message);

  // 根据主题执行操作（示例）
  if (strcmp(topic, "smarthome/livingroom/ledcolor") == 0) {
    // 控制LED逻辑
    char colorStr[length + 1];
    memcpy(colorStr, payload, length);
    colorStr[length] = '\0';
    
    uint8_t r, g, b;
    if (sscanf(colorStr, "%hhu,%hhu,%hhu", &r, &g, &b) == 3) {
        ledStrip.setColor(CRGB(r, g, b));
    }
  } 
  else if (strcmp(topic, "smarthome/livingroom/led") == 0) {
    if(message == "0")       ledStrip.beginTurnOff()      ;
    else if (message == "1")  ledStrip.turnOn()           ;  
  }
  else if (strcmp(topic, "smarthome/livingroom/ledmode") == 0) {
    if(message == "0")       ledStrip.setMode(BreathingLed::MODE_CONSTANT)     ;
    else if (message == "1")  ledStrip.setMode(BreathingLed::MODE_BREATHING)       ;  
  }
  else if (strcmp(topic, "smarthome/livingroom/ledbrightness") == 0) {
    uint8_t brightness = message.toInt();
    ledStrip.setGlobalBrightness(constrain(brightness, 1, 255));
  }
  else if (strcmp(topic, "smarthome/livingroom/humidifier") == 0) {
    // 控制加湿器逻辑
    if(message == "0")          ;
    else if (message == "1")    ;
  }
  else if (strcmp(topic, "smarthome/livingroom/fan") == 0) {
    // 控制风扇
    if(message == "0")               fan.setSpeed(0);
    else if (message == "1")     fan.setSpeed(80);
    else if (message == "2")     fan.setSpeed(150);
    else if (message == "3")     fan.setSpeed(200);

  }
  else if (strcmp(topic, "smarthome/livingroom/windows") == 0) {
    // 控制窗户
    windowServo.setAngle(message.toInt());
  }
}
// 中断服务函数（必须ICACHE_RAM_ATTR）
IRAM_ATTR void handlePIR() {
  if (millis() - motionSensor.lastTriggerTime > 200) {
    motionSensor.motionDetected = true;
    motionSensor.lastTriggerTime = millis();
    Serial.println("检测到运动");
  }
}
// 人体检测感应触发呼吸灯函数
void breathing_lamps() {
      if (motionSensor.checkMotion()&&!isDone) {
        fadeTimer.start();
        isDone=true;
    }
    else if(fadeTimer.isElapsed(10000)) {
        // 没有检测到运动
        led.setColor(255, 255, 255);
        isDone=false;
    }
    else if (isDone)
    {
      led.fadeToNext();   // 触发渐变
      led.update();          
    }
    else
    {
      led.setColor(255, 255, 255);
    }
}
void sensor_data_upload() {
  if(MQ2Timer.isElapsed(5000)) {
    MQ2.update(); // Update data, the arduino will read the voltage from the analog pin
    float ppm = MQ2.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
    // MQ2.serialDebug(); // Will print the table on the serial port
    MQ2Timer.start();
    
    char ppm_str[10]; // 预留足够空间（如 "1234.5678" 占9字节）
    dtostrf(ppm, 7, 4, ppm_str); // 格式：总长度7，保留4位小数
    network.mqttClient.publish("smarthome/kitchen/mq2", (byte*)ppm_str,strlen(ppm_str), true);
  }

  if(LightTimer.isElapsed(1000)) {
    int raw = lightSensor.readRaw();
    int percent = lightSensor.readPercent();
    Serial.printf("Raw: %d | Filtered: %d%%\n", raw, percent);
    LightTimer.start();
    network.mqttClient.publish("smarthome/livingroom/light", (byte*)String(percent).c_str(), String(percent).length(), true);  
  }

  if (dhtTimer.isElapsed(5000)) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    dhtTimer.start();

    if (String(humidity) == "nan" || temperature == -1) {
      Serial.println("DHT11读取失败！");
    }
    else {
      String payload = String(humidity) + "," + String(temperature);    
      Serial.printf("发送数据: %s\n",payload);
      network.mqttClient.publish("smarthome/livingroom/humidity", (byte*)String(humidity).c_str(), String(humidity).length(), true);  
      network.mqttClient.publish("smarthome/livingroom/temperature", (byte*)String(temperature).c_str(), String(humidity).length(), true);  
      
    }
  }
  if (dustTimer.isElapsed(5000)) {
    static uint64_t lastDustTime = 6000*10;
    uint16 dustDensity = dustSensor.getDustDensity();
    Serial.printf("Density: %d ug/m3\n", dustDensity);
    network.mqttClient.publish("smarthome/airindex", (byte*)String(dustDensity).c_str(), String(dustDensity).length(), true);  
    dustTimer.start();
    if(dustTimer.isTimeReached(lastDustTime)) {
      lastDustTime +=  lastDustTime;
      uint16 dustAverage = dustSensor.getRunningAverage();
      float newBaseline = dustSensor.getBaselineCandidate();
      dustSensor.setBaseline(newBaseline);
      Serial.printf("1m Avg Dust Density: %d ug/m3; New baseline: %.4f\n", dustAverage, newBaseline);
    }    
  }



  
}

void verifyFingerprint() {
  if(!fingerTimer.isElapsed(500))  return;
  fingerTimer.start();
  int result = finger.identifyFinger();
  
  if(result >= 0 ) {
    Serial.print("识别成功！ID:");
    Serial.println(result);
    network.mqttClient.publish("smarthome/livingroom/door", (byte*)String("1").c_str(), String("1").length(), false);  
  } else if(result == -1);
   else {
    Serial.print("识别错误：");
    Serial.println(result);
  }  
}
void MQ2Ibegin()
{
  MQ2.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ2.setA(574.25); MQ2.setB(-2.222); // Configure the equation to to calculate LPG concentration
  MQ2.init(); 

  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ2.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ2.calibrate(9.83);
    Serial.print(".");
  }
  MQ2.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
}


void handelControl() {
  if (rfidModule.mqttflag==-1)return;
  else if (rfidModule.mqttflag==1) {
    network.mqttClient.publish("smarthome/livingroom/door", (byte*)String("1").c_str(), String("1").length(), false);  
    rfidModule.mqttflag=-1;
  }
  else if (rfidModule.mqttflag==0){
    network.mqttClient.publish("smarthome/livingroom/door", (byte*)String("0").c_str(), String("0").length(), false);  
    rfidModule.mqttflag=-1;
  }
}
void setup() {
    Serial.begin(115200);
    network.connectWiFi();
    network.connectMQTT();
    network.mqttClient.setCallback(callback);

    // windowServo.setup();
    // fan.setup();
    // motionSensor.setup();
    // dustSensor.begin();
    // dustSensor.begin();
    // dht.begin();
    // finger.begin();
    // ledStrip.begin();
    // lightSensor.begin();
    rfidModule.begin();


    // led.begin();
    // MQ2Ibegin();
    // fastledbegin();
    // blurledbegin();

    // attachInterrupt(digitalPinToInterrupt(PIR_PIN), handlePIR, RISING);  // 上升沿触发

    // dustSensor.setBaseline(0);

    // MQ2Timer.start();
    // LightTimer.start();
    // fingerTimer.start();
    // dhtTimer.start();
    // dustTimer.start();

    
}


void loop() {
    network.maintainConnection();
    // breathing_lamps();    
    // sensor_data_upload();
    // verifyFingerprint();
    // blurledrun();
    // ledStrip.update();    
    rfidModule.update();
  }
