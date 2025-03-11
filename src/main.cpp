#include <Arduino.h>
#include "MyWifi.h"
#include "MyAPI.h"
#include "MyMQTTManager.h"
#include <WiFi.h>

int timeout = 10000; // 10 secondes de timeout
MyWifi *mywifi;
MyAPI *myapi;
MyMQTTManager *mymqttmanager;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  mywifi = new MyWifi();
  myapi = new MyAPI();
  mymqttmanager = new MyMQTTManager();

  if (!mywifi)
  {
    Serial.println("Erreur d'initialisation de la classe MyWifi");
  }

  if (!myapi)
  {
    Serial.println("Erreur d'initialisation de la classe MyAPI");
  }

  if (!mymqttmanager)
  {
    Serial.println("Erreur d'initialisation de la classe MyMQTTManager");
  }
  
  if(!mywifi->connect())
  {
    Serial.println("Erreur de connexion WiFi");
  }
  else
  {
    Serial.println("Connexion WiFi établie!");
    Serial.print("Adresse IP: ");
    Serial.println(mywifi->getLocalIP());
  }

  if (!myapi->getBrokerInfo())
  {
    Serial.println("Erreur de récupération des informations du broker");
  }
  else
  {
    Serial.println("Informations du broker récupérées avec succès!");
  }

  if (!mymqttmanager->init())
  {
    Serial.println("Erreur d'initialisation du client MQTT");
  }
  else
  {
    mymqttmanager->connect();
  }


}

void loop()
{
  // put your main code here, to run repeatedly:
  mywifi->checkResetButton();
  mymqttmanager->publishtopic1("Hello World in topic 1 !");
  mymqttmanager->clientLoop();
  delay(500);
}