/**
 * @file ESP32_LAB2_WebServer.ino
 * @brief Control de salidas digitales y PWM mediante servidor web en modo Access Point (AP).
 *
 * @details
 * Este programa configura el ESP32 como punto de acceso Wi-Fi (AP) con un servidor web embebido.
 * Desde una página web se pueden controlar dos salidas digitales (GPIO18 y GPIO19)
 * y una salida PWM (GPIO23) usando un control deslizante (0–255).
 *
 * @author
 *   Johan Steven Sanchez Gutierrez
 * @date
 *   Octubre 2025
 */

#include <WiFi.h>
#include <WebServer.h>

/* -------------------- CONFIGURACIÓN GENERAL -------------------- */
const char *AP_SSID     = "ESP32-WIFI_LAB2";   ///< Nombre de la red Wi-Fi
const char *AP_PASSWORD = "Zinedine98";        ///< Contraseña de la red Wi-Fi
const int   SERVER_PORT = 80;                  ///< Puerto HTTP del servidor web

/* -------------------- CONFIGURACIÓN DE PINES -------------------- */
const uint8_t PIN_LED1 = 18;   ///< LED digital 1 (GPIO18)
const uint8_t PIN_LED2 = 19;   ///< LED digital 2 (GPIO19)
const uint8_t PIN_PWM  = 23;   ///< LED PWM controlado por deslizador (GPIO23)

WebServer server(SERVER_PORT);
volatile uint8_t pwmDuty = 0;  ///< Valor actual del PWM (0–255)

/* =================================================================
 *  FUNCIONES AUXILIARES
 * ================================================================= */

/**
 * @brief Devuelve el estado actual de los pines en formato JSON.
 * @return Cadena JSON con los valores de los GPIO18, GPIO19 y PWM23.
 */
String buildJsonState() {
  String json = "{";
  json += "\"D18\":" + String(digitalRead(PIN_LED1)) + ",";
  json += "\"D19\":" + String(digitalRead(PIN_LED2)) + ",";
  json += "\"PWM23\":" + String(pwmDuty);
  json += "}";
  return json;
}

/* =================================================================
 *  INTERFAZ WEB
 * ================================================================= */

/**
 * @brief Envía la página principal HTML al cliente.
 */
void handleRoot() {
  String html =
  "<!DOCTYPE html><html lang='es'><head><meta charset='utf-8'/>"
  "<meta name='viewport' content='width=device-width,initial-scale=1'/>"
  "<title>ESP32 - Control Web</title>"
  "<style>"
  "body{background:#202124;color:#eee;font-family:Arial;text-align:center;padding:20px;}"
  ".card{background:#2c2c2c;border-radius:12px;padding:20px;max-width:600px;margin:auto;box-shadow:0 0 10px #0008;}"
  "h1{color:#00d4ff;}"
  "button{background:#303f9f;color:#fff;padding:10px 20px;border:none;border-radius:6px;margin:6px;cursor:pointer;font-size:15px;}"
  "button:hover{background:#3f51b5;}"
  ".tag{display:inline-block;min-width:80px;padding:6px;border-radius:10px;margin-left:8px;}"
  ".on{background:#1b5e20;}"
  ".off{background:#b71c1c;}"
  "input[type=range]{width:240px;}"
  "</style></head><body>"
  "<div class='card'>"
  "<h1>ESP32 - Control de LEDs</h1>"
  "<p>Red Wi-Fi: <b>ESP32-WIFI_LAB2</b> | Clave: <b>Zinedine98</b></p>"

  "<h2>GPIO18</h2>"
  "<button onclick=\"setPin(18,1)\">Encender</button>"
  "<button onclick=\"setPin(18,0)\">Apagar</button>"
  "<span id='led18' class='tag'>...</span>"

  "<h2>GPIO19</h2>"
  "<button onclick=\"setPin(19,1)\">Encender</button>"
  "<button onclick=\"setPin(19,0)\">Apagar</button>"
  "<span id='led19' class='tag'>...</span>"

  "<h2>GPIO23 (PWM)</h2>"
  "<input id='slider' type='range' min='0' max='255' step='1' oninput='livePWM(this.value)'>"
  "<p>Brillo: <b id='pwmval'>0</b></p>"
  "<span id='tagPWM' class='tag'>PWM:0</span>"

  "<p><small>Conecta LEDs (con resistencia de 220Ω) a GPIO18, GPIO19 y GPIO23.</small></p>"
  "</div>"

  "<script>"
  "function setPin(pin,val){fetch('/api/set?pin='+pin+'&val='+val).then(r=>r.json()).then(updateUI);}"
  "function setPWM(v){fetch('/api/pwm?duty='+v).then(r=>r.json()).then(updateUI);}"
  "function livePWM(v){document.getElementById('pwmval').textContent=v;"
  "document.getElementById('tagPWM').textContent='PWM:'+v;"
  "clearTimeout(window.t);window.t=setTimeout(()=>setPWM(v),120);}"
  
  // 🔁 CORRECCIÓN VISUAL: los estados ahora se invierten para mostrar correctamente "ON/OFF"
  "function updateUI(j){"
  "upd('led18',!j.D18);"   // invertido
  "upd('led19',!j.D19);"   // invertido
  "document.getElementById('slider').value=j.PWM23;"
  "document.getElementById('pwmval').textContent=j.PWM23;"
  "document.getElementById('tagPWM').textContent='PWM:'+j.PWM23;"
  "}"
  
  "function upd(id,on){const el=document.getElementById(id);"
  "el.textContent=(on?'ON':'OFF');el.className='tag '+(on?'on':'off');}"
  "async function refresh(){const j=await (await fetch('/api/state')).json();updateUI(j);} "
  "setInterval(refresh,1500);refresh();"
  "</script>"
  "</body></html>";

  server.send(200, "text/html", html);
}

