
/*
 * Fichier: MyAPI.cpp
 * Créé le: 2024-03-01
 * Mis à jour le: 2024-05-01
 * Auteurs: Plamedi Ilunga
 * Contact: 2038993@cegeprdl.ca
 * Version: 2.0
 * Description: Ce fichier définit la classe MyAPI
 * Licence: Arduino
 */
#include "MyAPI.h"


MyAPI::MyAPI()
{
}

bool MyAPI::getBrokerInfo()
{
    HTTPClient http;

    Serial.println("Récupération des informations du broker...");

    http.begin("http://192.168.113.138:3003/api/broker/infoBroker");
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println("Réponse reçue: " + payload);

        // Allouer la mémoire pour le document JSON
        StaticJsonDocument<512> doc;

        // Parser la réponse JSON
        DeserializationError error = deserializeJson(doc, payload);

        // Vérifier si le parsing a réussi
        if (error)
        {
            Serial.print("Échec du parsing JSON: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }

        // Récupérer les valeurs en utilisant la nouvelle méthode
        _mqttAddress = doc["mqttAddress"].as<String>();
        _mqttPort = doc["mqttPort"].as<String>();
        _mqttUser = doc["mqttUser"].as<String>();
        _mqttPassword = doc["mqttPassword"].as<String>();

        Serial.println("Informations du broker récupérées :");
        Serial.println("Adresse: " + _mqttAddress);
        Serial.println("Port: " + _mqttPort);
        Serial.println("Utilisateur: " + _mqttUser);

        http.end();
        return true;
    }

    Serial.println("Erreur HTTP: " + String(httpCode));
    http.end();
    return false;
}