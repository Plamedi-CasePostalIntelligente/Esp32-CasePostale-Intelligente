#ifndef MYULTRASONIQUE_H
#define MYULTRASONIQUE_H
#include <Arduino.h>

class MyUltrasonique
{
private:
    int trig_Pin = 32;//6;//22;  // ESP32 pin GPIO22 connected to Ultrasonic Sensor's TRIG pin
    int echo_Pin = 33;//7;//21 ;   // ESP32 pin GPIO21 connected to Ultrasonic Sensor's ECHO pin
    float duration_us; // Durée de l'onde ultrasonore en microsecondes
    float sum = 0;     // Somme des mesures pour la distance de référence
    float distance_cm;
    float empty_box_distance; // Distance quand la boîte est vide
    float threshold = 0.5;  
    bool box_empty = true;

public:
    MyUltrasonique();
    int FindEmptyBoxDistance();
    int GetDistance();
    bool IsBoxEmpty();
};
#endif