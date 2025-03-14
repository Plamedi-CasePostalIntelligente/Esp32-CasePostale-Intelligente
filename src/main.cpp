/*
 * Fichier: main.cpp
 * Créé le: 2024-03-01
 * Mis à jour le: 2024-05-01
 * Auteurs: Plamedi Ilunga
 * Contact: 2038993@cegeprdl.ca 
 * Version: 2.0
 * Description: Ce fichier implémente les méthodes de toutes mes classes 
 * Licence: Arduino
 */
#include <Arduino.h>
#include "MyWifi.h"
#include "MyAPI.h"
#include "MyMQTTManager.h"
#include "MyUltrasonique.h"
#include <WiFi.h>

int timeout = 10000; // 10 secondes de timeout
MyWifi *mywifi;
MyAPI *myapi;
MyMQTTManager *mymqttmanager;
MyUltrasonique *myUltrasonique;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  mywifi = new MyWifi();
  myapi = new MyAPI();
  mymqttmanager = new MyMQTTManager();
  myUltrasonique = new MyUltrasonique();


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

  if (!myUltrasonique)
  {
    Serial.println("Erreur d'initialisation de la classe MyUltrasonique");
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

  if(!myUltrasonique->FindEmptyBoxDistance())
  {
    Serial.println("Erreur de récupération de la distance de la boîte vide");
  }
  else
  {
    Serial.println("Distance de la boîte vide récupérée avec succès!");
  }


}

void loop()
{
  // put your main code here, to run repeatedly:
  mywifi->checkResetButton();
  myUltrasonique->GetDistance();

  if (myUltrasonique->IsBoxEmpty())
  {
    //Serial.println("La boîte est vide");
    mymqttmanager->publishtopic1("Vide");
  }
  else
  {
    //Serial.println("La boîte est pleine");
    mymqttmanager->publishtopic1("Pleine");
    Serial.println("");
  }
  mymqttmanager->clientLoop();
  delay(500);
}