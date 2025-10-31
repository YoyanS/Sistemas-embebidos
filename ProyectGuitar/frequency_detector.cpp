#include "frequency_detector.h"
#include "lcd_handler.h"

const int freqs[] = {82, 110, 147, 196, 247, 330};
const char* notas[] = {"Mi2", "La", "Re", "Sol", "Si", "Mi1"};
const int numNotas = 6;

long sum, sum_old;
int thresh = 0;
byte pd_state = 0;

bool captureAudioSignal() {
  count = 0;
  int signalMax = 0;
  int signalMin = 4095;
  
  while (count < LENGTH) {
    unsigned long startTime = micros();
    int adcValue = analogRead(36);
    rawData[count] = adcValue;
    
    if (adcValue > signalMax) signalMax = adcValue;
    if (adcValue < signalMin) signalMin = adcValue;
    
    count++;
    long elapsed = micros() - startTime;
    long timetodelay = SAMPLE_PERIOD - elapsed;
    if (timetodelay > 0) {
      delayMicroseconds(timetodelay);
    }
  }

  int signalAmplitude = signalMax - signalMin;
  if (signalAmplitude < 50) {
    displayWeakSignal();
    return false;
  }
  return true;
}

int processFrequency() {
  float realSampleFreq = 1000000.0 / SAMPLE_PERIOD;

  // Autocorrelación
  sum = 0;
  pd_state = 0;
  int period = 0;
  bool periodFound = false;
  int best_period = 0;
  long best_correlation = -1000000;

  for (int i = 5; i < LENGTH/3; i++) {
    sum_old = sum;
    sum = 0;
    
    for (int k = 0; k < LENGTH - i; k += 2) {
      sum += (rawData[k] - 2048) * (rawData[k + i] - 2048);
    }

    if (sum > best_correlation && i > 8) {
      best_correlation = sum;
      best_period = i;
    }

    if (i == 5) {
      thresh = sum * 0.3;
      pd_state = 1;
    }
    else if (pd_state == 1 && sum > thresh && (sum - sum_old) > 0) {
      pd_state = 2;
    }
    else if (pd_state == 2 && (sum - sum_old) <= 0) {
      period = i;
      periodFound = true;
    }
  }

  if (best_period > 0) {
    period = best_period;
    periodFound = true;
  }

  if (periodFound && period > 0) {
    int freq_per = int((realSampleFreq / period) + 0.5);
    
    if (freq_per < 60 || freq_per > 400) {
      displayOutOfRange(freq_per);
      return 0;
    }
    
    Serial.print("Frecuencia detectada: ");
    Serial.println(freq_per);
    return freq_per;
  }
  
  displayNoPeriod();
  return 0;
}

bool detectNote(int freq_per, int& noteIndex) {
  for (int j = 0; j < numNotas; j++) {
    int target = freqs[j];
    int lowerBound = target - 15;
    int upperBound = target + 15;
    
    if (freq_per >= lowerBound && freq_per <= upperBound) {
      noteIndex = j;
      return true;
    }
  }
  return false;
}