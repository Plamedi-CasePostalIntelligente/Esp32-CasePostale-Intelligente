/*
 * Fichier: MyAPI.h
 * Créé le: 2024-03-01
 * Mis à jour le: 2024-05-01
 * Auteurs: Plamedi Ilunga
 * Contact: 2141363@cegeprdl.ca , 2038993@cegeprdl.ca ,2236622@cegeprdl.ca 
 * Version: 2.0
 * Description: Ce fichier définit la classe MyAPI
 * Licence: Arduino
 */
#ifndef MYAPI_H
#define MYAPI_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class MyAPI
{
public:
    MyAPI();
    bool getBrokerInfo();
    bool AjouterThermostat(String mac);

    // Getters
    String getMqttAddress() const { return _mqttAddress; }
    String getMqttPort() const { return _mqttPort; }
    String getMqttUser() const { return _mqttUser; }
    String getMqttPassword() const { return _mqttPassword; }

private:
    String _mqttAddress;
    String _mqttPort;
    String _mqttUser;
    String _mqttPassword;
};

#endif
