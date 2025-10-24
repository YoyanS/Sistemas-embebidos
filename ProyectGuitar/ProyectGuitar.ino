#include <ESP32Servo.h>
#include <LiquidCrystal_PCF8574.h>

#define LENGTH 1024              // Muestras para mejor resolución
#define SAMPLE_PERIOD 150        // 150us → ~6.67 kHz
int rawData[LENGTH];             
int count = 0;

int PinServo = 18;
Servo servo1;
int TiempoGiro = 400;            
int soltar = 135;                
int apretar = 45;                

LiquidCrystal_PCF8574 lcd(0x27);

long sum, sum_old;
int thresh = 0;
int freq_per = 0;
byte pd_state = 0;

// Frecuencias de las 6 cuerdas de guitarra estándar
int freqs[] = {82, 110, 147, 196, 247, 330};             
const char* notas[] = {"Mi2", "La", "Re", "Sol", "Si", "Mi1"};
const int numNotas = 6;

void setup() {
  Serial.begin(115200);
  servo1.attach(PinServo);
  servo1.write(90);

  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Afinador ESP32");
  delay(2000);
  lcd.clear();
}

void loop() {
  // ---- 1. Capturar señal ----
  count = 0;
  int signalMax = 0;
  int signalMin = 4095;
  
  while (count < LENGTH) {
    unsigned long startTime = micros();
    int adcValue = analogRead(36);           
    rawData[count] = adcValue;
    
    // Calcular amplitud de la señal
    if (adcValue > signalMax) signalMax = adcValue;
    if (adcValue < signalMin) signalMin = adcValue;
    
    count++;
    long elapsed = micros() - startTime;
    long timetodelay = SAMPLE_PERIOD - elapsed;
    if (timetodelay > 0) {
      delayMicroseconds(timetodelay);
    }
  }

  // Verificar si la señal tiene suficiente amplitud
  int signalAmplitude = signalMax - signalMin;
  Serial.print("Amplitud señal: ");
  Serial.println(signalAmplitude);
  
  if (signalAmplitude < 50) {
    lcd.setCursor(0, 0);
    lcd.print("Se escucha borroso");
    lcd.setCursor(0, 1);
    lcd.print("Sonido muy debil");
    servo1.write(90);
    delay(500);
    return;
  }

  float realSampleFreq = 1000000.0 / SAMPLE_PERIOD;

  // ---- 2. Autocorrelación MEJORADA ----
  sum = 0;
  pd_state = 0;
  int period = 0;
  bool periodFound = false;
  int best_period = 0;
  long best_correlation = -1000000;

  for (int i = 5; i < LENGTH/3; i++) {  // Reducir rango de búsqueda
    sum_old = sum;
    sum = 0;
    
    // Calcular autocorrelación con saltos para mayor velocidad
    for (int k = 0; k < LENGTH - i; k += 2) {
      sum += (rawData[k] - 2048) * (rawData[k + i] - 2048);
    }

    // Buscar la correlación más alta (más confiable)
    if (sum > best_correlation && i > 8) {
      best_correlation = sum;
      best_period = i;
    }

    // Lógica original de detección de cruce por cero
    if (i == 5) {
      thresh = sum * 0.3;   // Umbral más bajo
      pd_state = 1;
    }
    else if (pd_state == 1 && sum > thresh && (sum - sum_old) > 0) {
      pd_state = 2;
    }
    else if (pd_state == 2 && (sum - sum_old) <= 0) {
      period = i;
      periodFound = true;
      // No romper - continuar buscando el mejor
    }
  }

  // Usar el periodo con mejor correlación
  if (best_period > 0) {
    period = best_period;
    periodFound = true;
    Serial.print("Mejor periodo: ");
    Serial.print(best_period);
    Serial.print(", Correlacion: ");
    Serial.println(best_correlation);
  }

  // ---- 3. Calcular frecuencia ----
  if (periodFound && period > 0) {
    freq_per = int((realSampleFreq / period) + 0.5);
    
    Serial.print("Frecuencia detectada: ");
    Serial.println(freq_per);

    // RANGO MUCHO MÁS AMPLIO para pruebas
    if (freq_per < 60 || freq_per > 400) {
      lcd.setCursor(0, 0);
      lcd.print("Se escucha borroso");
      lcd.setCursor(0, 1);
      lcd.print("Freq: ");
      lcd.print(freq_per);
      lcd.print(" Hz ");
      servo1.write(90);
      delay(300);
      return;
    }

    lcd.setCursor(0, 1);
    lcd.print("Freq: ");
    lcd.print(freq_per);
    lcd.print(" Hz   ");

    bool foundNote = false;

    // ---- 4. Rangos MUY AMPLIOS para pruebas ----
    for (int j = 0; j < numNotas; j++) {
      int target = freqs[j];
      
      // RANGOS MUY AMPLIOS - ±15Hz para todas las cuerdas
      int lowerBound = target - 15;
      int upperBound = target + 15;
      
      Serial.print("Probando ");
      Serial.print(notas[j]);
      Serial.print(": ");
      Serial.print(lowerBound);
      Serial.print("-");
      Serial.print(upperBound);
      Serial.print(" Hz -> ");
      
      if (freq_per >= lowerBound && freq_per <= upperBound) {
        Serial.println("DETECTADA!");
        foundNote = true;

        // Mostrar nota detectada
        lcd.setCursor(0, 0);
        lcd.print("Nota: ");
        lcd.print(notas[j]);
        lcd.print(" (");
        lcd.print(target);
        lcd.print(") ");

        // ---- 5. Afinación automática ----
        int toleranceHz = 5; // Margen más amplio para pruebas
        
        if (freq_per < target - toleranceHz) {
          lcd.setCursor(0, 0);
          lcd.print("Nota: ");
          lcd.print(notas[j]);
          lcd.print(" Apretar");
          servo1.write(apretar);
          delay(TiempoGiro);
          servo1.write(90);
        }
        else if (freq_per > target + toleranceHz) {
          lcd.setCursor(0, 0);
          lcd.print("Nota: ");
          lcd.print(notas[j]);
          lcd.print(" Aflojar");
          servo1.write(soltar);
          delay(TiempoGiro);
          servo1.write(90);
        }
        else {
          lcd.setCursor(0, 0);
          lcd.print("Nota: ");
          lcd.print(notas[j]);
          lcd.print(" Afinada!");
          servo1.write(90);
        }
        break;
      } else {
        Serial.println("NO");
      }
    }

    if (!foundNote) {
      lcd.setCursor(0, 0);
      lcd.print("Fuera de rangos  ");
      Serial.println("--- FUERA DE TODOS LOS RANGOS ---");
      
      // Mostrar info de debug
      Serial.println("Rangos actuales:");
      for (int j = 0; j < numNotas; j++) {
        Serial.print(notas[j]);
        Serial.print(" (");
        Serial.print(freqs[j]);
        Serial.print(" Hz): ");
        Serial.print(freqs[j] - 15);
        Serial.print(" - ");
        Serial.print(freqs[j] + 15);
        Serial.println(" Hz");
      }
    }
  } else {
    // No se detectó periodo claro
    lcd.setCursor(0, 0);
    lcd.print("Se escucha borroso");
    lcd.setCursor(0, 1);
    lcd.print("No hay periodo   ");
    servo1.write(90);
    Serial.println("No se pudo detectar periodo");
  }

  delay(400);
}