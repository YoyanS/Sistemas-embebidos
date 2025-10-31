#ifndef FREQUENCY_DETECTOR_H
#define FREQUENCY_DETECTOR_H

#include <Arduino.h>

#define LENGTH 1024
#define SAMPLE_PERIOD 150

// Frecuencias de las cuerdas
extern const int freqs[];
extern const char* notas[];
extern const int numNotas;

// Variables globales
extern int rawData[];
extern int count;

// Funciones
bool captureAudioSignal();
int processFrequency();
bool detectNote(int freq_per, int& noteIndex);

#endif