#include "MySolenoide.h"

MySolenoide::MySolenoide()
{
    
}

MySolenoide::~MySolenoide()
{
}

bool MySolenoide::init()
{
    // État initial: relais désactivé (solénoïde fermé)
    pinMode(relayPinSolenoide, OUTPUT);
    digitalWrite(relayPinSolenoide, LOW);
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
