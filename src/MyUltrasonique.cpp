#include "MyUltrasonique.h"

MyUltrasonique::MyUltrasonique()
{
    pinMode(trig_Pin, OUTPUT);
    pinMode(echo_Pin, INPUT);
    duration_us = 0;
    distance_cm = 0;
    empty_box_distance = 0;
    threshold = 2.0; // Seuil initial de 2 cm, ajustable selon tes besoins
    box_empty = true;
    sample_index = 0;
    // Initialiser le tableau des échantillons à 0
    for (int i = 0; i < NUM_SAMPLES; i++) {
        distance_samples[i] = 0;
    }
}

bool MyUltrasonique::FindEmptyBoxDistance()
{
    float sum = 0;
    const int num_measurements = 10; // 10 mesures pour la calibration

    Serial.println("Calibration de la distance de la boîte vide...");
    for (int i = 0; i < num_measurements; i++) {
        digitalWrite(trig_Pin, LOW);
        delayMicroseconds(2);
        digitalWrite(trig_Pin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig_Pin, LOW);

        duration_us = pulseIn(echo_Pin, HIGH, 30000); // Timeout de 30ms (env. 5m max)
        if (duration_us == 0) {
            Serial.println("Erreur : aucune réponse du capteur ultrasonique");
            return false;
        }

        float distance = (duration_us * 0.0343) / 2; // Conversion en cm
        if (distance > 400 || distance < 2) { // Limites du capteur (2cm à 400cm)
            Serial.println("Mesure hors plage : " + String(distance) + " cm");
            return false;
        }

        sum += distance;
        delay(50); // Petite pause entre les mesures
    }

    empty_box_distance = sum / num_measurements;
    Serial.println("Distance de référence (boîte vide) : " + String(empty_box_distance) + " cm");
    return true;
}

float MyUltrasonique::GetDistance()
{
    digitalWrite(trig_Pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_Pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_Pin, LOW);

    duration_us = pulseIn(echo_Pin, HIGH, 30000); // Timeout de 30ms
    if (duration_us == 0) {
        Serial.println("Erreur : capteur ultrasonique ne répond pas");
        return distance_cm; // Retourne la dernière valeur valide
    }

    float raw_distance = (duration_us * 0.0343) / 2; // Conversion en cm
    if (raw_distance < 2 || raw_distance > 400) {
        Serial.println("Distance hors plage : " + String(raw_distance) + " cm");
        return distance_cm; // Garde la dernière valeur valide
    }

    // Ajouter la nouvelle mesure au tableau
    distance_samples[sample_index] = raw_distance;
    sample_index = (sample_index + 1) % NUM_SAMPLES;

    // Calculer la moyenne mobile
    float sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += distance_samples[i];
    }
    distance_cm = sum / NUM_SAMPLES;

    Serial.println("Distance mesurée : " + String(distance_cm) + " cm");
    return distance_cm;
}

bool MyUltrasonique::IsBoxEmpty()
{
    float current_distance = GetDistance();
    // La boîte est vide si la distance est proche de la distance de référence
    box_empty = (current_distance >= (empty_box_distance - threshold));
    Serial.println("État de la boîte : " + String(box_empty ? "Vide" : "Pleine"));
    return box_empty;
}