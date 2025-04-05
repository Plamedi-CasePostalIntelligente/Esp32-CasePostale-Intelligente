#include "MyButton.h"
#include <Arduino.h>
MyButton::MyButton(int buttonPin)
{
    _buttonPin = buttonPin;
    pinMode(buttonPin, INPUT_PULLUP); // Pas de pull-up interne, pull-down externe
}

MyButton::~MyButton()
{
}

int MyButton::ButtonPressed()
{
    return digitalRead(_buttonPin) == LOW;
}