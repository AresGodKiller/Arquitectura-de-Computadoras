// Definiciones de Pines
#define PIN_BUTTON_SOS 6
#define PIN_BUTTON_BLINK 7
#define PIN_BUTTON_PULSE 8
#define PIN_LED 9
#define PIN_INTERRUPT 2

// Tiempos en milisegundos (ms)
#define TIME_SLOW 100
#define TIME_FAST 200

// Tipos de Acciones
#define ACTION_NONE 0
#define ACTION_SOS 1
#define ACTION_BLINK 2
#define ACTION_PULSE 3

// Variables de Control
volatile bool paused = false;     // Estado de Pausa/Reanudación
int currentAction = ACTION_NONE;  // Acción actualmente en curso
unsigned long lastMillis = 0;     // Para el control de tiempo (millis())

// Variables de la Máquina de Estados (para el LED)
int ledState = LOW;
int blinkCounter = 0; // Contador de ciclos para SOS y Blink
int sosStage = 0;     // Etapa actual de SOS (0=corto, 1=largo, 2=corto, 3=pausa)

// Variables para el efecto Pulse (PWM)
int pulseValue = 0;
int pulseIncrement = 1;
// Aproximadamente 1.5s subida y 1.5s bajada, ajustado para ser suave.
int pulseStep = 3; // Intervalo de tiempo en ms para cada paso de PWM

// ------------------------------------
// 1. SETUP
// ------------------------------------

void setup() {
  pinMode(PIN_BUTTON_SOS, INPUT_PULLUP);
  pinMode(PIN_BUTTON_BLINK, INPUT_PULLUP);
  pinMode(PIN_BUTTON_PULSE, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  pinMode(PIN_INTERRUPT, INPUT_PULLUP);

  // Configura la interrupción para alternar la pausa en flanco de bajada (al presionar)
  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT), togglePause, FALLING);

  Serial.begin(9600);
  Serial.println("Sistema iniciado. Presiona un botón de acción.");
}

// ------------------------------------
// 2. LOOP (Bucle Principal)
// ------------------------------------

void loop() {
  // A. Manejo de Pausa
  if (paused) {
    // Si está pausado, apaga el LED y sale del loop
    digitalWrite(PIN_LED, LOW);
    return;
  }
  
  // B. Iniciar Nueva Acción (Solo si no hay nada en curso)
  if (currentAction == ACTION_NONE) {
    if (digitalRead(PIN_BUTTON_SOS) == LOW) {
      startAction(ACTION_SOS);
    } else if (digitalRead(PIN_BUTTON_BLINK) == LOW) {
      startAction(ACTION_BLINK);
    } else if (digitalRead(PIN_BUTTON_PULSE) == LOW) {
      startAction(ACTION_PULSE);
    }
  }

  // C. Continuar/Reanudar la Acción (Si hay una acción en curso y NO está pausado)
  if (currentAction != ACTION_NONE) {
    unsigned long currentMillis = millis();

    switch (currentAction) {
      case ACTION_SOS:
        runSOS(currentMillis);
        break;
      case ACTION_BLINK:
        runBlink(currentMillis);
        break;
      case ACTION_PULSE:
        runPulse(currentMillis);
        break;
    }
  }
}

// ------------------------------------
// 3. FUNCIONES DE INTERRUPCIÓN Y AUXILIARES
// ------------------------------------

// Función de interrupción: alterna la pausa/reanudación
void togglePause() {
  // Alterna el estado de 'paused'
  paused = !paused;

  if (paused) {
    Serial.println("--> ACCIÓN PAUSADA.");
  } else {
    Serial.println("--> ACCIÓN REANUDADA.");
  }
}

// Función para inicializar una nueva acción
void startAction(int newAction) {
  // Solo se inicia si no hay una acción en curso
  if (currentAction != ACTION_NONE) return; 
  
  currentAction = newAction;
  lastMillis = millis();
  ledState = LOW;
  blinkCounter = 0;
  sosStage = 0;
  pulseValue = 0;
  pulseIncrement = 1;

  Serial.print("Iniciando acción: ");
  Serial.println(newAction);
}

// Función para terminar la acción
void finishAction() {
  currentAction = ACTION_NONE;
  digitalWrite(PIN_LED, LOW);
  analogWrite(PIN_LED, 0);
  Serial.println("Acción completada.");
}

// ------------------------------------
// 4. FUNCIONES DE ACCIÓN (NO BLOQUEANTES)
// ------------------------------------

void runSOS(unsigned long currentMillis) {
  int delayTime = TIME_SLOW;
  int targetCycles = 3 * 2; // 3 veces ON/OFF = 6 cambios de estado

  // 1. Definir la Etapa de SOS
  if (sosStage == 0 || sosStage == 2) { // Corto (S)
    delayTime = TIME_SLOW;
  } else if (sosStage == 1) { // Largo (O)
    delayTime = TIME_FAST;
  } else if (sosStage == 3) { // Pausa entre repeticiones de SOS
    delayTime = TIME_FAST * 7; // Pausa más larga
    if (currentMillis - lastMillis >= delayTime) {
      lastMillis = currentMillis;
      sosStage = 0; // Reinicia el ciclo completo (S-O-S)
    }
    return;
  }

  // 2. Control de Tiempo
  if (currentMillis - lastMillis >= delayTime) {
    lastMillis = currentMillis;

    // 3. Alternar LED
    ledState = !ledState;
    digitalWrite(PIN_LED, ledState);
    blinkCounter++;

    // 4. Transición de Etapa
    if (blinkCounter >= targetCycles) {
      blinkCounter = 0;
      sosStage++; // Pasa a la siguiente etapa (S->O->S->Pausa)
    }
  }
}

void runBlink(unsigned long currentMillis) {
  int delayTime = TIME_FAST;
  int targetCycles = 7 * 2; // 7 veces ON/OFF = 14 cambios de estado

  // 1. Control de Tiempo
  if (currentMillis - lastMillis >= delayTime) {
    lastMillis = currentMillis;

    // 2. Alternar LED
    ledState = !ledState;
    digitalWrite(PIN_LED, ledState);
    blinkCounter++;

    // 3. Finalizar
    if (blinkCounter >= targetCycles) {
      finishAction();
      return;
    }
  }
}

void runPulse(unsigned long currentMillis) {
  // 1. Control de Tiempo
  if (currentMillis - lastMillis >= pulseStep) {
    lastMillis = currentMillis;

    // 2. Mover el valor de PWM
    pulseValue += pulseIncrement;

    if (pulseValue >= 255) {
      pulseValue = 255;
      pulseIncrement = -1; // Comienza a bajar
    } else if (pulseValue <= 0) {
      pulseValue = 0;
      pulseIncrement = 1; // Comienza a subir

      // Si el pulso va de 0 a 255 y regresa a 0, termina.
      // Aquí, lo hacemos continuo hasta que se interrumpa o cambie de acción.
    }

    // 3. Aplicar PWM
    analogWrite(PIN_LED, pulseValue);
  }
}