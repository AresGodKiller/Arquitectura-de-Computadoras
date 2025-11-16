#define PIN_BUTTON_SOS 6 
#define PIN_BUTTON_BLINK 7
#define PIN_BUTTON_PULSE 8
#define PIN_LED 9
#define PIN_INTERRUPT 2


#define DELAY_SLOW 100
#define DELAY_FAST 200

volatile bool stopFlag = false;  
  
void setup() {
  pinMode(PIN_BUTTON_SOS, INPUT_PULLUP);
  pinMode(PIN_BUTTON_BLINK, INPUT_PULLUP);
  pinMode(PIN_BUTTON_PULSE, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  pinMode(PIN_INTERRUPT, INPUT_PULLUP);


  attachInterrupt(digitalPinToInterrupt(PIN_INTERRUPT), stopAll, FALLING);

  Serial.begin(9600);
}

void loop() {
  stopFlag = false;
  if (digitalRead(PIN_BUTTON_SOS) == LOW) {
    Serial.println("Boton sos presionado");
    stopFlag = false;
    writeSOS();
  }

  if (digitalRead(PIN_BUTTON_BLINK) == LOW) {
    stopFlag = false;
    writeBlink();
  }

  if (digitalRead(PIN_BUTTON_PULSE) == LOW) {
    stopFlag = false;
    writePulse();
  }
}

void stopAll() {
  stopFlag = true; 
  digitalWrite(PIN_LED, LOW); 
}


void writeSOS() {
  for (int i = 0; i < 3 && !stopFlag; i++) {
    Serial.println("inicio sos");
    digitalWrite(PIN_LED, HIGH);
    delayWithStop(DELAY_SLOW);
    digitalWrite(PIN_LED, LOW);
    delayWithStop(DELAY_SLOW);
  }

  delayWithStop(DELAY_SLOW);

  for (int i = 0; i < 3 && !stopFlag; i++) {
    digitalWrite(PIN_LED, HIGH);
    delayWithStop(DELAY_FAST);
    digitalWrite(PIN_LED, LOW);
    delayWithStop(DELAY_FAST);
  }

  delayWithStop(DELAY_SLOW);

  for (int i = 0; i < 3 && !stopFlag; i++) {
    digitalWrite(PIN_LED, HIGH);
    delayWithStop(DELAY_SLOW);
    digitalWrite(PIN_LED, LOW);
    delayWithStop(DELAY_SLOW);
  }

  digitalWrite(PIN_LED, LOW);
}

void writeBlink() {
  for (int i = 0; i < 7 && !stopFlag; i++) {
    digitalWrite(PIN_LED, HIGH);
    delayWithStop(DELAY_FAST);
    digitalWrite(PIN_LED, LOW);
    delayWithStop(DELAY_FAST);
  }
  digitalWrite(PIN_LED, LOW);
}

void writePulse() {
  float seconds = 1.5;
  float increment = 1.0 / seconds;

  for (float value = 0.0; value < 255.0 && !stopFlag; value += increment) {
    analogWrite(PIN_LED, (int)value);
    delayWithStop(1000 / 255);
  }

  for (float value = 255.0; value > 0.0 && !stopFlag; value -= increment) {
    analogWrite(PIN_LED, (int)value);
    delayWithStop(1000 / 255);
  }

  analogWrite(PIN_LED, 0);
}


void delayWithStop(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    if (stopFlag) return;
  }
}