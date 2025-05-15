#include "myLed.h"

// Constructeur
MyLed::MyLed(int ledPin) {
  pin = ledPin;
}

// Initialisation de la LED
void MyLed::init() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); // LED éteinte au démarrage
}

// Allumer la LED
void MyLed::turnOn() {
  digitalWrite(pin, HIGH);
}

// Éteindre la LED
void MyLed::turnOff() {
  digitalWrite(pin, LOW);
}

// Basculer l'état de la LED
void MyLed::toggle() {
  int state = digitalRead(pin);
  digitalWrite(pin, !state);
}