#include <ESP32Servo.h>
#include <LiquidCrystal_PCF8574.h>
#include "frequency_detector.h"
#include "lcd_handler.h"
#include "servo_handler.h"
#include "audio_handler.h"

// Instancias globales
LiquidCrystal_PCF8574 lcd(0x27);
Servo servo1;

// Variables globales principales
int rawData[LENGTH];
int count = 0;

void setup() {
  Serial.begin(115200);
  
  // Inicializar módulos
  setupLCD();
  setupServo();
  setupAudio();
  
  Serial.println("Sistema iniciado - Listo");
}

void loop() {
  // 1. Capturar señal de audio
  if (!captureAudioSignal()) {
    return;
  }

  // 2. Procesar frecuencia
  int freq_per = processFrequency();
  if (freq_per == 0) {
    return;
  }

  // 3. Mostrar frecuencia en LCD
  displayFrequency(freq_per);

  // 4. Detectar nota y afinar
  int noteIndex;
  if (detectNote(freq_per, noteIndex)) {
    tuneString(freq_per, freqs[noteIndex], notas[noteIndex]);
  } else {
    displayNoteNotFound();
  }

  delay(400);
}