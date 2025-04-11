#ifndef MYULTRASONIQUE_H
#define MYULTRASONIQUE_H
#include <Arduino.h>

class MyUltrasonique
{
private:
    int trig_Pin = 32;  // GPIO32 pour TRIG
    int echo_Pin = 33;  // GPIO33 pour ECHO
    float duration_us;  // Durée de l'onde en microsecondes
    float distance_cm;  // Distance mesurée
    float empty_box_distance; // Distance de référence (boîte vide)
    float threshold;    // Seuil pour déterminer si la boîte est vide ou pleine
    bool box_empty;     // État actuel de la boîte
    static const int NUM_SAMPLES = 5; // Nombre d'échantillons pour la moyenne mobile
    float distance_samples[NUM_SAMPLES]; // Tableau pour stocker les échantillons
    int sample_index;   // Index pour la moyenne mobile

public:
    MyUltrasonique();
    bool FindEmptyBoxDistance(); // Retourne true si calibration réussie
    float GetDistance();         // Retourne la distance filtrée
    bool IsBoxEmpty();           // Retourne l'état de la boîte
};
#endif