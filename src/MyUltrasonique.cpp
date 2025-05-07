#include "MyUltrasonique.h"

MyUltrasonique::MyUltrasonique(int trigPin, int echoPin)
{
    trig_Pin = trigPin;
    echo_Pin = echoPin;
    pinMode(trig_Pin, OUTPUT);
    pinMode(echo_Pin, INPUT);
    duration_us = 0;
    distance_cm = 0;
    empty_box_distance = 0;
    threshold = 2.0;
    hysteresis = 0.5;
    box_empty = true;
    is_calibrated = false;
    sample_index = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        distance_samples[i] = 0;
    }
}

bool MyUltrasonique::FindEmptyBoxDistance()
{
    if (is_calibrated) {
        Serial.println("Calibration déjà effectuée pour trig_Pin=" + String(trig_Pin) + ", ignorée.");
        return true;
    }

    float sum = 0;
    const int num_measurements = 10;

    Serial.println("Calibration de la distance de la boîte vide pour trig_Pin=" + String(trig_Pin) + "...");
    for (int i = 0; i < num_measurements; i++) {
        digitalWrite(trig_Pin, LOW);
        delayMicroseconds(2);
        digitalWrite(trig_Pin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig_Pin, LOW);

        duration_us = pulseIn(echo_Pin, HIGH, 50000);
        if (duration_us == 0) {
            Serial.println("Erreur : aucune réponse du capteur ultrasonique pour trig_Pin=" + String(trig_Pin));
            return false;
        }

        float distance = (duration_us * 0.0343) / 2;
        if (distance > 400 || distance < 2) {
            Serial.println("Mesure hors plage : " + String(distance) + " cm pour trig_Pin=" + String(trig_Pin));
            return false;
        }

        sum += distance;
        delay(50);
    }

    empty_box_distance = sum / num_measurements;
    is_calibrated = true;
    Serial.println("Distance de référence (boîte vide) : " + String(empty_box_distance) + " cm pour trig_Pin=" + String(trig_Pin));
    return true;
}

float MyUltrasonique::GetDistance()
{
    digitalWrite(trig_Pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_Pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_Pin, LOW);

    duration_us = pulseIn(echo_Pin, HIGH, 50000);
    if (duration_us == 0) {
        Serial.println("Erreur : capteur ultrasonique ne répond pas pour trig_Pin=" + String(trig_Pin));
        return distance_cm;
    }

    float raw_distance = (duration_us * 0.0343) / 2;
    if (raw_distance < 2 || raw_distance > 400) {
        Serial.println("Distance hors plage : " + String(raw_distance) + " cm pour trig_Pin=" + String(trig_Pin));
        return distance_cm;
    }

    distance_samples[sample_index] = raw_distance;
    sample_index = (sample_index + 1) % NUM_SAMPLES;

    float sum = 0;
    int valid_samples = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        if (distance_samples[i] >= 2 && distance_samples[i] <= 400) {
            sum += distance_samples[i];
            valid_samples++;
        }
    }
    distance_cm = valid_samples > 0 ? sum / valid_samples : distance_cm;

    Serial.println("Distance mesurée : " + String(distance_cm) + " cm pour trig_Pin=" + String(trig_Pin));
    return distance_cm;
}

bool MyUltrasonique::IsBoxEmpty()
{
    if (!is_calibrated) {
        Serial.println("Erreur : capteur non calibré pour trig_Pin=" + String(trig_Pin));
        return box_empty;
    }

    float current_distance = GetDistance();
    if (current_distance < 2 || current_distance > 400) {
        Serial.println("Distance invalide, état inchangé : " + String(box_empty ? "Vide" : "Pleine") + " pour trig_Pin=" + String(trig_Pin));
        return box_empty;
    }

    if (box_empty && current_distance < (empty_box_distance - threshold - hysteresis)) {
        box_empty = false;
    } else if (!box_empty && current_distance > (empty_box_distance - threshold + hysteresis)) {
        box_empty = true;
    }

    Serial.println("État de la boîte : " + String(box_empty ? "Vide" : "Pleine") + " pour trig_Pin=" + String(trig_Pin));
    return box_empty;
}

bool MyUltrasonique::IsSensorResponsive()
{
    digitalWrite(trig_Pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_Pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_Pin, LOW);

    duration_us = pulseIn(echo_Pin, HIGH, 50000);
    if (duration_us == 0) {
        Serial.println("Test réactivité : capteur ultrasonique ne répond pas pour trig_Pin=" + String(trig_Pin));
        return false;
    }

    float distance = (duration_us * 0.0343) / 2;
    if (distance < 2 || distance > 400) {
        Serial.println("Test réactivité : mesure hors plage : " + String(distance) + " cm pour trig_Pin=" + String(trig_Pin));
        return false;
    }

    Serial.println("Test réactivité : capteur ultrasonique répond correctement pour trig_Pin=" + String(trig_Pin));
    return true;
}