/*
 * =====================================================
 *  ESP32 ARCADE NFC — Modo Serial USB (para launcher.py)
 * =====================================================
 *  Lee la tarjeta NFC y manda el UID por Serial USB
 *  en formato  UID:XXXXXXXXXXXX
 *  para que launcher.py lo reciba directamente.
 *
 *  Conexiones PN532 (modo I2C):
 *    PN532 VCC  -> 3.3V
 *    PN532 GND  -> GND
 *    PN532 SDA  -> GPIO 21
 *    PN532 SCL  -> GPIO 22
 *    PN532 IRQ  -> GPIO 4  (opcional)
 *    PN532 RST  -> GPIO 5  (opcional)
 *
 *    JUMPERS DEL PN532: ambos en posición 1 (ON) = modo I2C
 *
 *  Librerías necesarias (Library Manager):
 *    - Adafruit PN532   (by Adafruit)
 *    - Wire             (incluida)
 * =====================================================
 */

#include <Wire.h>
#include <Adafruit_PN532.h>

// ============================================================
//  PINES
// ============================================================
#define PN532_IRQ_PIN   4
#define PN532_RST_PIN   5
#define I2C_SDA_PIN    21
#define I2C_SCL_PIN    22
#define LED_PIN         2    // LED azul integrado del ESP32
                             // Si usas ESP32-S3, cámbialo a 48

// Tiempo mínimo entre lecturas de la misma tarjeta (ms)
#define COOLDOWN_MS   4000

// ============================================================
//  VARIABLES GLOBALES
// ============================================================
Adafruit_PN532 nfc(PN532_IRQ_PIN, PN532_RST_PIN);

String        lastRFID    = "";
unsigned long lastReadTime = 0;
bool          nfcOK        = false;

// ============================================================
//  LED — helpers
// ============================================================
void ledOn()  { digitalWrite(LED_PIN, HIGH); }
void ledOff() { digitalWrite(LED_PIN, LOW);  }

void flashLED(int veces, int ms) {
  for (int i = 0; i < veces; i++) {
    ledOn();  delay(ms);
    ledOff(); delay(ms);
  }
}

// ============================================================
//  Leer tarjeta NFC — retorna UID como String hex o ""
// ============================================================
String leerNFC() {
  uint8_t uid[7];
  uint8_t uidLen = 0;

  bool found = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 100);
  if (!found || uidLen == 0) return "";

  String rfid = "";
  for (uint8_t i = 0; i < uidLen; i++) {
    if (uid[i] < 0x10) rfid += "0";
    rfid += String(uid[i], HEX);
  }
  rfid.toUpperCase();
  return rfid;
}

// ============================================================
//  SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(LED_PIN, OUTPUT);
  ledOff();

  Serial.println("\n================================");
  Serial.println("  ESP32 Arcade NFC — Modo Serial ");
  Serial.println("================================");

  // Inicializar I2C y PN532
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  nfc.begin();

  uint32_t fw = nfc.getFirmwareVersion();

  if (!fw) {
    Serial.println("ERROR: PN532 no detectado.");
    Serial.println("Revisa cables SDA/SCL, alimentacion 3.3V");
    Serial.println("y que los jumpers esten en modo I2C (ambos ON)");
    while (true) {
      flashLED(1, 100);
      delay(400);
    }
  }

  Serial.printf("PN532 OK — Firmware v%d.%d\n",
                (fw >> 16) & 0xFF, (fw >> 8) & 0xFF);
  nfc.SAMConfig();
  nfcOK = true;

  // Sin WiFi — solo Serial USB
  flashLED(3, 100);
  Serial.println("Listo. Acerca tu tarjeta NFC...\n");
  ledOn(); // LED fijo = listo para leer
}

// ============================================================
//  LOOP
// ============================================================
void loop() {
  if (!nfcOK) return;

  String rfid = leerNFC();

  // Sin tarjeta — seguir esperando
  if (rfid == "") {
    delay(50);
    return;
  }

  unsigned long ahora = millis();

  // Cooldown: evitar leer la misma tarjeta repetidamente
  if (rfid == lastRFID && (ahora - lastReadTime) < COOLDOWN_MS) {
    delay(200);
    return;
  }

  // ── Tarjeta válida — mandar UID por Serial ──
  lastRFID     = rfid;
  lastReadTime = ahora;

  ledOff();

  // Esta es la línea que lee launcher.py
  Serial.print("UID:");
  Serial.println(rfid);

  // Flash de confirmación (2 destellos)
  flashLED(2, 150);

  delay(300);
  ledOn(); // Volver a estado de espera

  Serial.println("Listo para siguiente tarjeta...\n");
}
