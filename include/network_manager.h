#pragma once
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class NetworkManager
{
public:
    const char *ssid;
    const char *wifiPass;
    const char *mqttServer;
    int mqttPort;
    const char *clientId;
    const char *mqttUser;
    const char *mqttPass;

    WiFiClient espClient;
    PubSubClient mqttClient;

public:
    NetworkManager(const char *ssid, const char *wifiPass,
                   const char *mqttServer, int mqttPort,
                   const char *clientId, const char *mqttUser, const char *mqttPass);

    void connectWiFi();
    void connectMQTT();
    void maintainConnection();
};
