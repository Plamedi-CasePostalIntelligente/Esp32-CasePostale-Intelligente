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
#include "MyRFID.h"
#include "MySolenoide.h"
#include <Arduino.h>
#include "MyOled.h"
#include "MyTemp.h"
#include "MyButton.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 4
#define BTN_RIGHT 4 // Bouton droit (entrée/sortie)
#define BTN_LEFT 16

int timeout = 10000; // 10 secondes de timeout
MyWifi *mywifi;
MyAPI *myapi;
MyMQTTManager *mymqttmanager;
MyUltrasonique *myUltrasonique;
MyRFID *myRFID;
MySolenoide *mySolenoide;
MyTemp *mytemp = NULL;
MyOled *myOled = NULL;
MyButton *myButtonLEFT = NULL;
MyButton *myButtonRIGHT = NULL;

int buttonleftState;
int buttonrightState;
float temperature;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  mywifi = new MyWifi();
  myapi = new MyAPI();
  mymqttmanager = new MyMQTTManager();
  myUltrasonique = new MyUltrasonique();
  myRFID = new MyRFID();
  myButtonLEFT = new MyButton(BTN_LEFT);
  myButtonRIGHT = new MyButton(BTN_RIGHT);
  mytemp = new MyTemp();
  myOled = new MyOled(&Wire, OLED_RESET, SCREEN_HEIGHT, SCREEN_WIDTH);
  // mySolenoide = new MySolenoide();

  while (!mywifi)
  {
    Serial.println("Erreur d'instanciation de la classe MyWifi");
  }

  while (!myapi)
  {
    Serial.println("Erreur d'instanciation de la classe MyAPI");
  }

  while (!mymqttmanager)
  {
    Serial.println("Erreur d'instanciation de la classe MyMQTTManager");
  }

  while (!myUltrasonique)
  {
    Serial.println("Erreur d'instanciation de la classe MyUltrasonique");
  }

  while (!myRFID)
  {
    Serial.println("Erreur d'instanciation de la classe MyRFID");
  }
  /*
  while (!mySolenoide)
  {
    Serial.println("Erreur d'instanciation de la classe MySolenoide");
  }*/

  if (!myButtonLEFT || !myButtonRIGHT)
  {
    Serial.println("Erreur d'instanciation de la classe MyButton");
  }

  if (!mytemp)
  {
    Serial.println("Erreur d'instanciation de la classe MyTemp");
  }

  if (!mytemp->init())
  {
    Serial.println("Erreur d'initialisation de la classe MyTemp");
  }
  mytemp->setUniteUsed(MyTemp::UNITY_CELSIUS);

  if (!myOled || myOled->init(2) != 0)
  { // 2 secondes de splash, retourne 0 si OK
    Serial.println("Erreur d'instanciation ou d'initialisation de la classe MyOled");
    while (1)
      ; // Boucle infinie si échec
  }

  if (!mywifi->connect())
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

  if (!myUltrasonique->FindEmptyBoxDistance())
  {
    Serial.println("Erreur de récupération de la distance de la boîte vide");
  }
  else
  {
    Serial.println("Distance de la boîte vide récupérée avec succès!");
  }

  if (!myRFID->IsRFIDDetected())
  {
    Serial.println("Erreur de détection du module RFID");
  }
  else
  {
    Serial.println("Module RFID détecté avec succès!");
  }
  /*
    if(!mySolenoide->init())
    {
      Serial.println("Erreur d'initialisation de la classe MySolenoide");
    }
    else
    {
      Serial.println("Initialisation de la classe MySolenoide réussie!");
    }*/
}

void loop()
{
  // put your main code here, to run repeatedly:
  mywifi->checkResetButton();
  myUltrasonique->GetDistance();
  myRFID->ReadCardSerial();

  buttonleftState = myButtonLEFT->ButtonPressed();
  buttonrightState = myButtonRIGHT->ButtonPressed();
  temperature = mytemp->getTemperature();

  if (buttonleftState == 1)
  {
    Serial.println(buttonleftState);
    myOled->moveLeftButton();
    delay(100); // Anti-rebond
  }

  if (buttonrightState == 1)
  {
    Serial.println(buttonrightState);
    myOled->moveRightButton(temperature); // Passe la température réelle
    delay(100);
  }

  // Affiche toujours quelque chose
  if (myOled->getCurrentSubMenu() == -1)
  {
    myOled->displayMainMenu(); // Menu principal si pas dans un sous-menu
  }
  else
  {
    myOled->displaySubMenu(myOled->getCurrentSubMenu(), temperature); // Sous-menu avec température réelle
  }

  String nombreRfid = myRFID->ReadCardSerial();
  /*
  if(nombreRfid=="BB776813")
  {
    mySolenoide->openCase();
    delay(5000);
  }
  mySolenoide->closeCase();*/
  Serial.println("UID: " + nombreRfid);

  if (myUltrasonique->IsBoxEmpty())
  {
    // Serial.println("La boîte est vide");
    mymqttmanager->publishtopic1("Vide");
  }
  else
  {
    // Serial.println("La boîte est pleine");
    mymqttmanager->publishtopic1("Pleine");
  }
  mymqttmanager->clientLoop();
  delay(1000);
}