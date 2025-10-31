#ifndef SERVO_HANDLER_H
#define SERVO_HANDLER_H

#include <ESP32Servo.h>

extern Servo servo1;

// Configuración servo
#define PIN_SERVO 18
#define TIEMPO_GIRO 400
#define SOLTAR 135
#define APRETAR 45
#define NEUTRAL 90

// Funciones servo
void setupServo();
void tuneString(int freq_per, int target, const char* note);

#endif