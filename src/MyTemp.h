/* MyTemp.h 
/* Copyright (C) 2024 Plamedi Ilunga
 * All rights reserved.
 *
 * Faire la gestion de la température et l'humidité
 *
 * */
#ifndef MyTemp_H_
#define MyTemp_H_

#include <DHT.h>

class MyTemp
{
private:
    //const int DEFAULT_PIN = 25;

    int inputPinUsed=25;
    #define typeSensorUsed DHT22
    //int typeSensorUsed;

    int UniteUsed;

    DHT *dht;

    float lastTemperatureRead;

    float lastHumidityRead;

public:
    enum UNITY_TEMP
    {
        UNITY_CELSIUS,
        UNITY_FAHRENHEIT
    };
    MyTemp();

    bool init();

    bool setUniteUsed(UNITY_TEMP uniteUsed);

    float getTemperature();

    float getHumidity();

    float getLastTemperatureRead();

    float getDefautPin();
};

#endif
