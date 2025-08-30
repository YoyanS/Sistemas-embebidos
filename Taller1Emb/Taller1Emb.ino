/**
 * @file Taller1Emb.ino
 * @brief Calculadora básica con Arduino UNO/UNO R4.
 * @author Johan Steven Sanchez Gutierrez
 * El usuario selecciona una operación (+, -, *, /) y dos números enteros 
 * desde el Monitor Serial. El resultado se indica con parpadeos del LED:
 * 
 * - Resultado positivo  → 1 parpadeo
 * - Resultado negativo  → 2 parpadeos
 * - Resultado igual a 0 → 3 parpadeos
 */

int led = LED_BUILTIN; ///< LED integrado en la placa

// === Prototipos de funciones ===
void parpadear(int veces);
char leerOperacion();
int leerNumero();

/**
 * @brief Configuración inicial.
 */
void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);

  // Para UNO clásico: comentar la siguiente línea
  // while (!Serial) { ; }

  Serial.println("=== CALCULADORA SIMPLE ===");
  Serial.println("Escribe la operacion: + , - , * , /");
}

/**
 * @brief Bucle principal del programa.
 */
void loop() {
  if (Serial.available() > 0) {
    char operacion = leerOperacion(); ///< Leer operación (+, -, *, /)

    int num1 = leerNumero();  ///< Primer número
    int num2 = leerNumero();  ///< Segundo número
    int resultado = 0;

    // Realizar operación
    switch (operacion) {
      case '+': resultado = num1 + num2; break;
      case '-': resultado = num1 - num2; break;
      case '*': resultado = num1 * num2; break;
      case '/': resultado = num1 / num2; break;
      default:
        Serial.println("Operacion no valida.");
        return; ///< Sale de loop en esta iteración
    }

    // Mostrar resultado
    Serial.print("Resultado: ");
    Serial.println(resultado);

    // Parpadear según el signo
    if (resultado > 0) parpadear(1);
    else if (resultado < 0) parpadear(2);
    else parpadear(3);

    Serial.println("Operacion terminada. Escribe +, -, *, / para otra operacion:");
  }
}

/**
 * @brief Hace parpadear el LED integrado.
 * @param veces Número de parpadeos.
 */
void parpadear(int veces) {
  for (int i = 0; i < veces; i++) {
    digitalWrite(led, HIGH);
    delay(300);
    digitalWrite(led, LOW);
    delay(300);
  }
}

/**
 * @brief Lee la operación desde el Serial, ignorando saltos de línea.
 * @return char Carácter de la operación (+, -, *).
 */
char leerOperacion() {
  String linea = Serial.readStringUntil('\n'); // lee hasta Enter
  linea.trim(); // elimina espacios y saltos
  if (linea.length() > 0) {
    return linea.charAt(0); // primer carácter
  }
  return '?'; // valor por defecto si está vacío
}

/**
 * @brief Lee un número entero desde el Serial.
 * @return int Número introducido por el usuario.
 */
int leerNumero() {
  Serial.println("Ingresa un numero y presiona Enter:");
  while (Serial.available() == 0); // esperar entrada
  String linea = Serial.readStringUntil('\n');
  linea.trim();
  return linea.toInt(); // convierte la cadena a entero
}
