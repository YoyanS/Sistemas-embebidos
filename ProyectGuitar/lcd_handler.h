#ifndef LCD_HANDLER_H
#define LCD_HANDLER_H

#include <LiquidCrystal_PCF8574.h>

extern LiquidCrystal_PCF8574 lcd;

// Funciones LCD
void setupLCD();
void displayWeakSignal();
void displayOutOfRange(int freq_per);
void displayNoPeriod();
void displayFrequency(int freq_per);
void displayNoteNotFound();

#endif