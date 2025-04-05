/**
    Gestion d'un senseur de température DHT22 Utilisant un GPIO d'un contrôleur

    @file MyTemp.cpp
    @author Plamedi Ilunga
    @version 1.1 03/03/2024
    Modifié le : 31/03/2024 , correction des bugs et erreurs de logiques

*/
#include "MyTemp.h"

MyTemp::MyTemp()
{
    //UniteUsed = UNITY_FAHRENHEIT;
}

bool MyTemp::init()
{
    const int nombreDelay = 500;
    dht = new DHT(inputPinUsed, typeSensorUsed);
    delay(nombreDelay);
    dht->begin();

    if (isnan(dht->readTemperature()))
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool MyTemp::setUniteUsed(UNITY_TEMP uniteUsed)
{
    if (uniteUsed != UNITY_CELSIUS && uniteUsed != UNITY_FAHRENHEIT)
    {
        return false;
    }
    this->UniteUsed = uniteUsed;
    return true;
}

float MyTemp::getTemperature()
{
    float temp = dht->readTemperature();
    if (isnan(temp))
    {
        return lastTemperatureRead; // Retourne la dernière température valide si la lecture est non définie
    }
    lastTemperatureRead = temp;
    if (UniteUsed == UNITY_FAHRENHEIT)
    {
        lastTemperatureRead = dht->convertCtoF(lastTemperatureRead);
    }
    return lastTemperatureRead;
}

float MyTemp::getHumidity()
{
    float hum = dht->readHumidity();
    if (isnan(hum))
    {
        return lastHumidityRead; // Retourne la dernière humidité valide si la lecture est non définie
    }
    lastHumidityRead = hum;
    return lastHumidityRead;
}
