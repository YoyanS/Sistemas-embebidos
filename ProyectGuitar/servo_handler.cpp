#include "servo_handler.h"
#include "lcd_handler.h"
#include "audio_handler.h"

void setupServo() {
  servo1.attach(PIN_SERVO);
  servo1.write(NEUTRAL);
  delay(1000);
}

void tuneString(int freq_per, int target, const char* note) {
  int toleranceHz = 5;
  
  lcd.setCursor(0, 0);
  lcd.print("Nota: ");
  lcd.print(note);
  lcd.print("       ");

  if (freq_per < target - toleranceHz) {
    lcd.setCursor(10, 0);
    lcd.print("Apretar");
    Serial.println("Girando servo: APRETAR");
    servo1.write(APRETAR);
    delay(TIEMPO_GIRO);
    servo1.write(NEUTRAL);
    reproducirAudio("Apretar");  // ← VOZ APRETAR
  }
  else if (freq_per > target + toleranceHz) {
    lcd.setCursor(10, 0);
    lcd.print("Aflojar");
    Serial.println("Girando servo: AFLOJAR");
    servo1.write(SOLTAR);
    delay(TIEMPO_GIRO);
    servo1.write(NEUTRAL);
    reproducirAudio("Aflojar");  // ← VOZ AFLOJAR
  }
  else {
    lcd.setCursor(10, 0);
    lcd.print("Afinada!");
    servo1.write(NEUTRAL);
    reproducirAudio("Afinada");  // ← VOZ AFINADA
  }
}