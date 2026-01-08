#include "network_manager.h"

NetworkManager::NetworkManager(const char *ssid, const char *wifiPass,
                               const char *mqttServer, int mqttPort,
                               const char *clientId, const char *mqttUser, const char *mqttPass)
    : ssid(ssid), wifiPass(wifiPass),
      mqttServer(mqttServer), mqttPort(mqttPort),
      clientId(clientId), mqttUser(mqttUser), mqttPass(mqttPass),
      mqttClient(espClient) {}

void NetworkManager::connectWiFi()
{
    WiFi.begin(ssid, wifiPass);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Connecting to WiFi.......");
        delay(3000);
    }
}

void NetworkManager::connectMQTT()
{
    char *topics[] = {
        // "smarthome/door"
        "smarthome/livingroom/led",
        "smarthome/livingroom/ledcolor",
        "smarthome/livingroom/ledmode",
        "smarthome/livingroom/ledbrightness",
        "smarthome/livingroom/fan",
        "smarthome/livingroom/curtain",
        "smarthome/livingroom/airconditioner",
        // "smarthome/bedroom/led",
        // "smarthome/bedroom/ledcolor",
        // "smarthome/bedroom/ledmode",
        // "smarthome/bedroom/ledbrightness",
        // "smarthome/bedroom/fan",
        // "smarthome/bedroom/humidifier",
    };
    mqttClient.setServer(mqttServer, mqttPort);
    if (mqttClient.connect(clientId, mqttUser, mqttPass))
    {
        Serial.println("MQTT connected");
        for (int i = 0; i < sizeof(topics) / sizeof(topics[0]); i++)
        {
            mqttClient.subscribe(topics[i], 1); // PubSubClient仅支持QoS1订阅
        }
    }
}

void NetworkManager::maintainConnection()
{
    if (!mqttClient.connected())
    {
        Serial.println("MQTT is not connected");
        connectMQTT();
    }
    mqttClient.loop();
}
