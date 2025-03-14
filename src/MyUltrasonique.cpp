#include <MyUltrasonique.h>


MyUltrasonique::MyUltrasonique()
{
    pinMode(trig_Pin, OUTPUT);
    pinMode(echo_Pin, INPUT);
}

int MyUltrasonique::FindEmptyBoxDistance()
{
    for (int i = 0; i < 10; i++)
    {
        digitalWrite(trig_Pin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig_Pin, LOW);
        duration_us = pulseIn(echo_Pin, HIGH);
        sum += 0.017 * duration_us;
        delay(100);
    }
    empty_box_distance = sum / 10;
    return empty_box_distance;
}

int MyUltrasonique::GetDistance()
{
    digitalWrite(trig_Pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_Pin, LOW);
    duration_us = pulseIn(echo_Pin, HIGH);
    distance_cm = 0.017 * duration_us;
    return distance_cm;
}

bool MyUltrasonique::IsBoxEmpty()
{
    if (empty_box_distance - distance_cm > threshold)
    {
        box_empty = false;
    }
    else
    {
        box_empty = true;
    }
    return box_empty;
}
