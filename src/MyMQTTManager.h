#ifndef MYMQTTMANAGER_H
#define MYMQTTMANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "MyAPI.h"
#include "MyWifi.h"
#include <Preferences.h>

class MyMQTTManager
{
public:
    MyMQTTManager();
    bool init();
    void connect();
    bool tryConnect();
    void reset();
    void saveBrokerInfo();
    bool loadBrokerInfo();

    void publishtopic1(const char *message);
    void publishtopic2(const char *message);
    void publishTempStatus(const char *message);
    void publishOledStatus(const char *message);
    void publishRfidStatus(const char *message);
    void publishUltrasonicStatus(const char *message);
    void publishTempError(float number);
    void publishOledError(float number);
    void publishRfidError(float number);
    void publishUltrasonicError(float number);

    void subscribetopic1();
    void subscribetopic2();
    void subscribetempStatusTopic();
    void subscribeoledStatusTopic();
    void subscriberfidStatusTopic();
    void subscribeultrasonicStatusTopic();
    void subscribetempErrorTopic();
    void subscribeoledErrorTopic();
    void subscriberfidErrorTopic();
    void subscribeultrasonicErrorTopic();

    void clientLoop();
    void setCallback();
    static void mqttCallbackHandler(char *topic, byte *payload, unsigned int length);
    static MyMQTTManager* instance;
    String floatToString(float value, int precision = 1);
    PubSubClient client;

private:
    String server;
    String port;
    String user;
    String password;

    const char *topic1 = "casier1/temperature";
    const char *topic2 = "Skibidy/Desiree";
    const char *tempStatusTopic = "casier1/temperature/status";
    const char *oledStatusTopic = "casier1/oled/status";
    const char *rfidStatusTopic = "casier1/rfid/status";
    const char *ultrasonicStatusTopic = "casier1/ultrasonic/status";
    const char *tempErrorTopic = "casier1/temperature/error";
    const char *oledErrorTopic = "casier1/oled/error";
    const char *rfidErrorTopic = "casier1/rfid/error";
    const char *ultrasonicErrorTopic = "casier1/ultrasonic/error";

    WiFiClient espClient;
    MyAPI api;
    MyWifi myWifi;
    Preferences preferences;
    String messageFirstTopic;
    String messageSecondTopic;
    String messagetempStatusTopic;
    String messageoledStatusTopic;
    String messagerfidStatusTopic;
    String messageultrasonicStatusTopic;
    String messagetempErrorTopic;
    String messageoledErrorTopic;
    String messagerfidErrorTopic;
    String messageultrasonicErrorTopic;
};

#endif