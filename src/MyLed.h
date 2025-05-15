#ifndef MY_LED_H
#define MY_LED_H

#include <Arduino.h>

class MyLed {
  private:
    int pin;
    
  public:
    // Constructeur
    MyLed(int ledPin);
    
    // Méthodes
    void init();
    void turnOn();
    void turnOff();
    void toggle();
};

#endif