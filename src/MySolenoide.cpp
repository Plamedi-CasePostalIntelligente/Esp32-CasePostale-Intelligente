#include "MySolenoide.h"

MySolenoide::MySolenoide(int _relayPinSolenoide)
{
    relayPinSolenoide = _relayPinSolenoide;
}

MySolenoide::~MySolenoide()
{
}

bool MySolenoide::init()
{
    // État initial: relais désactivé (solénoïde fermé)
    pinMode(relayPinSolenoide, OUTPUT);
    return true;
}

void MySolenoide::openCase()
{
    // Activation du relais 1 (solénoïde ouvert)
    digitalWrite(relayPinSolenoide, HIGH);
}

void MySolenoide::closeCase()
{
    // Activation du relais 1 (solénoïde ouvert)
    digitalWrite(relayPinSolenoide, LOW);
}