/* =================================================================
 *  ENDPOINTS API
 * ================================================================= */

/**
 * @brief Envía el estado actual de los pines en formato JSON.
 */
void handleApiState() {
  server.send(200, "application/json", buildJsonState());
}

/**
 * @brief Cambia el estado lógico de las salidas digitales (GPIO18 o GPIO19).
 */
void handleApiSet() {
  if (!server.hasArg("pin") || !server.hasArg("val")) {
    server.send(400, "text/plain", "Faltan argumentos pin/val"); return;
  }

  int pin = server.arg("pin").toInt();
  int val = server.arg("val").toInt();

  // 🔁 INVERSIÓN DE LÓGICA: ahora HIGH apaga y LOW enciende
  if (!((pin == PIN_LED1) || (pin == PIN_LED2)) || (val < 0 || val > 1)) {
    server.send(400, "text/plain", "Argumentos invalidos"); return;
  }

  digitalWrite(pin, val ? LOW : HIGH);  // Invertido
  server.send(200, "application/json", buildJsonState());
}

/**
 * @brief Actualiza el valor del PWM (GPIO23) con el duty ingresado (0–255).
 */
void handleApiPWM() {
  if (!server.hasArg("duty")) {
    server.send(400, "text/plain", "Falta parametro duty (0–255)"); return;
  }

  int d = server.arg("duty").toInt();
  if (d < 0) d = 0;
  if (d > 255) d = 255;

  pwmDuty = (uint8_t)d;

  // 🔁 INVERSIÓN DE PWM: máximo = LED encendido, mínimo = LED apagado
  analogWrite(PIN_PWM, 255 - pwmDuty);

  server.send(200, "application/json", buildJsonState());
}

/**
 * @brief Página de error 404 para rutas no definidas.
 */
void handleNotFound() {
  server.send(404, "text/html",
    "<html><body><h3>Error 404 - Recurso no encontrado</h3><a href='/'>Volver</a></body></html>");
}

/* =================================================================
 *  SETUP Y LOOP PRINCIPAL
 * ================================================================= */

/**
 * @brief Inicializa el sistema, el AP y el servidor web.
 */
void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED1, OUTPUT); digitalWrite(PIN_LED1, HIGH);  // apagado por defecto
  pinMode(PIN_LED2, OUTPUT); digitalWrite(PIN_LED2, HIGH);
  pinMode(PIN_PWM, OUTPUT);  analogWrite(PIN_PWM, 255);     // apagado (inverso)

  // Crear red Wi-Fi AP
  if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
    Serial.println("Error al crear el punto de acceso");
    while (true) delay(1000);
  }

  Serial.print("AP iniciado: "); Serial.println(AP_SSID);
  Serial.print("IP del ESP32: "); Serial.println(WiFi.softAPIP());

  // Rutas HTTP
  server.on("/", handleRoot);
  server.on("/api/state", handleApiState);
  server.on("/api/set", handleApiSet);
  server.on("/api/pwm", handleApiPWM);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor web en ejecución.");
}

/**
 * @brief Bucle principal: atiende las peticiones del cliente.
 */
void loop() {
  server.handleClient();
  delay(5);
}
