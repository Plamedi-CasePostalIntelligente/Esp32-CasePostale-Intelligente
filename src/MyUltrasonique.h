#ifndef MYULTRASONIQUE_H
#define MYULTRASONIQUE_H
#include <Arduino.h>

class MyUltrasonique
{
private:
    int trig_Pin;
    int echo_Pin;
    float duration_us;
    float distance_cm;
    float empty_box_distance;
    float threshold;
    float hysteresis;
    bool box_empty;
    bool is_calibrated;
    static const int NUM_SAMPLES = 5;
    float distance_samples[NUM_SAMPLES];
    int sample_index;

public:
    MyUltrasonique(int trigPin, int echoPin);
    bool FindEmptyBoxDistance();
    float GetDistance();
    bool IsBoxEmpty();
    bool IsCalibrated() { return is_calibrated; }
    bool IsSensorResponsive(); // Tester la réactivité sans recalibrer
};

#endif