#include "lcd_handler.h"

void setupLCD() {
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Afinador ESP32");
  delay(2000);
  lcd.clear();
}

void displayWeakSignal() {
  lcd.setCursor(0, 0);
  lcd.print("Toca una cuerda ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void displayOutOfRange(int freq_per) {
  lcd.setCursor(0, 0);
  lcd.print("Toca una cuerda ");
  lcd.setCursor(0, 1);
  lcd.print("Freq: ");
  lcd.print(freq_per);
  lcd.print(" Hz ");
}

void displayNoPeriod() {
  lcd.setCursor(0, 0);
  lcd.print("Toca una cuerda ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void displayFrequency(int freq_per) {
  lcd.setCursor(0, 1);
  lcd.print("Freq: ");
  lcd.print(freq_per);
  lcd.print(" Hz   ");
}

void displayNoteNotFound() {
  lcd.setCursor(0, 0);
  lcd.print("Nota no detectada");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}